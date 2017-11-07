/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file BootWorker.cpp
@brief Maxtouch BootWorker cpp file
@details MxtUpdater app bootworker functions
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#include <fstream>

#include "internal.h"
#include "util.h"
#include "i2cBridge.h"
#include "BootWorker.h"

BOOL debug_output = false;

#define BOOT_RESET_CMD 0x0A

extern HANDLE hI2CDriver;
extern HANDLE hI2CDriverBootloader;
extern TCHAR EncFile[MAX_PATH];

extern UINT8 CommandRegisterLow;
extern UINT8 CommandRegisterHigh;

extern UCHAR *FWFileRes;
extern DWORD FWFileSize;
extern CHAR *CfgFileRes;
extern DWORD CfgFileSize;

extern TCHAR PrintBuffer[512];

extern BOOL MaxTouchAnalyzer;
extern BOOL MaxTouchStudio;

extern BOOL ErrorCodes;
extern BOOL SilentMode;

#ifdef NEW_OLD_BLDR
extern UCHAR *FWFileRes2;
extern DWORD FWFileSize2;
#endif

extern const TCHAR *version_decoder_name[NUM_BOOTLOADER_IDS];
extern BOOL DeviceBlrFromXML;
extern TCHAR DeviceBtlNameFromXML[32];


//used for storing FW data
static UINT8 frame[1024];
static UINT32 fileSize;
UINT ByteCounter = 0;

UINT8 RealBootloaderId, BootloaderId;

DWORD Delay = 0;
DWORD FrameDelay = 0;

BOOL SwitchToBootloader(void)
{
  int result;
  //write 'reset to bootloader' command to command register
  UINT8 command[] = {CommandRegisterLow, CommandRegisterHigh, 1, BOOT_RESET_CMD};
  result = i2cBridgeWrite(hI2CDriver, command, 4);

  if(result != 4){
    DBGPRINT(_T("ERROR: Unable to issue RESET command to Command Register. Wrote %i bytes, expected %i\n"), result, 4);
    return FALSE;
  }
  return TRUE;
}


BOOL BootWorkerUnlock(void)
{
  UINT8 command[2];
  command[0] = 0xDC;
  command[1] = 0xAA;

  int result = i2cBridgeWrite(hI2CDriverBootloader, command, 2);
  if(result != 2) return FALSE;
  return TRUE;
}

BOOL ExitBootloader(void)
{
  UINT8 command[2];
  command[0] = 0xDD;
  command[1] = 0x55;

  int result = i2cBridgeWrite(hI2CDriverBootloader, command, 2);
  if(result != 2) return FALSE;
  return TRUE;
}

