/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file  internal.h
@brief This module contains the internal type definitations and helper function declarations for the SpbTestTool app.
@details Environment:user-mode
@copyright Copyright (c) Microsoft Corporation.  All rights reserved.
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#pragma once

using namespace std;

#include <string>
#include <map>
#include <list>
#include <map>
#include <functional>

#include <iomanip>
#include <iostream>
#include <fstream>

#include <cstdlib>

#include <stdexcept>

#include <math.h>

#include <wchar.h>
#include <windows.h>
#include <winioctl.h>
#include <specstrings.h>

#include "spbtestioctl.h"

// Flag to get extra debug printf --> becomes true with undocumented option "-d"
extern BOOL DebugMode;
#define DBGPRINT if (DebugMode) _tprintf
// Flag to get extra debug printf --> becomes true with undocumented option "-dx"
extern BOOL DebugModeEx;
#define DBGPRINTEX if (DebugModeEx) _tprintf

//#define AUTO_UPDATER 1   // Put it in the build pre-processor options instead

#define integerise(a) ((a < 60) ? (a-48) : (a-55))

#ifdef AUTO_UPDATER
#ifdef _DEBUG
#define _TPRINTFD _tprintflog
#else
#define _TPRINTFD //
#endif
#else
#define _TPRINTFD _tprintflog
#endif

#define _TPRINTF _tprintflog
//#else
//#define _TPRINTF _tprintf
//#endif

// ConnectFWType
#define HIDUSB        0   // Standard HID-USB connected device, full inbox support
#define HIDI2C        1   // Standard HID-I2C connected device, full inbox support
#define HIDI2CNOINBOX 2   // HID-I2C connected device, no inbox support for object protocol
#define I2CSTOCK      3   // I2C device, no inbox support at all
#define I2CBRIDGE     4   // I2C bridge board. Could be HID or OBP. Added by Miracle.