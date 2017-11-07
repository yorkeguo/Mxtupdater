/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file HIDHelper.cpp
@brief Maxtouch HID Helper cpp file
@details
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#include "HIDHelper.h"
#include "i2cBridge.h"

extern HANDLE hI2CDriver;
UINT8 OutputRegisterLow = 0;
UINT8 OutputRegisterHigh = 0;
UINT8 OutputRegisterSize = 0;
UINT8 CommandRegisterLow = 0;
UINT8 CommandRegisterHigh = 0;

BOOL ReadHIDDescriptor(HANDLE file){
	//request HID register, 0x0000
	UINT16 descriptorRegister = 0x0000;
	UINT8 hidDescriptor[HID_DESCRIPTOR_SIZE];
	int result = i2cBridgeWriteRead(file, (UINT8*)&descriptorRegister, 2, hidDescriptor, HID_DESCRIPTOR_SIZE);
		if(result != HID_DESCRIPTOR_SIZE){
	  	DBGPRINT(_T("Unable to read HID Descriptor.\n"));
		return FALSE;
	}

	//store address of Output Register
	OutputRegisterLow = hidDescriptor[12];
	OutputRegisterHigh = hidDescriptor[13];

	//store size of output register
	OutputRegisterSize = (hidDescriptor[15] << 8) | hidDescriptor[14];

	//store address of command register
	CommandRegisterLow = hidDescriptor[16];
	CommandRegisterHigh = hidDescriptor[17];
  
  return TRUE;
}