BOOL BootWorkerAnyState()
{
  UINT8 device_status = 0;

  //read current state of device
  if(i2cBridgeRead(hI2CDriverBootloader, &device_status, 1) != 1) {
    DBGPRINT(_T("BW: WaitForState: Failed to read state of device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  return TRUE;
}

BOOL BootWorkerWaitForState(UINT8 state, BOOL Quiet)
{
  UINT8 device_status[3];
  TCHAR printBuffer1[512];
  TCHAR printBuffer2[512];
  UINT16 ReportedBootloaderId;
  UINT8 BootloaderVersion;
  INT iRetCode;
  string name;

  UNREFERENCED_PARAMETER(iRetCode);

  BOOL bRecheck = FALSE;

  do {
    //read current state of device
    if (Delay) Sleep(Delay);
    if(i2cBridgeRead(hI2CDriverBootloader, device_status, 3) != 3) {
      DBGPRINT(_T("BW: WaitForState: Failed to read state of device. Error: %d\n"), GetLastError());
      return FALSE;
    }

    //process device status
    switch(state) {

    case MXT_WAITING_BOOTLOAD_CMD:
      BootloaderId = device_status[1];
      BootloaderVersion = device_status[2];

#ifdef NEW_OLD_BLDR
      if (BootloaderId == 36) {
        FWFileRes = FWFileRes2;
        FWFileSize = FWFileSize2;
      }
#endif
      ReportedBootloaderId = BootloaderId;
      RealBootloaderId = BootloaderId;
      ReportedBootloaderId <<= 8u;
      ReportedBootloaderId |= BootloaderVersion;
      BootloaderId += 32u;
      //update version number on screen
//      if(BootloaderId >= NUM_BOOTLOADER_IDS) {
//        DBGPRINT(_T("Unknown Bootloader ID %d\n"), BootloaderId);
         // unknown bootloader 
//        BootloaderId = 0u;
//      }
      if (BootloaderId) {
#ifdef AUTO_UPDATER
        DeviceBlrFromXML = FALSE;
#else
        iRetCode = GetDeviceBtlName(RealBootloaderId, name);
        // check if success...
        if (!iRetCode) {     //if not successful, use UNKNOWN BOOTLOADER
          _tcscpy_s(DeviceBtlNameFromXML, CA2T(name.c_str()));
          DeviceBlrFromXML = TRUE;  // it either from external XML file or from internal XML resource
        } else {
          DeviceBlrFromXML = FALSE;
        }
#endif
      }

      if (DeviceBlrFromXML) {
        // print up the chip info 
        _stprintf(printBuffer1, _T("Bootloader Type: %s - Bootloader ID:%d"), DeviceBtlNameFromXML, RealBootloaderId);
      } else {
	      BootloaderId = 0;
        // print up the chip info 
        _stprintf(printBuffer1, _T("Bootloader Type: %s"), version_decoder_name[BootloaderId]);
      }
      // add the version code for extended mode chips
      _stprintf(printBuffer2, _T("%s (Version:%d)"), printBuffer1, BootloaderVersion);
      // print up the raw ID info and I2C address 
      _stprintf(printBuffer1, _T("%s\n(ID: 0x%X)\n"), printBuffer2, ReportedBootloaderId);

      _stprintf(PrintBuffer, _T("%s\n"), printBuffer1);
      if (!Quiet) {
        _TPRINTFD(PrintBuffer);
      }
    case MXT_WAITING_FRAME_DATA:
      device_status[0] &= ~MXT_BOOT_STATUS_MASK;
      break;

    case MXT_FRAME_CRC_PASS:
      if(device_status[0] == MXT_FRAME_CRC_CHECK){
        //device is not done checking the CRC, wait a little then check again
        //if(debug_output) DBGPRINT(_T("BW: WaitForState: Waiting for CRC. Recheking...\n");
        if (FrameDelay) Sleep(FrameDelay);  // use -fs=100 to get the same
//        Sleep(100); //TODO Too long a sleep
        bRecheck = TRUE;
      } else {
        bRecheck = FALSE;
      }
      break;

    default:
      DBGPRINT(_T("BW: WaitForState: Unsupported state requested\n"));
      return FALSE;
    }
  } while(bRecheck);

  //check if status matches the one we waited for
  if(device_status[0] != state){
    UINT8 printByte = device_status[0];
    if(printByte & MXT_BOOT_STATUS_MASK)
      printByte &= ~MXT_BOOT_STATUS_MASK;
    DBGPRINT(_T("BW: WaitForState: Device is in incorrect state: 0x%x, expected: 0x%x\n"), printByte, state);
    return FALSE;
  }
  return TRUE;
}

BOOL ResetBootloaderMode(){
  //to reset device, send a frame with length field set to 0x0000
  UINT16 length = 0;

  int status = i2cBridgeWrite(hI2CDriverBootloader, (UINT8*)&length, 2);
  if (status != 2) {
    DBGPRINT(_T("BW: Failed issuing reset command.Error: %d\n"), GetLastError());
    return FALSE;
  }
  return TRUE;
}


BOOL BootWorkerWriteFrame(UINT8 *Frame, UINT16 Length, DWORD WaitTime)
{
  int ret, i;
  /* This would normally do:
  *
  * Check Change line to see that we are in idle
  * Push frame over I2C
  * Wait for change line to not idle
  * Read back CRC
  */
  //wait for device to be ready for new frame

  if (WaitTime) Sleep(WaitTime);

  for (i = 0; i < 10; i++) {   // Add by Miracle. Add delay if need more time to wait frame ready.
    if (i > 0) Sleep(300);
    if (!BootWorkerWaitForState(MXT_WAITING_FRAME_DATA, TRUE)) {
      DBGPRINT(_T("BW: Failed reading Waiting Frame Data state\n"));
      //_TPRINTF(_T("BW: Failed reading Waiting Frame Data state\n"));
      // return FALSE;
      continue;
    }
    break;
  }

  if (i == 10) {
    _TPRINTFD(_T("BW: Failed reading Waiting Frame Data state\n"));
    return FALSE;
  }

  //real write frame
  DBGPRINT(_T("BW: Writing frame of length %d\n"), Length);
  ret = i2cBridgeWrite(hI2CDriverBootloader, Frame, Length);
  if (ret != Length) {
    DBGPRINT(_T("BW: Failed writing frame. Wrote %d bytes, expected %d\n"), ret, Length);
    return FALSE;
  }

  DBGPRINT(_T("\nBW: waiting for CRC!\n"));

  if(FrameDelay) Sleep(FrameDelay);
//  Sleep(200);

  //check that CRC passed
  if (!BootWorkerWaitForState(MXT_FRAME_CRC_PASS, TRUE)) {
    DBGPRINT(_T("BW: CRC failed!\n"));
    return FALSE;
  }
  return TRUE;
}

// Return codes:
// -1: generic error
// if error codes required, negative code returned with error
// 0: success
int WriteFWToDevice()
{
  int iRetCode = -1; // set as generic error
  BOOL bFirstTimeWFW = TRUE;
  UINT percentDone = 0; 
  ifstream encFWFile;
  DWORD WaitTime = 0;
  UINT32 totalFileSize;

#ifndef AUTO_UPDATER
  encFWFile.open(EncFile, ifstream::in);

  if (!encFWFile.is_open()) {
    _stprintf(PrintBuffer, _T("Error opening binary file: %s. Cannot proceed. \n"), EncFile);
    _TPRINTF(PrintBuffer);
    if (ErrorCodes) iRetCode = FW_FILE_OPEN_ERROR;
    return iRetCode;
  }
  //get total filesize
  encFWFile.seekg(0, std::ios::end);
  totalFileSize = (UINT32)encFWFile.tellg() / 2; //divide by two because of ASCII encoding of enc files
  encFWFile.seekg(0);

#else
   totalFileSize = FWFileSize / 2;  //divide by two because of ASCII encoding of enc files
#endif

  //start writing fw frames
  fileSize = 0;

  for(;;) { 
    /* Read block of data. */
    UINT16 blockLength;

    blockLength = ((UINT16)readNibble(&encFWFile)) << (4*3);
    blockLength |= ((UINT16)readNibble(&encFWFile)) << (4*2);
    blockLength |= ((UINT16)readNibble(&encFWFile)) << (4*1);
    blockLength |= ((UINT16)readNibble(&encFWFile)) << (4*0);

    if (blockLength == 0) {
//      _stprintf(PrintBuffer, _T("\b\b\b\b\b\b\b\b100 %% done\n"));  //Sometimes we get a strange output...
      _stprintf(PrintBuffer, _T("\rDownloading firmware completed\n"));
      _TPRINTF(PrintBuffer);
      _stprintf(PrintBuffer, _T("If the program gets stuck here longer than 10-15 sec. please hit CTRL+c.\nYou can perform \"MxtUpdater -de\" and \"MxtUpdater -i\" to check status.\n"));
      _TPRINTFD(PrintBuffer);
      _stprintf(PrintBuffer, _T("To prevent getting stuck, you can add \"-de\" at the end of the command line.\nOccasionally it may be necessary to power-cycle.\n\n"));
      _TPRINTFD(PrintBuffer);
      iRetCode = SUCCESS;
      break;
    }

    UINT16 i = 0;
    //add framelength to frame!
    frame[i++] = (blockLength & 0xFF00) >> 8;
    frame[i++] = (blockLength & 0x00FF) >> 0;


    while (blockLength) {
      UINT8 byte = readNibble(&encFWFile) << (4*1);
      byte |= readNibble(&encFWFile) << (4*0);
      frame[i] = byte;
      ++i;
      ++fileSize;
      --blockLength;
    }
    // Add a wait here: some flashing fails otherwise...
    if(FrameDelay) {
     if (percentDone > 95) {
        WaitTime = 300;
      }
    } 
    if (!BootWorkerWriteFrame(frame, i, WaitTime)) {
      _stprintf(PrintBuffer, _T("\nFailed to write frame to device - aborting\n"));
      _TPRINTFD(PrintBuffer);
      if (ErrorCodes) iRetCode = FW_UPDATE_FAILED_FRAMES;
     break;
    }

    DBGPRINT(_T("Wrote frame of length %d\n"), i);
    DBGPRINT(_T("-- %dB / %dB, %d%% --\n\n"), fileSize, totalFileSize, (fileSize*100)/totalFileSize);
    if(bFirstTimeWFW) {
      if (!SilentMode) _tprintf(_T("Downloading firmware 00%c done"), '%');
      if (MaxTouchAnalyzer || MaxTouchStudio) _TPRINTF(_T("\n"));
	  bFirstTimeWFW = false;
    } else {
      if (MaxTouchAnalyzer || MaxTouchStudio) {
        if ((fileSize*100)/totalFileSize != percentDone) {
          _tprintf(_T("%2d%c done"), ((fileSize*100)/totalFileSize), '%');
          _TPRINTF(_T("\n"));
        }
      } else {
        if ((fileSize*100)/totalFileSize != percentDone) {
          if (!SilentMode) _tprintf(_T("\b\b\b\b\b\b\b\b%2d%c done"), ((fileSize*100)/totalFileSize), '%');
        }
      }
      percentDone = (fileSize*100)/totalFileSize;
    }
  }

#ifndef AUTO_UPDATER
  //close .enc file handle
  encFWFile.close();
#endif

  return(iRetCode);
}

static UINT8 readNibble(ifstream *file)
{
char temp;

#ifndef AUTO_UPDATER
  if (!file->is_open()) {
    return 0;
  }

  if (!file->good()) {
    return 0;
  }

  temp = (char)file->get();

  if (temp == -1 || (temp < '0' || temp > 'f')) {
    return 0;
  }

#else
  UNREFERENCED_PARAMETER(file);
  // get temp
  if (ByteCounter < FWFileSize) {
    temp = (UINT8)*FWFileRes++;
    ++ByteCounter;
  } else {
    return 0;
  }

#endif
  return (integerise(temp));
}

