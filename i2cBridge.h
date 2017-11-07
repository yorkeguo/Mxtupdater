/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file i2cBridge.h
@brief I2C functions header file
@details I2C hardware interface prototype functions and definitions
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#ifndef I2CBRIDGE_H_INCLUDED
#define I2CBRIDGE_H_INCLUDED


#include <stdint.h>
#include "internal.h"
#include <tchar.h>

//extern HANDLE fileNormal;

int i2cBridgeInit(HANDLE file);
int i2cBridgeWrite(HANDLE file, UINT8 *data, UINT16 length);
int i2cBridgeRead(HANDLE file, UINT8 *data, UINT16 length);
int i2cBridgeWriteRead(HANDLE file, UINT8* writeBuffer, UINT16 writeLength, UINT8* readBuffer, UINT16 readLength);
int WriteToMxt(HANDLE file, UINT16 addr, UINT16 size, UINT8* buffer);
int ReadFromMxt(HANDLE file, UINT16 addr, UINT16 size, UINT8* buffer);
void exitObjectProtocolMode(HANDLE file);

BOOL ReadFileIO(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped);
BOOL WriteFileIO(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);

#endif /*I2CBRIDGE_H_INCLUDED*/
