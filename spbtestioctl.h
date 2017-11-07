/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file  spbtestioctl.h
@brief Device path names and IOCTL definitions for the SpbTestTool.
@details This module contains the public device path names and IOCTL definitions for the SpbTestTool.
@copyright Copyright (c) Microsoft Corporation.  All rights reserved.
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/



#ifndef _SPBTESTIOCTL_H_
#define _SPBTESTIOCTL_H_

//
// Device path names
//

#define MXTBRIDGE_NAME L"MXTBRIDGE"

#define MXTBRIDGE_SYMBOLIC_NAME L"\\DosDevices\\" MXTBRIDGE_NAME
#define MXTBRIDGE_USERMODE_PATH L"\\\\.\\" MXTBRIDGE_NAME
#define MXTBRIDGE_USERMODE_PATH_SIZE sizeof(MXTBRIDGE_USERMODE_PATH)

#define MXTBRIDGEBOOT_NAME L"MXTBOOTBRIDGE"

#define MXTBRIDGEBOOT_SYMBOLIC_NAME L"\\DosDevices\\" MXTBRIDGEBOOT_NAME
#define MXTBRIDGEBOOT_USERMODE_PATH L"\\\\.\\" MXTBRIDGEBOOT_NAME
#define MXTBRIDGEBOOT_USERMODE_PATH_SIZE sizeof(MXTBRIDGEBOOT_USERMODE_PATH)

#define MXTBRIDGEBOOT2_NAME L"MXTBOOTBRIDGE2"

#define MXTBRIDGEBOOT2_SYMBOLIC_NAME L"\\DosDevices\\" MXTBRIDGEBOOT2_NAME
#define MXTBRIDGEBOOT2_USERMODE_PATH L"\\\\.\\" MXTBRIDGEBOOT2_NAME
#define MXTBRIDGEBOOT2_USERMODE_PATH_SIZE sizeof(MXTBRIDGEBOOT2_USERMODE_PATH)

//
// Private SpbTestTool IOCTLs
//

#define FILE_DEVICE_SPB_PERIPHERAL 0x400

#define IOCTL_SPBTESTTOOL_OPEN              CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x700, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_CLOSE             CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x701, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_LOCK              CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x702, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_UNLOCK            CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x703, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_WRITEREAD         CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x704, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_LOCK_CONNECTION   CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x705, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_UNLOCK_CONNECTION CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x706, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_SIGNAL_INTERRUPT  CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x707, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_WAIT_ON_INTERRUPT CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x708, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_FULL_DUPLEX       CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x709, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_READ              CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x70A, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SPBTESTTOOL_WRITE             CTL_CODE(FILE_DEVICE_SPB_PERIPHERAL, 0x70B, METHOD_BUFFERED, FILE_ANY_ACCESS)

#endif _SPBTESTIOCTL_H_