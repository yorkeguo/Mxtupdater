/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file i2cBridge.cpp
@brief Maxtouch I2C Brige cpp file
@details I2C interface functions
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#include "util.h"
#include "HIDHelper.h"
#include "i2cBridge.h"

HANDLE hI2CDriver = INVALID_HANDLE_VALUE;
HANDLE hI2CDriverBootloader = INVALID_HANDLE_VALUE;
BOOL SmallBufferIntel = FALSE;

extern UINT ConnectFWType;
extern TCHAR PrintBuffer[512];
extern BOOL IsWin7;

UINT MaxBuffSize = 32;

int i2cBridgeInit(HANDLE file)
{
  /* Return <0 upon error - 0 on success loading the I2C bridge driver. */
  ULONG bytesReturned;
  BOOL result;
  //open and lock coms with normal driver
  result = DeviceIoControl(file, IOCTL_SPBTESTTOOL_OPEN, nullptr, 0, nullptr, 0, &bytesReturned, nullptr);
  //result &= DeviceIoControl(fileNormal, IOCTL_SPBTESTTOOL_LOCK, nullptr, 0, nullptr, 0, &bytesReturned, nullptr);//&Overlapped
  if(!result) {
    DBGPRINT(_T("i2cBridge: Failed to open I2C driver. Error #: %d\n"), GetLastError());
    if (GetLastError() == ERROR_INVALID_PARAMETER) {
      _stprintf(PrintBuffer, _T("\nError: possibly out-of-date ACPI table for ATML drivers.\n\n"));
      _TPRINTF(PrintBuffer);
      return -1;
    }
    if (GetLastError() == ERROR_BUSY) {
      _stprintf(PrintBuffer, _T("\nDriver conflict. Please try disabling 'HID-I2C Device' (from Device Manager).\n\n"));
      _TPRINTF(PrintBuffer);
      return -1;
    }
    return -1;
  }
  return 0;
}

int i2cBridgeWrite(HANDLE file, UINT8 *data, UINT16 length)
{
  /* Write bytes onto the I2C bus. Return number of bytes sent - or <0 on error. */

  ULONG bytesWritten, totalBytesWritten = 0;
  UINT i = 0, chunk;

  // Added by Miracle.
  UINT8 num_write;
  uint16_t num_total;

  if(ConnectFWType==I2CBRIDGE) {
    num_total = length;
    for(;num_total > 0;) {    // split to small i2c frame.
      if(num_total > 50)
        num_write = 50;
      else
        num_write = (UINT8)(num_total&0xFF);
      if(file==hI2CDriverBootloader) {
        if(! I2CBridge_I2C_ReadWrite(num_write, data, 0, NULL))
          return -1;
      }
      else {
        if(! I2CBridge_ReadWrite(num_write, data, 0, NULL))
          return -1;
      }
      data += num_write;
      num_total -= num_write;
    }

    return length;
  }

  if (SmallBufferIntel) {
    while (i < length) {
      //    Sleep(50);
      chunk = length - i;
      if (chunk > MaxBuffSize) {
        chunk = MaxBuffSize;
      }

      BOOL result = WriteFileIO(file, &data[i], chunk, &bytesWritten, nullptr);
      if(!result) {
        DBGPRINT(_T("i2cBridge: Failed to write data to bridge. Error #: %d\n"), GetLastError());
        return -1;
      }
      i += chunk;
      totalBytesWritten += bytesWritten;
    }
  } else {
    BOOL result = WriteFileIO(file, data, length, &totalBytesWritten, nullptr);
    if(!result) {
      DBGPRINT(_T("i2cBridge: Failed to write data to bridge. Error #: %d\n"), GetLastError());
      return -1;
    }
  }
  return totalBytesWritten;
}


int i2cBridgeRead(HANDLE file, UINT8 *data, UINT16 length)
{
  /* Return number of bytes read - ir <0 upon error. */
  ULONG bytesRead;

  // Added by Miracle.
  if(ConnectFWType == I2CBRIDGE) {

    if(file==hI2CDriverBootloader) {
      if(! I2CBridge_I2C_ReadWrite(0,  NULL, (UINT8)(length & 0xFF), data)) {
        DBGPRINT(_T("i2cBridgeRead: Failed to Read data. request: %d bytes.\n"), length);
        return -1;
      }
    }
    else {
      if(! I2CBridge_ReadWrite(0,  NULL, (UINT8)(length & 0xFF), data)) {
        DBGPRINT(_T("i2cBridgeRead: Failed to Read data. request: %d bytes.\n"), length);
        return -1;
      }
    }

    return length;
  }

  BOOL result = ReadFileIO(file, data, length, &bytesRead, nullptr);
  if(!result) {
    DBGPRINT(_T("i2cBridge: Failed to Read data from bridge. Error #: %d\n"), GetLastError());
    // 22 == ERROR_BAD_COMMAND
    return -1;
  }
  return bytesRead;
}

