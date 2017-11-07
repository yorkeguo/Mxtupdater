/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file stdafx.h
@brief Include file for standard system files
@details Project specific include files that are used frequently, but are changed infrequently
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#pragma once

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>

#ifdef ADD_MFC
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS         // remove support for MFC controls in dialogs

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            				// Exclude rarely-used stuff from Windows headers
#endif

#include <afx.h>
#include <afxwin.h>         					// MFC core and standard components
#include <afxext.h>         					// MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           				// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                    			// MFC support for Windows Common Controls
#endif 											// _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#endif