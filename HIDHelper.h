/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file HIDHelper.h
@brief HID Helper header file
@details 
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#ifndef HIDHELPER_H_INCLUDED
#define HIDHELPER_H_INCLUDED


#include "internal.h"

#define HID_DESCRIPTOR_SIZE 24
#define HID_OBJECT_PROTOCOL_TUNNEL_REPORT_ID 6

extern UINT8 OutputRegisterLow;
extern UINT8 OutputRegisterHigh;
extern UINT8 OutputRegisterSize;

BOOL ReadHIDDescriptor(HANDLE file);


#endif