int i2cBridgeWriteRead(HANDLE file, UINT8* writeBuffer, UINT16 writeLength, UINT8* readBuffer, UINT16 readLength)
{
  ULONG bytesTransferred = 0;

  BOOL result = DeviceIoControl(file, 
    IOCTL_SPBTESTTOOL_WRITEREAD,
    writeBuffer,
    writeLength,
    readBuffer,
    readLength,
    &bytesTransferred,
    nullptr);

  if(!result) {
    //There was an error
    DBGPRINT(_T("i2cBridge: Failed to WriteRead data to bridge. Error #: %d\n"), GetLastError());
    return -1;
  }
  return bytesTransferred;
}

int ReadFromMxt(HANDLE file, UINT16 addr, UINT16 size, UINT8* buffer)
{
  int result;

  if (ConnectFWType == HIDI2CNOINBOX) {
    //prepare HID message that will move Object Protocl pointer to right address
    UINT8 hidMessageBuffer[7];
    int hidMessageSize = 2 + 2 + 1 + 2; // Register ID + message size + Report ID + object protocol address 
    hidMessageBuffer[0] = OutputRegisterLow;  //Register ID, Low byte      TODO: check if the order of these are correct
    hidMessageBuffer[1] = OutputRegisterHigh; //Register ID, High byte
    hidMessageBuffer[2] = (hidMessageSize - 2) & 0x00FF; //message size, low byte. Whole message, minus two bytes for Register ID
    hidMessageBuffer[3] = ((hidMessageSize - 2) >> 8) & 0x00FF;     //message size, high byte
    hidMessageBuffer[4] = HID_OBJECT_PROTOCOL_TUNNEL_REPORT_ID; //Report ID of object protocol tunnel
    hidMessageBuffer[5] = addr & 0x00FF; //object protocol address, low byte
    hidMessageBuffer[6] = addr >> 8;     //object protocol address, high byte

    //perform a WriteRead operation
    result = i2cBridgeWriteRead(file, hidMessageBuffer, 7, buffer, size);
  } else {
    UINT8 address[2];
    address[0] = addr & 0xFF;
    address[1] = (addr>>8) & 0xFF;
    result = i2cBridgeWrite(file, address, 2);
    if (result != 2) {
      return -1;
    }
    result = i2cBridgeRead(file, buffer, size);
  }
  return result;
}

int WriteToMxt(HANDLE file, UINT16 addr, UINT16 size, UINT8* buffer)
{
  int result;

  if (ConnectFWType == HIDI2CNOINBOX) {
    //create buffer to hold HID Output command plus object protocol command
    int hidMessageSize = 2 + 2 + 1 + 2; // Register ID + message size + Report ID + object protocol address
    hidMessageSize += size; // + payload size 
    UINT8* hidMessageBuffer = new UINT8[hidMessageSize];

    hidMessageBuffer[0] = OutputRegisterLow;  //Register ID, Low byte      TODO: check if the order of these are correct
    hidMessageBuffer[1] = OutputRegisterHigh; //Register ID, High byte
    hidMessageBuffer[2] = (hidMessageSize - 2) & 0x00FF; //message size, low byte. Whole message, minus two bytes for Register ID
    hidMessageBuffer[3] = ((hidMessageSize - 2) >> 8) & 0x00FF;     //message size, high byte
    hidMessageBuffer[4] = HID_OBJECT_PROTOCOL_TUNNEL_REPORT_ID; //Report ID of object protocol tunnel
    hidMessageBuffer[5] = addr & 0x00FF; //object protocol address, low byte
    hidMessageBuffer[6] = addr >> 8;     //object protocol address, high byte

    //copy object protocl message into buffer
    if(size){
      memcpy(&hidMessageBuffer[7], buffer, size);
    }

    //write register to device
    result = i2cBridgeWrite(file, hidMessageBuffer, (UINT16)hidMessageSize);

    delete[] hidMessageBuffer;

    return result - 7;
  }

  //create new buffer to hold both data and address
  UINT8* pExtraBuffer = new UINT8[size + 2];
  //load register address bytes
  pExtraBuffer[0] = addr & 0xFF;
  pExtraBuffer[1] = (addr>>8) & 0xFF;
  //load payload data
  memcpy(&pExtraBuffer[2], buffer, size);

  //write entire buffer to device
  result = i2cBridgeWrite(file, pExtraBuffer, size + 2);
  delete[] pExtraBuffer;

  return result - 2;
}

void exitObjectProtocolMode(HANDLE file)
{
  //write address of HID Descriptor register to exit object protocol mode
  UINT16 reg = 0x0000;
  i2cBridgeWrite(file, (UINT8*)&reg, 2);
}

BOOL ReadFileIO(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
  BOOL result;
  UNREFERENCED_PARAMETER(lpOverlapped);

  if (IsWin7) {
    result = DeviceIoControl(hFile, IOCTL_SPBTESTTOOL_READ, nullptr, 0, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, nullptr);
    return result;
  }
  return (ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped));
}

BOOL WriteFileIO(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
  BOOL result;
  UNREFERENCED_PARAMETER(lpOverlapped);
  DWORD Temp = nNumberOfBytesToWrite;

  if (IsWin7) {
    result = DeviceIoControl(hFile, IOCTL_SPBTESTTOOL_WRITE, lpBuffer, nNumberOfBytesToWrite, nullptr, 0, lpNumberOfBytesWritten, nullptr);
    if (result) *lpNumberOfBytesWritten = Temp;
    return result;
  }
  return (WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped));
}