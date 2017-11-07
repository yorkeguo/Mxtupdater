/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file BootWorker.h
@brief Boot Loader functions header file
@details bootloader functions prototypes and definitions
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#ifndef BOOTWORKER_H_INCLUDED
#define BOOTWORKER_H_INCLUDED


#include "internal.h"


#define MXT_WAITING_BOOTLOAD_CMD 0xC0
#define MXT_WAITING_FRAME_DATA 0x80
#define MXT_FRAME_CRC_CHECK 0x02
#define MXT_FRAME_CRC_PASS 0x04
#define MXT_BOOT_STATUS_MASK 0x3F


int SwitchToBootloader(void);
BOOL BootWorkerUnlock(void);
BOOL ExitBootloader(void);
BOOL BootWorkerWriteFrame(UINT8 *Frame, UINT16 Length, DWORD WaitTime = 0);
BOOL BootWorkerWaitForState(UINT8 state, BOOL Quiet=FALSE);
BOOL ResetBootloaderMode(void);
static UINT8 readNibble(ifstream *file);
int WriteFWToDevice();
BOOL BootWorkerAnyState();
#endif /*BOOTWORKER_H_INCLUDED*/
