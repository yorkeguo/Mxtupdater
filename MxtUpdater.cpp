/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file MxtUpdater.cpp
@brief Maxtouch Updater main cpp file
@details Defines the entry point for the console application.
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/

#include "stdafx.h"
#include <basetyps.h>
#include <stdlib.h>
#include <wtypes.h>
#include <setupapi.h>
extern "C" {
#include <hidsdi.h>
}
#include "hid.h"
#include <tchar.h>
#include <strsafe.h>
#include <intsafe.h>
#include <process.h>
#include "Util.h"
#include "BootWorker.h"
#include "resource1.h"

#pragma warning(disable: 4996)

#ifdef AUTO_UPDATER
#include "resource.h"
#endif

extern TCHAR EncFile[MAX_PATH];
extern TCHAR CfgFile[MAX_PATH];
extern TCHAR ThqaFile[MAX_PATH];
extern TCHAR SerialNumFile[MAX_PATH];
extern SECURITY_ATTRIBUTES ReadWriteEventAttribs;
extern SECURITY_ATTRIBUTES CompletionEventAttribs;
extern HANDLE hReadWriteEvent;
extern HANDLE hCompletionEvent;
extern BOOL bStateOk;

//These are the vendor and product IDs to look for.
//Uses ATMEL's Vendor ID.
extern int PRODUCTID[2];
extern int BOOTLOADER_ID;
extern const TCHAR *ATMEL_CTRL_STRING;
extern const TCHAR *ATMEL_BOOTLOADER_STRING[];
extern const TCHAR *HIDI2C_DEVICE_STRING[];
extern const TCHAR *IDENTIFY_CONNECTION_MSG[];
extern const TCHAR *DEVICE_NAME[];
extern const TCHAR *FromXmlFileMsg;
extern UINT DeviceConnected;
extern int DeviceDiscovered;
extern UINT8 AtmelReportId;
extern UINT ConnectFWType;
extern BOOL bIsXCfg;
extern BOOL bIsRaw;
extern BOOL ConfigObjectNotFoundError;
extern UINT DeviceIndex;
extern UINT DevicePID;			// 2016/06/06. Miracle. HIDI2C BL.
extern UINT8 ReportIds[];
extern UINT UsbI2c;

// Extern
#ifdef AUTO_UPDATER
extern UINT8 *CfgFileResAll[MAX_PAYLOADS];
extern UINT8 *ThqaFileResAll[MAX_PAYLOADS];
extern DWORD FWFileSizeAll[MAX_PAYLOADS];
extern DWORD CfgFileSizeAll[MAX_PAYLOADS];
extern DWORD ThqaFileSizeAll[MAX_PAYLOADS];
extern UINT8 *FWFileRes;
extern UINT8 *CfgFileRes;
extern UINT8 *ThqaFileRes;
extern DWORD FWFileSize;
extern DWORD CfgFileSize;
extern DWORD ThqaFileSize;
extern BOOL CheckACPower;
extern UINT RebootNeeded;
extern BOOL PauseBeforeExit;
extern BOOL WriteThqa;
extern BOOL TurnOnThqa;
extern UINT PidAll[MAX_PAYLOADS];
#else
extern UINT8 *XMLFileRes;
extern DWORD XMLFileSize;
BOOL LoadXMLFile();
#endif

extern UINT PidToWrite;
BOOL ReadGPIOPins(UINT8 *GPIOPins);

#ifdef AUTO_UPDATER
TCHAR CombinedVersion[32];
TCHAR OpenMsg[1024];
BOOL CheckDevicePid = FALSE;
UINT PidOrGPIOForCfg = 0;
UINT8 GPIOMask = 0; 
UINT ExpectedPid;
UINT ExpectedVersion;
UINT ExpectedVersionAll[MAX_PAYLOADS];
UINT32 ExpectedChecksum;
UINT32 ExpectedChecksumAll[MAX_PAYLOADS];
INT NumberOfPayloads;
UINT PayloadNum;
BOOL PidFound = FALSE;

BOOL IsFWUpdateRequested = FALSE;
BOOL FWNotNeeded = FALSE;
BOOL CfgNotNeeded = FALSE;
BOOL ThqaNotNeeded = FALSE;

BOOL WorkOnResources();
void ShowVersion();
BOOL IsFWVersionSame();
BOOL IsCfgVersionSame();
BOOL IsCfgChecksumSame();
BOOL IsThqaSame();
BOOL RestartSystem = TRUE;
BOOL SpecialWrite(UINT8 ObjectType, UINT8 Value);
#endif
BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
void SystemReboot(BOOL Restart = TRUE);
#define BUFFER_SIZE 108
size_t   iserial;

#ifdef MXTUPDATER_SVC
int MainEntry(int argc, _TCHAR *argv[], TCHAR *LogFileFullName = NULL);
#endif

extern BOOL SmallBufferIntel;
// To be used if there is a MxtUpdater.ini file
extern TCHAR Custom_DevicePath[64];
extern BOOL IsCustomDevicePath;

extern BOOL DeviceAlreadyIdentified;
extern BOOL USBReadDelay;
extern DWORD Delay;
extern DWORD FrameDelay;
extern UINT MaxBuffSize;

extern UINT32 ConfigChecksum; // Holds the checksum calculated from the config file to be written

extern BOOL DebugMode;
extern BOOL LogMode;
extern BOOL LogModeSvc;
extern BOOL SilentMode;
extern fstream LogFile;
extern const TCHAR *LogFileName;
extern const TCHAR *LogFileSuccess;
extern const TCHAR *LogFileFail;
extern const TCHAR *LogFileAuto;

extern BOOL MaxTouchAnalyzer;
extern BOOL MaxTouchStudio;

extern BOOL ErrorCodes;

extern UINT8 BootloaderId; //, ReportedBootloaderId, BootloaderVersion;
extern const TCHAR *version_decoder_name[NUM_BOOTLOADER_IDS];

extern BOOL CheckBootloaderOnly;
extern BOOL LeaveInBootloaderMode;
extern TCHAR PrintBuffer[512];
extern BOOL VerifyConfigEnabled;
extern BOOL ClearConfig;

extern UINT8 MsgProcSize;
extern UINT AllPids[];
extern BOOL UseCustomString;

extern BOOL XMLFilePresent;
extern BOOL DeviceAppFromXML;
extern BOOL DeviceBlrFromXML;
extern TCHAR DeviceAppNameFromXML[32];
extern TCHAR DeviceBtlNameFromXML[32];

extern vector<OPObject*> ObjectsInFile, ObjectsInFW;

char CustomString[512];

#ifdef OCULAR_TOSHIBA
UINT FWRetries = 0;
#endif

TCHAR PathIniFile[MAX_PATH];

ULONG DevicePathId = 0;

void ShowUsage();
int ConfigUpdate();
BOOL ReadConfig();
BOOL FindAtmelDevice(int Order = FIRST_DISCOVERY, BOOL Always = FALSE);
INT FindAtmelDeviceBootloaderMode(int Order = FIRST_DISCOVERY, BOOL Quiet = FALSE);

BOOL bFWOnly = FALSE;
BOOL bCfgOnly = FALSE;
BOOL bCfgRead = FALSE;
BOOL bPreserveCfg = FALSE;
BOOL bBootloaderMode = FALSE;
// This is to disable and re-enable the I2C device (to force Windows Inbox driver to re-read HID descriptor after FW flashing)
BOOL DisableEnableDevice = FALSE;

// 2016/06/03. Miracle. This flag support HID bootloader.
BOOL bHIDBLFW = FALSE;

// 2016-08-24 MCG - This flag for suppressing print statements.
BOOL bQuiet = FALSE;	
// 2016-08-24 MCG - This flag to indcate flash command sent.
BOOL bFlashCmdSent = FALSE;
//2016-08-24 MCG - This unsigned int holds error values for functions
INT retError = 0;



BOOL VerifyConfig();
BOOL IsWin7 = FALSE;

#ifdef MXT_VERIFIER

BOOL RediscoverDevice();
BOOL CheckIfFWCanBeFlashed();
int FlashFWUsb();
UINT Version;
UINT16 ProductID;
UINT8 ActualThqa[512];
BOOL FWFlashNeeded = FALSE;
BOOL CfgStoringNeeded = FALSE;
BOOL PIDStoringNeeded = FALSE;
BOOL THQAStoringNeeded = FALSE;

enum VERIFIERCODES {
  eSuccess,
  eFWError,
  eCfgError,
  ePIDError,
  eThqaError,
} VerifCodes;
#endif

// Version is defined in Util.h
//#define PRODUCT_NAME_VERSION _T("\n\nAtmel Firmware and Configuration update tool for maXTouch devices,\nconnected via USB or I2C. Version: %s - %s - Special Version: Disable/Enable touch device (Hard reset)\n\n")
//#define PRODUCT_NAME_VERSION _T("\n\nAtmel Firmware and Configuration update tool for maXTouch devices,\nconnected via USB or I2C.\nVersion: Experimental 2.18.01 - DO NOT DISTRIBUTE\n\n")
#ifdef MXT_INFO
#define PRODUCT_NAME_VERSION _T("\n\nAtmel information tool for maXTouch devices,\nconnected via USB or I2C. Version: %s - %s\n\n")
#else
#define PRODUCT_NAME_VERSION _T("\n\nAtmel Firmware and Configuration update tool for maXTouch devices,\nconnected via USB or I2C. Version: %s - %s\n\n")
#endif
//#define PRODUCT_NAME_VERSION _T("\n\nAtmel Firmware and Configuration update tool for maXTouch devices,\nconnected via USB or I2C. Version: %s - %s - Test Version\n\n--- NOT FOR DISTRIBUTION ---\n\n")
//#define PRODUCT_NAME_VERSION _T("\n\nAtmel Firmware and Configuration update tool for maXTouch devices,\nconnected via USB or I2C. Version: %s - %s - Test Version\n\n")

void CloseLog(int iRetCode);
void CloseLogSvc(int iRetCode);

#ifdef AUTO_UPDATER
#ifdef MXT_VERIFIER
#define EXIT(x) exit(x)
#else
#define EXIT(x) exit(ExitPrintMsg(x))
#endif
#else
#ifndef MXTUPDATER_SVC
#define EXIT(x) CloseLog(x); exit(x)
#else
#define EXIT(x) CloseLogSvc(x); CleanSavedDeviceInfo(); return(x)
#endif
#endif

#ifdef AUTO_UPDATER

TCHAR FWVersion[1024];

int ExitPrintMsg(int iRetCode)
{
#ifndef ADD_MFC
  if (iRetCode < 0) {
    _TPRINTF(_T("\nUpdate was not successful.\n"));
  } else {
    _TPRINTF(_T("\nUpdate completed successfully.\n"));
  }
  if (LogMode)  LogFile.close();
  // no reboot check in case of an error
  if (iRetCode < 0) {
    if (!SilentMode && PauseBeforeExit) {
      _tprintf(_T("Press any key to continue . . .\n"));
      std::getchar();
    }
  } else { // no error
    if (!SilentMode && RebootNeeded) {
      _tprintf(_T("Your system needs to restart/shutdown. Press any key to proceed or Ctrl+c to postpone it.\n"));
      std::getchar();
      // RebootNeeded = 1: restart; RebootNeeded = 2: shutdown
      SystemReboot(RebootNeeded==1);
    } else { //no reboot needed
      if (!SilentMode && PauseBeforeExit) {
        _tprintf(_T("Press any key to continue . . .\n"));
        std::getchar();
      }
    }
  }
#else
  if (iRetCode < 0) {
    AfxMessageBox(_T("Update was not successful. Please try running it again."), MB_OK | MB_ICONEXCLAMATION);
  } else {
//    AfxMessageBox(_T("Update completed successfully. Please restart your system."), MB_OK | MB_ICONINFORMATION);
    AfxMessageBox(_T("Update completed successfully. There is no need to restart your system."), MB_OK | MB_ICONINFORMATION);
  }
#endif
  return(iRetCode);
}
#endif

#ifdef ADD_MFC
// The one and only application object 
CWinApp theApp;
using namespace std;
#endif

#ifdef MXTUPDATER_SVC
int MainEntry(int argc, _TCHAR *argv[], TCHAR *LogFileFullName)
#else
int __cdecl _tmain(int argc, _TCHAR* argv[])
#endif
{
  UNREFERENCED_PARAMETER(argc);
  UNREFERENCED_PARAMETER(argv);
  BOOL bReturncode = FALSE;
  int i;
  int iRetCode = -1;  // Set it as error
  OSVERSIONINFO osvi;

#ifdef _DEBUG
  getchar();
#endif

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  GetVersionEx(&osvi);

  //Verify that the system is 6.1 Windows 7 or Win Server2008R2

  if ((osvi.dwMajorVersion == 6) && (osvi.dwMinorVersion == 1)) {
    IsWin7 = TRUE;
  } else {
    IsWin7 = FALSE;
  }

#ifdef MXT_INFO
  argc = 2;
  argv[1] = _T("-i");
#endif

#ifdef ADD_MFC
  HMODULE hModule = ::GetModuleHandle(NULL);
  if (hModule != NULL) {
    // initialize MFC and print and error on failure
    if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0)) {
      // TODO: change error code to suit your needs
      AfxMessageBox(_T("Application internal error."), MB_OK | MB_ICONSTOP);
      EXIT(iRetCode);
    }
  }
#endif

#ifdef AUTO_UPDATER
  if (!WorkOnResources()) {
#ifdef ADD_MFC
    AfxMessageBox(_T("Application internal error."), MB_OK | MB_ICONSTOP);
#else
    _tprintf(_T("\nApplication internal error. Exiting...\n"));
    if (!SilentMode && PauseBeforeExit) {
      _tprintf(_T("Press any key to continue . . .\n"));
      std::getchar();
    }
#endif
    exit(iRetCode);
  }

#ifndef END_USER
  if (argc > 1) {
    if (!(_tcsnicmp(argv[1], _T("/i"), 2)) || !(_tcsnicmp(argv[1], _T("-i"), 2))) {
      ShowVersion();
      exit(0);
    }
  }
#endif
  if (argc > 1) {
    if (!(_tcsnicmp(argv[1], _T("/s"), 2)) || !(_tcsnicmp(argv[1], _T("-s"), 2))) {
      SilentMode = TRUE;
    } else if (!(_tcsnicmp(argv[1], _T("/q"), 2)) || !(_tcsnicmp(argv[1], _T("-q"), 2))) {
      SilentMode = TRUE;
    } else if (!(_tcsnicmp(argv[1], _T("/r"), 2)) || !(_tcsnicmp(argv[1], _T("-r"), 2))) {
      RebootNeeded = 1;
    } else if (!(_tcsnicmp(argv[1], _T("/h"), 2)) || !(_tcsnicmp(argv[1], _T("-h"), 2))) {
      RebootNeeded = 2;
    }
  }

  if(LogMode) {
    LogFile.open(LogFileAuto, fstream::in | fstream::out | fstream::trunc);
    if (LogFile.is_open()) {
      LogMode = TRUE;
    } else {
      _stprintf(PrintBuffer, _T("Cannot create log file %s.\n"), LogFileAuto);
      _TPRINTF(PrintBuffer);
    }
  }
  _TPRINTF(OpenMsg);

  if (IdentifyConnectFWType() < 0) {
#ifdef ADD_MFC
    AfxMessageBox(_T("Could not identify any touch-screen controller. Please power-cycle your system and run it again."), MB_OK | MB_ICONSTOP);
#else
    _TPRINTF(_T("Could not identify any touch-screen controller. Please power-cycle your system and run it again.\n\n"));
    if (!SilentMode && PauseBeforeExit) {
      _tprintf(_T("Press any key to continue . . .\n"));
      std::getchar();
    }
 #endif
    exit(iRetCode);
  }
#endif

#ifndef AUTO_UPDATER
  _stprintf(PrintBuffer, PRODUCT_NAME_VERSION, MXTUPDATER_VERSION, MXTUPDATER_DATE);
  if (MaxTouchAnalyzer || MaxTouchStudio) _TPRINTF(_T("\n"));
  fstream serialnumFileRead;
  fstream serialnumFileWrite;
  fstream thqaFile;
  TCHAR temp[16];
  TCHAR *pch;
  int repeatTimes = 1;
  int grRetries = 1;
  UINT objType = 0;
  UINT8 objSize = 0;
  UINT offset = 0;
  UINT typeMsg = 0;
  UINT timeoutMsg = 0;
  wfstream encFile, cfgFile;
  TCHAR line[LINE_MAX_SIZE];
  SYSTEMTIME lt;
  UINT startTime, endTime, currentTime;
  UINT reportId = 0;
  UINT8 msgBuffer[16];
  int numParam = 0;
  UINT8 refsDeltas = 0;

#ifdef MXTUPDATER_SVC
  if (LogFileFullName) {
    LogFile.open(LogFileFullName, fstream::out | fstream::app);
    if (LogFile.is_open()) {
      LogModeSvc = TRUE;
    }
    else {
      return ERROR_OPENING_LOGFILE;
    }
  }
#endif

  // make sure there is at least one parameter...

  _TPRINTF(PrintBuffer);  // Print MxtUpdater info

  if (argc == 1) {
    ShowUsage();
    EXIT(iRetCode);
  }

  if (!(_tcsnicmp(argv[1], _T("/?"), 2)) || !(_tcsnicmp(argv[1], _T("-?"), 2))) {
    ShowUsage();
    EXIT(0);
  }

  // Get device info...
  LoadXMLFile();

#ifdef OCULAR_TOSHIBA
startover:
#endif

  if ((argc == 2) && (!(_tcsnicmp(argv[1], _T("/reboot"), 7)) || !(_tcsnicmp(argv[1], _T("-reboot"), 7)))) {
    _tprintf(_T("Your system will restart. Press any key to proceed or Ctrl+c to abort.\n"));
    std::getchar();
    SystemReboot();
    Sleep(10000);
    EXIT(0);
  }

  if ((argc == 2) && (!(_tcsnicmp(argv[1], _T("/de"), 3)) || !(_tcsnicmp(argv[1], _T("-de"), 3)))) {
    _tprintf(_T("Attempting to disable and enable I2C HID device.\n"));
    UINT retCode = DisableDevice(_T("ACPI\\ATML1000"), TRUE);
    if (!retCode) { // success
      Sleep(5000);
      if (!DisableDevice(_T("ACPI\\ATML1000"), FALSE)) {
        _stprintf(PrintBuffer, _T("\nTouch Device was successfully disabled and enabled\n"));
        _TPRINTFD(PrintBuffer);
        EXIT(0);
      } else {
        _stprintf(PrintBuffer, _T("\nThere was an error re-enabling Touch Device\n"));
        _TPRINTFD(PrintBuffer);
        EXIT(-1);
      }
    } else {
      switch (retCode) {
      case 1:
        _stprintf(PrintBuffer, _T("\nDevice Enumeration failed: unable to disable/enable device\n"));
        _TPRINTFD(PrintBuffer);
        break;
      case 2:
        _stprintf(PrintBuffer, _T("\nDevice not found: unable to disable/enable device\n"));
        _TPRINTFD(PrintBuffer);
        break;
      case 3:
        _stprintf(PrintBuffer, _T("\nInsufficient rights: you must run in an elevated command prompt (Administrator prompt)\n"));
        _TPRINTFD(PrintBuffer);
        break;
      default:
        _stprintf(PrintBuffer, _T("\nError %d: unable to disable/enable device\n"), retCode);
        _TPRINTFD(PrintBuffer);
      }
      EXIT(-1);
    }
  }

  // Check if file conversion is requested: no need to probe connected device
  //
  if ((argc == 3) && (!(_tcsnicmp(argv[1], _T("/x2raw"), 6)) || !(_tcsnicmp(argv[1], _T("-x2raw"), 6)))) {
    _tcscpy_s(CfgFile, MAX_PATH, argv[2]);
    cfgFile.open(CfgFile, ifstream::in);
    if(!cfgFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    } else {
      if (cfgFile.getline(line, LINE_MAX_SIZE) ) {
        if (_tcsnicmp(line, _T("[COMMENTS]"), 10) && _tcsnicmp(line, _T("[VERSION_INFO_HEADER]"), 21)) {
          _stprintf(PrintBuffer, _T("The config file %s does not appear a valid extended cfg (.xcfg) file.\nMaybe it is a .cfg. Please verify it. Exiting...\n"), CfgFile);
          _TPRINTF(PrintBuffer);
          EXIT(iRetCode);
        }
        cfgFile.close();
      }
    }
    if (!ParseReadConfigXFileOnly(CfgFile)) {
      _stprintf(PrintBuffer, _T("\nError parsing configuration file. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    // get just the name wihout extension
    TCHAR* lastDot = _tcsrchr(CfgFile, '.');
    TCHAR extension[10];
    if (!lastDot) {
      _stprintf(PrintBuffer, _T("\nCannot create output file. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }

    lastDot[0] = ' ';
    //parse and find object name and id
    _stscanf(CfgFile, _T("%s %s"), CfgFile, extension);
    _tcscat(CfgFile, _T(".raw"));
    cfgFile.open(CfgFile, ofstream::out);
    if(!cfgFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open/create: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
      _TPRINTF(PrintBuffer);
      DeleteFile(CfgFile);
      EXIT(iRetCode);
    }
    cfgFile.close();
    if (!WriteRawConfigFile(CfgFile, FALSE)) {
      _stprintf(PrintBuffer, _T("Could not convert .xcfg file: %s to raw format. Exiting...\n"), argv[2]);
      _TPRINTF(PrintBuffer);
      DeleteFile(CfgFile);
      EXIT(iRetCode);
    }
    _stprintf(PrintBuffer, _T("The .xcfg file: %s was successfully converted to raw format.\nThe converted raw file is: %s. Exiting...\n"), argv[2], CfgFile);
    _TPRINTF(PrintBuffer);
    iRetCode = 0;
    EXIT(iRetCode);
  }

  // Check I2C device disable/enable option - it is the last param if present
  if ((_tcslen(argv[argc-1]) == 2) && !(_tcsnicmp(argv[argc-1], _T("/de"), 3)) || !(_tcsnicmp(argv[argc-1], _T("-de"), 3))) {
    DisableEnableDevice = TRUE;
  } else if ((_tcslen(argv[argc-1]) == 2) && !(_tcsnicmp(argv[argc-1], _T("/d"), 2)) || !(_tcsnicmp(argv[argc-1], _T("-d"), 2))) {
    // Check undocumented debug option - it is the last param if present
    DebugMode = TRUE;
  } else if ((_tcslen(argv[argc-1]) == 3) && !(_tcsnicmp(argv[argc-1], _T("/ms"), 3)) || !(_tcsnicmp(argv[argc-1], _T("-ms"), 3))) {
    // Check undocumented MaxTouchStudio option - it is the last param if present
    MaxTouchStudio = TRUE;
  } else if ((_tcslen(argv[argc-1]) == 2) && !(_tcsnicmp(argv[argc-1], _T("/m"), 2)) || !(_tcsnicmp(argv[argc-1], _T("-m"), 2))) {
    // Check undocumented MaxTouchAnalyzer option - it is the last param if present
    MaxTouchAnalyzer = TRUE;
  } else if ((_tcslen(argv[argc-1]) == 3) && !(_tcsnicmp(argv[argc-1], _T("/ec"), 3)) || !(_tcsnicmp(argv[argc-1], _T("-ec"), 3))) {
    // Check undocumented error codes option - it is the last param if present
    ErrorCodes = TRUE;
  }

  // Check undocumented log option - it is the last param if present (cannot be with -d) - Requested by Samsung 8/8/2012
  if ((_tcslen(argv[argc-1]) == 4) && !(_tcsnicmp(argv[argc-1], _T("/log"), 4)) || !(_tcsnicmp(argv[argc-1], _T("-log"), 4))) {
    LogFile.open(LogFileName, fstream::in | fstream::out | fstream::trunc);
    if (LogFile.is_open()) {
      LogMode = TRUE;
      // delete previous ufiles if there...
      DeleteFile(LogFileSuccess);
      DeleteFile(LogFileFail);
    } else {
      _stprintf(PrintBuffer, _T("Cannot create log file %s.\n"), LogFileName);
      _TPRINTF(PrintBuffer);
    }
  }

  if (!(_tcsnicmp(argv[1], _T("/cs="), 4)) || !(_tcsnicmp(argv[1], _T("-cs="), 4))) {
    if (argc ==2) {
      ShowUsage();
      EXIT(iRetCode);
    }
    _tcscpy(temp, &argv[1][4]);
    int numChar;
    numChar = WideCharToMultiByte(CP_UTF8, NULL, (WCHAR *)temp, -1, CustomString, 512, NULL, NULL);
    UseCustomString = TRUE;
    ++numParam;
    DeviceIndex = GetIndexFromCustomString(CustomString);
    if (DeviceIndex == -1) {
      _stprintf(PrintBuffer, _T("There are multiple devices with the same custom string. Exiting.\n\n"));
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    if (!DeviceIndex) {
      _stprintf(PrintBuffer, _T("Invalid custom string or device not found.\n\n"));
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
  }
  if (!(_tcsnicmp(argv[1], _T("/ld="), 4)) || !(_tcsnicmp(argv[1], _T("-ld="), 4))) {
    if (argc ==2) {
      ShowUsage();
      EXIT(iRetCode);
    }
    _tcscpy(temp, &argv[1][4]);
    _stscanf(temp, _T("%d"), &DeviceIndex);
    ++numParam;
  } else if (!(_tcsnicmp(argv[1], _T("/ld"), 3)) || !(_tcsnicmp(argv[1], _T("-ld"), 3))) {
    DisplayMaxTouchControlDevices();
    EXIT(0);
  }

  if (!(_tcsnicmp(argv[1+numParam], _T("/l"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-l"), 2))) {
    DisplayEnumeratedDevices();
    EXIT(0);
  }

  if (!(_tcsnicmp(argv[1], _T("/usb"), 4)) || !(_tcsnicmp(argv[1], _T("-usb"), 4))) {
    if (argc == 2) {
      ShowUsage();
      EXIT(iRetCode);
    }
    UsbI2c = 1;
    ++numParam;
  } else if (!(_tcsnicmp(argv[1], _T("/i2c"), 4)) || !(_tcsnicmp(argv[1], _T("-i2c"), 4))) {
    if (argc == 2) {
      ShowUsage();
      EXIT(iRetCode);
    }
    UsbI2c = 2;
    ++numParam;
  }
  // Check if we have a mxtupdater.ini file
  if (GetPrivateProfileString(_T("DeviceInfo"), _T("DevicePath"), NULL, Custom_DevicePath, 64, _T(".\\MxtUpdater.ini"))) {
    IsCustomDevicePath = TRUE;
  }

  if (IdentifyConnectFWType() < 0) {
    _stprintf(PrintBuffer, _T("Could not identify any supported maXTouch/maXFusion device.\n\n"));
    _TPRINTF(PrintBuffer);
    if (ErrorCodes) iRetCode = NO_DEVICE_FOUND;
    EXIT (iRetCode);
  } else {
	  if (DeviceDiscovered == I2C_NORMAL && ConnectFWType == HIDI2C) {
	  }
	  else if (DeviceDiscovered != USB_BOOTLOADER && DeviceDiscovered != I2C_BOOTLOADER && !ValidateDevice()) {
      _stprintf(PrintBuffer, _T("The parameters read from the device found do not appear to be valid.\nMaybe there is a communication problem (wrong drivers?). Exiting...\n\n"));
      _TPRINTF(PrintBuffer);
      EXIT (iRetCode);
    }
  }

  // Check if special functions
  UINT16 address = 0, numBytes = 0;
  // use this with _stscanf: otherwise possible corruption on x64 version
  UINT addressInt = 0, numBytesInt = 0;
  DWORD bytesToWrite = 0;
  UINT8 tempBuffer[64];
  if (argc == 3+numParam) {
    // Check if something like: –regr=T66[0] nn
    if (!(_tcsnicmp(argv[1+numParam], _T("/regr=T"), 7)) || !(_tcsnicmp(argv[1+numParam], _T("-regr=T"), 7))) {
      _tcscpy(temp, &argv[1+numParam][7]);
      for (i = 0; i < (int)_tcslen(temp); ++i) {
        if ((temp[i] == _T('[')) || (temp[i] == _T(']'))) temp[i] = _T(' ');
      }
      _stscanf(temp, _T("%d %x"), &objType, &offset);
      _stscanf(argv[2+numParam], _T("%x"), &numBytesInt);
      numBytes = (UINT16)numBytesInt;
      if (!GetObjectAddressAndSize((UINT8)objType, &address, &objSize)) {
        _stprintf(PrintBuffer, _T("Invalid object provided: T%d. Exiting...\n"), objType);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      if ((offset + numBytes) > objSize) {
        _stprintf(PrintBuffer, _T("Bytes to be read (%d) at offset %d exceed object size (%d). Exiting...\n"), numBytes, offset, objSize);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      if(FindAtmelDevice()) {
        bReturncode = RegReadFromAddress(address+(UINT16)offset, numBytes);
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Read failed. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }
  if (argc == 4+numParam) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/regr"), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-regr"), 5))) { 
      _stscanf(argv[2+numParam], _T("%x"), &addressInt);
      _stscanf(argv[3+numParam], _T("%x"), &numBytesInt);
      numBytes = (UINT16)numBytesInt;
      address = (UINT16) addressInt;
      if (numBytes > 4096) {
        _stprintf(PrintBuffer, _T("The maximum amount of byes that can be read is 4096 (0x1000). Exiting...\n"));
        _TPRINTF(PrintBuffer);
        EXIT (iRetCode);
      }
      if(FindAtmelDevice()) {
        bReturncode = RegReadFromAddress(address, numBytes);
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Read failed. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }
  if (argc > 2+numParam) {
    // Check if something like: –regw=T66[0] 0 60 40
    if (!(_tcsnicmp(argv[1+numParam], _T("/regw=T"), 7)) || !(_tcsnicmp(argv[1+numParam], _T("-regw=T"), 7))) {
      _tcscpy(temp, &argv[1+numParam][7]);
      for (i = 0; i < (int)_tcslen(temp); ++i) {
        if ((temp[i] == _T('[')) || (temp[i] == _T(']'))) temp[i] = _T(' ');
      }
      _stscanf(temp, _T("%d %x"), &objType, &offset);
      bytesToWrite = (argc-2-numParam);
      if (!GetObjectAddressAndSize((UINT8)objType, &address, &objSize)) {
        _stprintf(PrintBuffer, _T("Invalid object provided: T%d. Exiting...\n"), objType);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      if ((offset + bytesToWrite) > objSize) {
        _stprintf(PrintBuffer, _T("Bytes to be written (%d) at offset %d exceed object size (%d). Exiting...\n"), bytesToWrite, offset, objSize);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      for (i = 2+numParam; i < argc; ++i) {
        _stscanf(argv[i], _T("%hhx"), &tempBuffer[i-2-numParam]);
      }
      if(FindAtmelDevice()) {
        bReturncode = WriteToDevice(address+(UINT16)offset, tempBuffer, &bytesToWrite);
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Write failed. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("WOK\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }
  if (argc > 3+numParam) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/regw"), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-regw"), 5))) {
      _stscanf(argv[2+numParam], _T("%x"), &addressInt);
      address = (UINT16)addressInt;
      for (i = 3+numParam; i < argc; ++i) {
        _stscanf(argv[i], _T("%hhx"), &tempBuffer[i-3-numParam]);
      }
      if(FindAtmelDevice()) {
        bytesToWrite = (argc-3-numParam);
        bReturncode = WriteToDevice(address, tempBuffer, &bytesToWrite);
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Write failed. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("WOK\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }
  
  if (argc > 1+numParam) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/mt"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-mt"), 3))) {
      if (!(_tcsnicmp(argv[1+numParam], _T("/mt*"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-mt*"), 4))) {
        typeMsg = 0xFF;  // we use 0xFF for all objects
      }
      _tcscpy_s(temp, argv[1+numParam]);
      pch = _tcstok(temp, _T("="));
      if (typeMsg != 0xFF) {
        _stscanf(&temp[3], _T("%d"), &typeMsg);
      }
      pch = _tcstok(NULL, _T("="));
      if (pch) {
        _stscanf(pch, _T("%d"), &timeoutMsg);
      }
      if(FindAtmelDevice()) {
        if (argc > 3+numParam) {
          // Check if something like: –regw=T66[0] 0 60 40
          if (!(_tcsnicmp(argv[2+numParam], _T("/regw=T"), 7)) || !(_tcsnicmp(argv[2+numParam], _T("-regw=T"), 7))) {
            _tcscpy(temp, &argv[2+numParam][7]);
            for (i = 0; i < (int)_tcslen(temp); ++i) {
              if ((temp[i] == _T('[')) || (temp[i] == _T(']'))) temp[i] = _T(' ');
            }
            _stscanf(temp, _T("%d %x"), &objType, &offset);
            bytesToWrite = (argc-3-numParam);
            if (!GetObjectAddressAndSize((UINT8)objType, &address, &objSize)) {
              _stprintf(PrintBuffer, _T("Invalid object provided: T%d. Exiting...\n"), objType);
              _TPRINTF(PrintBuffer);
              EXIT(iRetCode);
            }
            if ((offset + bytesToWrite) > objSize) {
              _stprintf(PrintBuffer, _T("Bytes to be written (%d) at offset %d exceed object size (%d). Exiting...\n"), bytesToWrite, offset, objSize);
              _TPRINTF(PrintBuffer);
              EXIT(iRetCode);
            }
            for (i = 3+numParam; i < argc; ++i) {
              _stscanf(argv[i], _T("%hhx"), &tempBuffer[i-3-numParam]);
            }
            bReturncode = WriteToDevice(address+(UINT16)offset, tempBuffer, &bytesToWrite);
            if (!bReturncode) {
              _stprintf(PrintBuffer, _T("Write failed. Exiting...\n"));
              _TPRINTF(PrintBuffer);
              EXIT(iRetCode);
            } else {
              _stprintf(PrintBuffer, _T("WOK\n"));
              _TPRINTF(PrintBuffer);
            }
          }
        }
      }
      GetLocalTime(&lt);
      startTime = lt.wMinute*60+lt.wSecond;
      endTime = startTime + timeoutMsg;
      // check if all msg are requested
//      ShowMessage(typeMsg, timeoutMsg);
      if (typeMsg != 0xFF) {
        reportId = GetReportIdFromType((UINT8)typeMsg);
      }
      if (MaxTouchAnalyzer) MessageMode();
      for (;;) {
        ReadMessage(msgBuffer);
        if (typeMsg == 0xFF || msgBuffer[0] == reportId) {
          if (msgBuffer[0] != 0xFF) {
            UINT j= 0;
            _stprintf(&PrintBuffer[j], _T("T%.3d "), ReportIds[msgBuffer[0]]);
            j += 5;
            for (i = 0; i < MsgProcSize-1; ++i) {
              _stprintf(&PrintBuffer[j], _T("%.2X "), msgBuffer[i]);
              j += 3;
            }
            _stprintf(&PrintBuffer[j], _T("%c"), '\n');
            _TPRINTF(PrintBuffer);
          }
        }
        GetLocalTime(&lt);
        currentTime =  lt.wMinute*60+lt.wSecond;
        if (currentTime > endTime) break;
      }
      iRetCode = 0;
      EXIT(iRetCode);
    }
  }
  if (MaxTouchAnalyzer) {
    if ((argc == 2 + numParam) || (argc == (3 + numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
      if (!(_tcsnicmp(argv[1 + numParam], _T("/activearea"), 6)) || !(_tcsnicmp(argv[1 + numParam], _T("-activearea"), 6))) {
        if (FindAtmelDevice()) {
          ShowActiveArea();
          EXIT(0);
        }
      }
    }
  }
  if ((argc == 2+numParam) || (argc == (3+numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/reset"), 6)) || !(_tcsnicmp(argv[1+numParam], _T("-reset"), 6))) {
      if(FindAtmelDevice()) {
        ZeroMemory(PrintBuffer, 512);
        if (XMLFilePresent) {
          _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
        }
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
        } else {
          _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTF(PrintBuffer);
        bReturncode = ResetDevice();
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Cannot reset maXTouch device. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("maXTouch device was reset successfully. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }

  if ((argc == 2+numParam) || (argc == (3+numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/calib"), 6)) || !(_tcsnicmp(argv[1+numParam], _T("-calib"), 6))) {
      if(FindAtmelDevice()) {
        ZeroMemory(PrintBuffer, 512);
        if (XMLFilePresent) {
          _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
        }
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
        } else {
          _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTF(PrintBuffer);
        bReturncode = CalibrateDevice();
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Cannot calibrate maXTouch device. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("maXTouch device was calibrated successfully. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }
  UINT8 backupCode = 0x55;
  if ((argc == 2+numParam) || (argc == (3+numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/backup"), 7)) || !(_tcsnicmp(argv[1+numParam], _T("-backup"), 7))) {
      _tcscpy_s(temp, argv[1+numParam]);
      pch = _tcstok(temp, _T("="));
      if ((_tcsicmp(pch, _T("/backup"))) && (_tcsicmp(pch, _T("-backup")))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      pch = _tcstok(NULL, _T("="));
      if (pch) {
        UINT tempCode;
        _stscanf(pch, _T("%x"), &tempCode);
        backupCode = (UINT8)tempCode;
      }
      if ((backupCode != 0x33) && (backupCode != 0x44) && (backupCode != 0x55)) {
        _stprintf(PrintBuffer, _T("\nInvalid parameter <%s>; allowed values are: 0x33, 0x44 and 0x55.\n"), argv[1+numParam]);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }

      if(FindAtmelDevice()) {
        ZeroMemory(PrintBuffer, 512);
        if (XMLFilePresent) {
          _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
        }
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
        } else {
          _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTF(PrintBuffer);
        bReturncode = BackupDevice(backupCode);
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Cannot backup maXTouch device. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("Backup command was executed successfully. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }
  if ((argc == 2+numParam) || (argc == (3+numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/uid"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-uid"), 4))) {
      iRetCode = ReadDeviceUniqueID();
      if (iRetCode == -1) {
        _stprintf(PrintBuffer, _T("Failed to get device unique ID.\n"));
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      if (iRetCode == 1) {
        _stprintf(PrintBuffer, _T("Unique ID reading not supported for this device.\n"));
        _TPRINTF(PrintBuffer);
        iRetCode = -1;
        EXIT(iRetCode);
      }
      iRetCode = 0;
      EXIT(iRetCode);
    }
  }
  if ((argc == 2+numParam) || (argc == (3+numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
    if (!(_tcsnicmp(argv[1+numParam], _T("/gr"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-gr"), 3))) {
      if (!(_tcsnicmp(argv[1+numParam], _T("/gr="), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-gr="), 4))) {
        _tcscpy(temp, &argv[1+numParam][4]);
        _stscanf(temp, _T("%d"), &grRetries);
      }
      if(FindAtmelDevice()) {
        ZeroMemory(PrintBuffer, 512);
        if (XMLFilePresent) {
          _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
        }
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
        } else {
          _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTF(PrintBuffer);
        for (i = 0; i < grRetries; ++i) {
          bReturncode = StoreGoldenRefs();
          if (!bReturncode) {
            _stprintf(PrintBuffer, _T("Failed attempt %d of %d.\n"), i+1, grRetries);
            _TPRINTF(PrintBuffer);
            Sleep(2000);
          } else {
            break;
          }
        }
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("\nFailed to save Golden References. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("\nGolden References have been stored successfully. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
        }
        EXIT(iRetCode);
      }
    }
  }

  // Check parameters number...
  if ((argc == 2+numParam) || (argc == (3+numParam) && (DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice))) {
    BOOL inMemory = FALSE;
    if (!(_tcsnicmp(argv[1+numParam], _T("/cm"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-cm"), 3))) {
      inMemory = TRUE;
    }
    if (!(_tcsnicmp(argv[1+numParam], _T("/c"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-c"), 2))) {
      if(FindAtmelDevice()) {
        ZeroMemory(PrintBuffer, 512);
        if (XMLFilePresent) {
          _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
        }
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
        } else {
          _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTF(PrintBuffer);
        bReturncode = DisplayChecksum(inMemory);
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Cannot access maXTouch device information. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          iRetCode = 0;
        }
      } else {
        if (FindAtmelDeviceBootloaderMode()) {
          _stprintf(PrintBuffer, _T("Found a maXTouch device in bootloader mode. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          if (ErrorCodes) iRetCode = FOUND_DEVICE_BOOTLOADER;
        } else {
          _stprintf(PrintBuffer, _T("Cannot access any supported maXTouch devices. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          if (ErrorCodes) iRetCode = NO_DEVICE_FOUND;
        }
      }
      EXIT(iRetCode);
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/wcc"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-wcc"), 4))) {
      _stprintf(PrintBuffer, _T("Clearing configuration: are you sure to proceed? (y/N)\n"));
      _TPRINTF(PrintBuffer);
      char a = (char)getchar();
      if ((a == 'y' || a == 'Y')) {
        if (!ZeroConfigMemory()) {
          _stprintf(PrintBuffer, _T("Clearing configuration failed. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          EXIT(iRetCode);
        }
        _stprintf(PrintBuffer, _T("Configuration cleared successfully. If required, execute '-backup'. Exiting...\n"));
        _TPRINTF(PrintBuffer);
        iRetCode = 0;
      }
      EXIT(iRetCode);
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/GPIO"), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-GPIO"), 5))) {
      UINT8 GPIOpins = 0;
      ReadGPIOPins(&GPIOpins);
      _tprintf(_T("GPIO pins status is: 0x%x\n"), GPIOpins);
      EXIT(0);
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/i"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-i"), 2))) {
      if(FindAtmelDevice()) {
        ZeroMemory(PrintBuffer, 512);
        if (XMLFilePresent) {
          _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
        }
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
        } else {
          _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTF(PrintBuffer);
        bReturncode = PrintChipInfo();
        if (!bReturncode) {
          _stprintf(PrintBuffer, _T("Cannot access maXTouch device information. Exiting...\n"));
          _TPRINTF(PrintBuffer);
        } else {
          iRetCode = 0;
        }
      } else {
        if (FindAtmelDeviceBootloaderMode()) {
          _stprintf(PrintBuffer, _T("Found a maXTouch device in bootloader mode. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          if (ErrorCodes) iRetCode = FOUND_DEVICE_BOOTLOADER;
        } else {
          _stprintf(PrintBuffer, _T("Cannot access any supported maXTouch devices. Exiting...\n"));
          _TPRINTF(PrintBuffer);
          if (ErrorCodes) iRetCode = NO_DEVICE_FOUND;
        }
      }
      EXIT(iRetCode);
    }
  }

  if (!(_tcsnicmp(argv[1+numParam], _T("/thqa"), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-thqa"), 5))) {
    if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
      ShowUsage();
      EXIT(iRetCode);
    }
    if (!(_tcsnicmp(argv[2+numParam], _T("ON"), 2))) {
      _stprintf(PrintBuffer, _T("\nAttempting to turn ON PID and THQA blob downloaded...\n\n"));
      _TPRINTF(PrintBuffer);
      if (!ActivatePidThqa(TRUE)) {
        _stprintf(PrintBuffer, _T("\nFailed to turn ON reporting of PID and THQA blob downloaded.\n\n"));
        _TPRINTF(PrintBuffer);
      } else {
        _stprintf(PrintBuffer, _T("\nSuccess! maXTouch device will now report PID and THQA blob downloaded.\n\n"));
        _TPRINTF(PrintBuffer);
        iRetCode = 0;
      }
      EXIT(iRetCode);
    } else if (!(_tcsnicmp(argv[2+numParam], _T("OFF"), 3))) {
      _stprintf(PrintBuffer, _T("\nAttempting to turn OFF reporting of PID and THQA blob downloaded...\n\n"));
      _TPRINTF(PrintBuffer);
      if (!ActivatePidThqa(FALSE)) {
        _stprintf(PrintBuffer, _T("\nFailed to turn OFF reporting of PID and THQA blob downloaded.\n\n"));
        _TPRINTF(PrintBuffer);
      } else {
        _stprintf(PrintBuffer, _T("\nSuccess! maXTouch device will now report default PID and THQA blob.\n\n"));
        _TPRINTF(PrintBuffer);
        iRetCode = 0;
      }
      EXIT(iRetCode);
    }
    ShowUsage();
    EXIT(iRetCode);
  }

  // Check the validity of the parameters (i.e. files)
  if (!(_tcsnicmp(argv[1+numParam], _T("/refs"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-refs"), 4))) {
    if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
      ShowUsage();
      EXIT(iRetCode);
    }
    refsDeltas = T37_REFS_MODE;
    if (!(_tcsnicmp(argv[1+numParam], _T("/refsc"), 6)) || !(_tcsnicmp(argv[1+numParam], _T("-refsc"), 6))) {
      refsDeltas = T37_SC_REFS_MODE;
    }
    _tcscpy_s(temp, argv[1+numParam]);
    pch = _tcstok(temp, _T("="));
    pch = _tcstok(NULL, _T("="));
    if (pch) {
      _stscanf(pch, _T("%i"), &repeatTimes);
    }
    if (repeatTimes <= 0) {
      _stprintf(PrintBuffer, _T("\nInvalid parameter <%s>: please provide a positive integer.\n"), argv[1+numParam]);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    _tcscpy_s(ThqaFile, MAX_PATH, argv[2+numParam]);
    _stprintf(PrintBuffer, _T("\nAttempting to save References values to file: %s.\n"), ThqaFile);
    _TPRINTF(PrintBuffer);
    thqaFile.open(ThqaFile, ifstream::out | ifstream::trunc);
    if(!thqaFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open/create: %s. \nCheck if it is a valid file/path. Exiting...\n"), ThqaFile);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    for (i = 0; i < repeatTimes; ++i) {
      if (ReadDeltasRefs(&thqaFile, refsDeltas)) {
        if (repeatTimes > 1) {
          _stprintf(PrintBuffer, _T("Saving snapshot: %i\n"), i+1);
          _TPRINTF(PrintBuffer);
        }
        iRetCode = 0;
      } else {
        break;
      }
    }
    thqaFile.close();
    if (iRetCode) {
      _stprintf(PrintBuffer, _T("Error saving References values. Exiting...\n"));
      _TPRINTF(PrintBuffer);
    } else {
      if (repeatTimes != 1) {
        _stprintf(PrintBuffer, _T("References values were successfully written to file: %s. Saved %i snapshots. Exiting...\n"), ThqaFile, repeatTimes);
        _TPRINTF(PrintBuffer);
      } else {
        _stprintf(PrintBuffer, _T("References values were successfully written to file: %s. Exiting...\n"), ThqaFile);
        _TPRINTF(PrintBuffer);
      }
    }
    EXIT(iRetCode);
  }

  if (!(_tcsnicmp(argv[1+numParam], _T("/deltas"), 6)) || !(_tcsnicmp(argv[1+numParam], _T("-deltas"), 6))) {
    if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
      ShowUsage();
      EXIT(iRetCode);
    }
    refsDeltas = T37_DELTAS_MODE;
    if (!(_tcsnicmp(argv[1+numParam], _T("/deltasc"), 8)) || !(_tcsnicmp(argv[1+numParam], _T("-deltasc"), 8))) {
      refsDeltas = T37_SC_DELTAS_MODE;
    }
    _tcscpy_s(temp, argv[1+numParam]);
    pch = _tcstok(temp, _T("="));
    pch = _tcstok(NULL, _T("="));
    if (pch) {
      _stscanf(pch, _T("%i"), &repeatTimes);
    }
    if (repeatTimes <= 0) {
      _stprintf(PrintBuffer, _T("\nInvalid parameter <%s>: please provide a positive integer.\n"), argv[1+numParam]);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    _tcscpy_s(ThqaFile, MAX_PATH, argv[2+numParam]);
    _stprintf(PrintBuffer, _T("\nAttempting to save Deltas values to file: %s.\n"), ThqaFile);
    _TPRINTF(PrintBuffer);
    thqaFile.open(ThqaFile, ifstream::out | ifstream::trunc);
    if(!thqaFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open/create: %s. \nCheck if it is a valid file/path. Exiting...\n"), ThqaFile);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    for (i = 0; i < repeatTimes; ++i) {
      if (ReadDeltasRefs(&thqaFile, refsDeltas)) {
        if (repeatTimes > 1) {
          _stprintf(PrintBuffer, _T("Saving snapshot: %i\n"), i+1);
          _TPRINTF(PrintBuffer);
        }
        iRetCode = 0;
      } else {
        break;
      }
    }
    if (iRetCode) {
      _stprintf(PrintBuffer, _T("Error saving Deltas values. Exiting...\n"));
      _TPRINTF(PrintBuffer);
    } else {
      if (repeatTimes != 1) {
        _stprintf(PrintBuffer, _T("Deltas values were successfully written to file: %s. Saved %i snapshots. Exiting...\n"), ThqaFile, repeatTimes);
        _TPRINTF(PrintBuffer);
      } else {
        _stprintf(PrintBuffer, _T("Deltas values were successfully written to file: %s. Exiting...\n"), ThqaFile);
        _TPRINTF(PrintBuffer);
      }
    }
    thqaFile.close();
    EXIT(iRetCode);
  }
  
  // Argument to write serial number
  if (!(_tcsnicmp(argv[1 + numParam], _T("/sw"), 3)) || !(_tcsnicmp(argv[1 + numParam], _T("-sw"), 3))) {
	  if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3 + numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4 + numParam)))) {
		  ShowUsage();
		  EXIT(iRetCode);
	  }
	  _stprintf(PrintBuffer, _T("\nAttempting to write Serial Number %s\n"), argv[2 + numParam]);
	  _TPRINTF(PrintBuffer);

	  // Copy serial number to file
	  _tcscpy_s(SerialNumFile, MAX_PATH, argv[2 + numParam]);	

	  // Convert the agrument from TCHAR to STRING
	  char      *pMBBuffer = (char *)malloc(BUFFER_SIZE);
	  wchar_t*pWCBuffer = SerialNumFile;

	  //printf("Convert wide-character string:\n");		//Debug only code

	  // Do Conversion
	  wcstombs_s(&iserial, pMBBuffer, (size_t)BUFFER_SIZE,
	  pWCBuffer, (size_t)BUFFER_SIZE);

	  // Output
	//  printf("   Characters converted: %u\n", i);		//Debug only code
	//  printf("    Multibyte character: %s\n\n",
	//	  pMBBuffer);
	  
	  //Save serial number to text file.  Delete file at end unless needed by customer
	  serialnumFileWrite.open("SerialNum.txt", ofstream::out | ofstream::binary);
	  serialnumFileWrite << pMBBuffer;

	  serialnumFileWrite.close();

	  // Free multibyte character buffer
	  if (pMBBuffer)
	  {
		  free(pMBBuffer);
	  }

	  //Now open the file to read data out
	  serialnumFileRead.open("SerialNum.txt", ifstream::in | ifstream::binary);
 
	  if (!serialnumFileRead.is_open()) {	//Error could mean possible file creation problem.
		  _stprintf(PrintBuffer, _T("Failed preparing serial number %s for write...\n"), SerialNumFile);
		  _TPRINTF(PrintBuffer); 
		  EXIT(iRetCode);
	  }
	
	  //Before writing, set T68 buffer to zero
	  _stprintf(PrintBuffer, _T("\nResetting serial number.\n"));
	  _TPRINTF(PrintBuffer);

	  if (WriteSerialNum(&serialnumFileRead, TRUE)) {
		  _stprintf(PrintBuffer, _T("Done resetting serial number.\n"));
		  _TPRINTF(PrintBuffer);
		  iRetCode = 0;
	  }
	  else {
		  _stprintf(PrintBuffer, _T("\nError resetting the serial number\n"));
		  _TPRINTF(PrintBuffer);
		  iRetCode = 1;
		  EXIT(iRetCode);
	  }

	  if (WriteSerialNum(&serialnumFileRead, FALSE)) {
		  _stprintf(PrintBuffer, _T("\nSerial number was successfully written to device. Exiting...\n"));
		  _TPRINTF(PrintBuffer);
		  iRetCode = 0;
	  }
	  else {
		  _stprintf(PrintBuffer, _T("\nError storing serial number. Exiting...\n"));
		  _TPRINTF(PrintBuffer);
	  }

 	  serialnumFileRead.close();
	  remove("SerialNum.txt");

	  EXIT(iRetCode);
  }
  //Read serial number
  else if (!(_tcsnicmp(argv[1 + numParam], _T("/sr"), 3)) || !(_tcsnicmp(argv[1 + numParam], _T("-sr"), 3))) {
	  if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3 + numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4 + numParam)))) {
		  ShowUsage();
		  EXIT(iRetCode);
	  }

	  _tcscpy_s(SerialNumFile, MAX_PATH, argv[2 + numParam]);	//use the same buffer since it is else if
	  _stprintf(PrintBuffer, _T("\nAttempting to read serial number from maXTouch device...\n\n"));
	  _TPRINTF(PrintBuffer);
	  serialnumFileRead.open(SerialNumFile, ifstream::out | ifstream::binary | ifstream::trunc);		//Use the same Read file

	  if (!serialnumFileRead.is_open()) {
		  _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), SerialNumFile);
		  _TPRINTF(PrintBuffer);
		  EXIT(iRetCode);
	  }
	  if (ReadSerialNumString(&serialnumFileRead)) {
		  _stprintf(PrintBuffer, _T("Serial Number was successfully saved to file: %s.\n"), SerialNumFile);
		  _TPRINTF(PrintBuffer);
		  iRetCode = 0;
	  }
	  else {
		  _stprintf(PrintBuffer, _T("Error reading Serial number from maXTouch device. Exiting...\n"));
		  _TPRINTF(PrintBuffer);
	  }

	  serialnumFileRead.close();

	  _stprintf(PrintBuffer, _T("Serial Number: "));
	  _TPRINTF(PrintBuffer);

	  serialnumFileRead.open(SerialNumFile);

	  string readserialnum;

	  serialnumFileRead >> readserialnum;

	  cout << readserialnum << "\n" << "\n" << "\n";

	  _stprintf(PrintBuffer, _T("Exiting . . .\n"));
	  _TPRINTF(PrintBuffer);

	  serialnumFileRead.close();

	  EXIT(iRetCode);
  }
  // Check the validity of the parameters (i.e. files)
  if (!(_tcsnicmp(argv[1+numParam], _T("/t68"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-t68"), 4))) {
    if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
      ShowUsage();
      EXIT(iRetCode);
    }
    _stprintf(PrintBuffer, _T("\nAttempting to write T68 object...\n\n"));
    _TPRINTF(PrintBuffer);
    _tcscpy_s(ThqaFile, MAX_PATH, argv[2+numParam]);
    thqaFile.open(ThqaFile, ifstream::in | ifstream::binary);
    if(!thqaFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), ThqaFile);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    if (WriteT68(&thqaFile)) {
      _stprintf(PrintBuffer, _T("\nFile was successfully written to device. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      iRetCode = 0;
    } else {
      _stprintf(PrintBuffer, _T("\nError storing T68 file. Exiting...\n"));
      _TPRINTF(PrintBuffer);
    }
    thqaFile.close();
    EXIT(iRetCode);
  }

  if (!(_tcsnicmp(argv[1+numParam], _T("/t0"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-t0"), 3))) {
    if (WriteThqaString(NULL, TRUE)) {
      _stprintf(PrintBuffer, _T("PID and THQA blob were successfully reset. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      iRetCode = 0;
      EXIT(iRetCode);
    }
  }
  if (!(_tcsnicmp(argv[1+numParam], _T("/pid="), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-pid="), 5))) {
    _tcscpy(temp, &argv[1+numParam][5]);
    _stscanf(temp, _T("%x"), &PidToWrite);
    // Fixes problem if user forgets -thqa=.... (Dan Wax did!)
    if (argc < 3+numParam) {
      ShowUsage();
      EXIT(iRetCode);
    }
    if (!(_tcsnicmp(argv[2+numParam], _T("/thqa="), 6)) || !(_tcsnicmp(argv[2+numParam], _T("-thqa="), 6))) {
      _tcscpy_s(ThqaFile, MAX_PATH, &argv[2+numParam][6]);
      thqaFile.open(ThqaFile, ifstream::in | ifstream::binary);
      if(!thqaFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), ThqaFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      if (WriteThqaString(&thqaFile, PidToWrite)) {
        _stprintf(PrintBuffer, _T("PID and THQA blob were successfully written to maXTouch device. Exiting...\n"));
        _TPRINTF(PrintBuffer);
        iRetCode = 0;
      } else {
        _stprintf(PrintBuffer, _T("Error saving PID and THQA blob. Exiting...\n"));
        _TPRINTF(PrintBuffer);
      }
      thqaFile.close();
      EXIT(iRetCode);
    } else {
      ShowUsage();
      EXIT(iRetCode);
    }
  }
  if (!(_tcsnicmp(argv[1+numParam], _T("/tw"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-tw"), 3))) {
    if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
      ShowUsage();
      EXIT(iRetCode);
    }
    if (ConnectFWType == I2CBRIDGE) {
      _stprintf(PrintBuffer, _T("\nThis operation sometimes fails when through %s.\n"), IDENTIFY_CONNECTION_MSG[4]);
      _TPRINTF(PrintBuffer);
 //     EXIT(iRetCode);
    }
    _stprintf(PrintBuffer, _T("\nAttempting to write PID and THQA blob to maXTouch device...\n\n"));
    _TPRINTF(PrintBuffer);
    _tcscpy_s(ThqaFile, MAX_PATH, argv[2+numParam]);
    thqaFile.open(ThqaFile, ifstream::in | ifstream::binary);
    if(!thqaFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), ThqaFile);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    if (WriteThqaString(&thqaFile)) {
      _stprintf(PrintBuffer, _T("PID and THQA blob were successfully written to maXTouch device. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      iRetCode = 0;
    } else {
      _stprintf(PrintBuffer, _T("Error saving PID and THQA blob. Exiting...\n"));
      _TPRINTF(PrintBuffer);
    }
    thqaFile.close();
    EXIT(iRetCode);
  } else if (!(_tcsnicmp(argv[1+numParam], _T("/tr"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-tr"), 3))) {
    if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
      ShowUsage();
      EXIT(iRetCode);
    }
    _tcscpy_s(ThqaFile, MAX_PATH, argv[2+numParam]);
    _stprintf(PrintBuffer, _T("\nAttempting to read THQA blob from maXTouch device...\n\n"));
    _TPRINTF(PrintBuffer);
    thqaFile.open(ThqaFile, ifstream::out | ifstream::binary | ifstream::trunc);
    if(!thqaFile.is_open()) {
      _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), ThqaFile);
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    if (ReadThqaString(&thqaFile)) {
      _stprintf(PrintBuffer, _T("THQA blob was successfully saved to file: %s. Exiting...\n"), ThqaFile);
      _TPRINTF(PrintBuffer);
      iRetCode = 0;
    } else {
      _stprintf(PrintBuffer, _T("Error reading THQA blob from maXTouch device. Exiting...\n"));
      _TPRINTF(PrintBuffer);
    }
    thqaFile.close();
    EXIT(iRetCode);
  }
  if (!(_tcsnicmp(argv[1+numParam], _T("/pid"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-pid"), 4))) {
    DisplayMaXTouchPIDs();
    iRetCode = 0;
    EXIT(iRetCode);
  }

  // Check the validity of the parameters (i.e. files)
  if (!(_tcsnicmp(argv[1+numParam], _T("/b"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-b"), 2))) {
    CheckBootloaderOnly = TRUE;
    bFWOnly = TRUE;
    if (!(_tcsnicmp(argv[1+numParam], _T("/bl"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-bl"), 3))) {
      LeaveInBootloaderMode = TRUE;
    }
  } else {
    if (!(_tcsnicmp(argv[1+numParam], _T("/fd"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-fd"), 3))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      if (!(_tcsnicmp(argv[1+numParam], _T("/fd="), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-fd="), 4))) {
        _tcscpy(temp, &argv[1+numParam][4]);
        _stscanf(temp, _T("%d"), &Delay);
      } else {
        Delay = 20;
      }
      bFWOnly = TRUE;
      USBReadDelay = TRUE;
      _tcscpy_s(EncFile, MAX_PATH, argv[2+numParam]);
      encFile.open(EncFile, ifstream::in);
      if(!encFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      encFile.close();
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/fid"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-fid"), 4))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      MaxBuffSize = 32;
      if (!(_tcsnicmp(argv[1+numParam], _T("/fid="), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-fid="), 5))) {
        _tcscpy(temp, &argv[1+numParam][5]);
        _stscanf(temp, _T("%d"), &MaxBuffSize);
      }
      bFWOnly = TRUE;
      SmallBufferIntel = TRUE;
      Delay = 100;
      USBReadDelay = TRUE;
      _stprintf(PrintBuffer, _T("Using small buffer - Intel\n"));
      _TPRINTF(PrintBuffer);
      _tcscpy_s(EncFile, MAX_PATH, argv[2+numParam]);
      encFile.open(EncFile, ifstream::in);
      if(!encFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      encFile.close();
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/fi"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-fi"), 3))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      MaxBuffSize = 32;
      if (!(_tcsnicmp(argv[1+numParam], _T("/fi="), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-fi="), 4))) {
        _tcscpy(temp, &argv[1+numParam][4]);
        _stscanf(temp, _T("%d"), &MaxBuffSize);
      }
      bFWOnly = TRUE;
      SmallBufferIntel = TRUE;
      _stprintf(PrintBuffer, _T("Using small buffer - Intel\n"));
      _TPRINTF(PrintBuffer);
      _tcscpy_s(EncFile, MAX_PATH, argv[2+numParam]);
      encFile.open(EncFile, ifstream::in);
      if(!encFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      encFile.close();
	}
	// 2016/06/03. Miracle. -fh support HID Bootloader firmware upgrade. v1.
	else if (!(_tcsnicmp(argv[1 + numParam], _T("/fh"), 3)) || !(_tcsnicmp(argv[1 + numParam], _T("-fh"), 3))) {
		if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3 + numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4 + numParam)))) {
			ShowUsage();
			EXIT(iRetCode);
		}
		bFWOnly = TRUE;
		bHIDBLFW = TRUE;
		_tcscpy_s(EncFile, MAX_PATH, argv[2 + numParam]);
		encFile.open(EncFile, ifstream::in);
		if (!encFile.is_open()) {
			_stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
			_TPRINTF(PrintBuffer);
			EXIT(iRetCode);
		}
		encFile.close();
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/f"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-f"), 2))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      if (!(_tcsnicmp(argv[1+numParam], _T("/fs="), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-fs="), 4))) {
        _tcscpy(temp, &argv[1+numParam][4]);
        _stscanf(temp, _T("%d"), &FrameDelay);
      }
      bFWOnly = TRUE;
      _tcscpy_s(EncFile, MAX_PATH, argv[2+numParam]);
      encFile.open(EncFile, ifstream::in);
      if(!encFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      encFile.close();
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/w"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-w"), 2))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      if (!(_tcsnicmp(argv[1+numParam], _T("/wv"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-wv"), 3))) {
        VerifyConfigEnabled = TRUE;
      } else if (!(_tcsnicmp(argv[1+numParam], _T("/wx"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-wx"), 3))) {
        bIsXCfg = TRUE;
        if (!(_tcsnicmp(argv[1+numParam], _T("/wxv"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-wxv"), 4))) {
          VerifyConfigEnabled = TRUE;
        }
        if (!(_tcsnicmp(argv[1+numParam], _T("/wxk"), 4)) || !(_tcsnicmp(argv[1+numParam], _T("-wxk"), 4))) {
          ClearConfig = FALSE;
        }
      } else if (!(_tcsnicmp(argv[1+numParam], _T("/wraw"), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-wraw"), 5))) {
        bIsRaw = TRUE;
        if (!(_tcsnicmp(argv[1+numParam], _T("/wrawv"), 6)) || !(_tcsnicmp(argv[1+numParam], _T("-wrawv"), 6))) {
          VerifyConfigEnabled = TRUE;
        }
        if (!(_tcsnicmp(argv[1+numParam], _T("/wrawk"), 6)) || !(_tcsnicmp(argv[1+numParam], _T("-wrawk"), 6))) {
          ClearConfig = FALSE;
        }
      }
      bCfgOnly = TRUE;
      _tcscpy_s(CfgFile, MAX_PATH, argv[2+numParam]);
      cfgFile.open(CfgFile, ifstream::in);
      if(!cfgFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      } else {
        if (cfgFile.getline(line, LINE_MAX_SIZE) ) {
          if (!_tcsnicmp(line, _T("OBP_RAW V1"), 10)) {
            bIsRaw = TRUE;
            bIsXCfg = FALSE;
          } else if (!_tcsnicmp(line, _T("[COMMENTS]"), 10) || !_tcsnicmp(line, _T("[VERSION_INFO_HEADER]"), 21)) {
            // this is an extended config
            bIsXCfg = TRUE;
            bIsRaw = FALSE;
          } else if (bIsXCfg) { // Idiot-proof check...
            _stprintf(PrintBuffer, _T("The config file %s does not appear a valid extended cfg (.xcfg) file.\nMaybe it is a .cfg. Please verify it. Exiting...\n"), CfgFile);
            _TPRINTF(PrintBuffer);
            EXIT(iRetCode);
          }
        } else {
          _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
          _TPRINTF(PrintBuffer);
        }
      }
      cfgFile.close();
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/r"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-r"), 2))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      if (!(_tcsnicmp(argv[1+numParam], _T("/rx"), 3)) || !(_tcsnicmp(argv[1+numParam], _T("-rx"), 3))) {
        // Extended config
        bIsXCfg = TRUE;
      } else if (!(_tcsnicmp(argv[1+numParam], _T("/rraw"), 5)) || !(_tcsnicmp(argv[1+numParam], _T("-rraw"), 5))) {
        bIsRaw = TRUE;
      }
      bCfgRead = TRUE;
      _tcscpy_s(CfgFile, MAX_PATH, argv[2+numParam]);
      cfgFile.open(CfgFile, ofstream::out);
      if(!cfgFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      cfgFile.close();
    } else if (!(_tcsnicmp(argv[1+numParam], _T("/u"), 2)) || !(_tcsnicmp(argv[1+numParam], _T("-u"), 2))) {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      // Check if valid path for enc FW file
      _tcscpy_s(EncFile, MAX_PATH, argv[2+numParam]);
      encFile.open(EncFile, ifstream::in);
      if(!encFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      encFile.close();
      bPreserveCfg = TRUE;
    } else {
      if (!((!(DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes) && argc == 3+numParam) || ((DebugMode || LogMode || MaxTouchAnalyzer || MaxTouchStudio || ErrorCodes || DisableEnableDevice) && (argc == 4+numParam)))) {
        ShowUsage();
        EXIT(iRetCode);
      }
      _tcscpy_s(EncFile, MAX_PATH, argv[1+numParam]);
      _tcscpy_s(CfgFile, MAX_PATH, argv[2+numParam]);
      encFile.open(EncFile, ifstream::in);
      if(!encFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), EncFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
      cfgFile.open(CfgFile, ifstream::in);
      if(!cfgFile.is_open()) {
        _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      } else {
        if (cfgFile.getline(line, LINE_MAX_SIZE) ) {
          if (!_tcsnicmp(line, _T("[COMMENTS]"), 10)) {
            // this is an extended config
            bIsXCfg = TRUE;
          }
        } else {
          _stprintf(PrintBuffer, _T("Could not open: %s. \nCheck if it is a valid file/path. Exiting...\n"), CfgFile);
          _TPRINTF(PrintBuffer);
        }
      }
      encFile.close();
      cfgFile.close();
    }
  }
#else

#ifdef LENOVO_COLTRANE
  if (AllPids[0] & 0x8000) {
#ifdef ADD_MFC
      AfxMessageBox(_T("This update is either not necessary or not applicable to your system."), MB_OK | MB_ICONINFORMATION);
#else
    _tprintf(_T("\nThis update is either not necessary or not applicable to your system. Exiting...\n"));
    if (PauseBeforeExit) {
      _tprintf(_T("Press any key to continue . . .\n"));
      std::getchar();
    }
#endif
      exit(iRetCode);
  }

  if (WriteThqaString()) {
    _stprintf(PrintBuffer, _T("Process completed successfully.\n"));
    _tprintf(PrintBuffer);
    iRetCode = 0;
  } else {
    _stprintf(PrintBuffer, _T("Process completed with errors.\n"));
    _tprintf(PrintBuffer);
  }
  CleanSavedDeviceInfo();
  EXIT(iRetCode);
#endif

  // See if required to check PID for update applicability
  if (CheckDevicePid) {
    for (i = 0; i < NumberOfPayloads; ++i) {
      if (AllPids[0] == PidAll[i]) {
        CfgFileRes = CfgFileResAll[i];
        CfgFileSize = CfgFileSizeAll[i];
        ThqaFileRes = ThqaFileResAll[i];
        ThqaFileSize = ThqaFileSizeAll[i];
        PidToWrite = PidAll[i];
        ExpectedVersion = ExpectedVersionAll[i];
        ExpectedChecksum = ExpectedChecksumAll[i];
        ExpectedPid = PidAll[i];
        PayloadNum = i;
        PidFound = TRUE;
        break;
      }
    }
    if (CheckDevicePid && !PidFound) {
#ifdef ADD_MFC
      AfxMessageBox(_T("This update is not applicable to your system."), MB_OK | MB_ICONINFORMATION);
#else
      _TPRINTF(_T("\nThis update is not applicable to your system. Exiting...\n"));
      if (!SilentMode && PauseBeforeExit) {
        _tprintf(_T("Press any key to continue . . .\n"));
        std::getchar();
      }
#endif
      exit(iRetCode);
    }
  }

#endif

#ifdef MXT_VERIFIER
  bReturncode = ReadMemoryMap();
  BOOL bFWFlashable = CheckIfFWCanBeFlashed();
  if (!bReturncode) {
    _stprintf(PrintBuffer, _T("Cannot access device. Exiting.\n"));
    _TPRINTF(PrintBuffer);
    EXIT(iRetCode);
  }
  if (!GetPIDAndVersion()) {    
    _stprintf(PrintBuffer, _T("Cannot get FW/Cfg version.\n"));
    _TPRINTF(PrintBuffer);
    EXIT(iRetCode);
  }
  if (!ReadThqaString(ActualThqa)) {
    _stprintf(PrintBuffer, _T("Cannot read THQA blob.\n"));
    _TPRINTF(PrintBuffer);
    EXIT(iRetCode);
  }

  if (Version != ExpectedVersion) {
    _stprintf(PrintBuffer, _T("Found version 0x%x; expected version 0x%x\n\n"), Version, ExpectedVersion);
    _TPRINTF(PrintBuffer);
    if ((Version & 0xffff0000) != (ExpectedVersion & 0xffff0000)) {
      FWFlashNeeded = TRUE;
      if (!bFWFlashable) {
        _stprintf(PrintBuffer, _T("This verifier cannot flash FW: please do it in UEFI-BIOS shell.\n\n"));
        _TPRINTF(PrintBuffer);
        EXIT(iRetCode);
      }
    }
  } else {
    _stprintf(PrintBuffer, _T("Found correct FW and Config version 0x%x\n\n"), Version);
    _TPRINTF(PrintBuffer);
  }
  if (FWFlashNeeded) {
    if (FlashFWUsb() == -1) {
      _stprintf(PrintBuffer, _T("Could not flash FW successfully.\n\n"));
      _TPRINTF(PrintBuffer);
      iRetCode = eFWError;
      EXIT(iRetCode);
    }
    iRetCode = eSuccess;
    FWFlashNeeded = FALSE;
  }

  // read config file (resource)
  if (!ParseReadConfigXFile(CfgFile)) {
    _stprintf(PrintBuffer, _T("Cannot read internal config file. Exiting.\n\n"));
    _TPRINTF(PrintBuffer);
    EXIT(iRetCode);
  }
  if (!VerifyConfig()) {
    _stprintf(PrintBuffer, _T("Configuration mismatch!\n\n"));
    _TPRINTF(PrintBuffer);
    CfgStoringNeeded = TRUE;
    // Set also verification of config after writing
    VerifyConfigEnabled = TRUE;

  } else {
    _stprintf(PrintBuffer, _T("Configuration verified OK.\n\n"));
    _TPRINTF(PrintBuffer);
  }

  if (ProductID != ExpectedPid) {
    _stprintf(PrintBuffer, _T("Found PID 0x%x; expected PID 0x%x\n\n"), ProductID, ExpectedPid);
    _TPRINTF(PrintBuffer);
    PIDStoringNeeded = TRUE;
  } else {
    _stprintf(PrintBuffer, _T("Found correct PID 0x%x\n\n"), ProductID);
    _TPRINTF(PrintBuffer);
  }

  if (memcmp(ActualThqa, ThqaFileRes, 0x100)) {
    _stprintf(PrintBuffer, _T("THQA blob is different than expected.\n\n"));
    _TPRINTF(PrintBuffer);
    THQAStoringNeeded = TRUE;
  } else {
    _stprintf(PrintBuffer, _T("THQA blob matches the expected values.\n\n"));
    _TPRINTF(PrintBuffer);
  }

  if (CfgStoringNeeded) {
    if (ConfigUpdate()) {
      _stprintf(PrintBuffer, _T("Cannot write configuration.\n\n"));
      _TPRINTF(PrintBuffer);
      iRetCode = eCfgError;
      EXIT(iRetCode);
    }
    Sleep(3000);
    // forget previous discovery...
    if (!RediscoverDevice()) {
      _stprintf(PrintBuffer, _T("RediscoverDevice() failed.\n\n"));
      _TPRINTF(PrintBuffer);
      EXIT(iRetCode);
    }
    if (!VerifyConfig()) {
      _stprintf(PrintBuffer, _T("\nConfiguration still mismatched!\n\n"));
      _TPRINTF(PrintBuffer);
      iRetCode = eCfgError;
    } else {
      CfgStoringNeeded = FALSE;
    }
  }

  if (PIDStoringNeeded || THQAStoringNeeded) {
    _stprintf(PrintBuffer, _T("\nUpdating PID and Thqa blob...\n\n"));
    _TPRINTF(PrintBuffer);
    iRetCode = ePIDError;
    if (WriteThqaString()) {
      Sleep(5000);
      // forget previous discovery...
      if (!RediscoverDevice()) EXIT(-1);
      for (i = 0; i < 5; ++i) {
        if ((UINT16)AllPids[DeviceIndex-1] == ExpectedPid) {
          PIDStoringNeeded = FALSE;
          break;
        }
        Sleep(3000);
      }
      if (i == 5) {
        _stprintf(PrintBuffer, _T("Updating PID failed.\n\n"));
        _TPRINTF(PrintBuffer);
        iRetCode = ePIDError;
        EXIT(iRetCode);
      }
      if (ReadThqaString(ActualThqa)) {
        iRetCode = eThqaError;
        if (!memcmp(ActualThqa, ThqaFileRes, 0x100)) {
          if (!PIDStoringNeeded && !CfgStoringNeeded && !FWFlashNeeded) {
            _stprintf(PrintBuffer, _T("All the Touch parameters in this Astro unit are correct. Exiting...\n"));
            _TPRINTF(PrintBuffer);
            iRetCode = eSuccess;
          }
        }
        EXIT(iRetCode);
      }
    }
    _stprintf(PrintBuffer, _T("Updating PID and/or Thqa blob failed.\n"));
    _TPRINTF(PrintBuffer);
    EXIT(iRetCode);
  }
  _stprintf(PrintBuffer, _T("All the Touch parameters in this Astro unit are correct. Exiting...\n"));
  _TPRINTF(PrintBuffer);
  iRetCode = eSuccess;

  EXIT(iRetCode);
}

BOOL CheckIfFWCanBeFlashed()
{
  if (ConnectFWType == HIDUSB) return TRUE;
  // In the future we can support MxtBootBridge ATML2000 driver
  // only if we have a commercial device with it
  return FALSE;
}

int FlashFWUsb()
{
  int iRetCode = -1;  // Set it as error
  int i;
  Sleep(500);
  // In case we have an I2C device already in bootloader mode...
  if (!DeviceAlreadyIdentified) {
    // I2C device in bootloader mode?
    for (i = 0; i < 5; i++) {
      Sleep(3000*i);
      if (FindAtmelDeviceBootloaderMode()) {
        break;
      }
    }
    if (i != 5) {
      bBootloaderMode = TRUE;
    }
  }
  if (!bBootloaderMode) {
    // Go in bootloader mode
    BOOL bReturncode = SetmXTDeviceInBootloadMode();
    if (!bReturncode) {
      _stprintf(PrintBuffer, _T("Command to set device in Bootloader mode failed.\nPlease try power-cycling the device. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      return (iRetCode);
    }
    Sleep(1000);
    // forget previous discovery...
    CleanSavedDeviceInfo();
    for (i = 0; i < 3; i++) {
      Sleep(3000*i);
      if (FindAtmelDeviceBootloaderMode()) {
        break;
      }
    }
    if (i == 3) {
      _stprintf(PrintBuffer, _T("Cannot detect if the device is in Bootloader mode.\n"));
      _TPRINTF(PrintBuffer);
      return (iRetCode);
    }
  }

  // Create an event to synchronize device access...
  ReadWriteEventAttribs.bInheritHandle = TRUE;
  ReadWriteEventAttribs.lpSecurityDescriptor = NULL;
  ReadWriteEventAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);

  // Create an event to synchronize device access...
  CompletionEventAttribs.bInheritHandle = TRUE;
  CompletionEventAttribs.lpSecurityDescriptor = NULL;
  CompletionEventAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);

  hReadWriteEvent = CreateEvent(&ReadWriteEventAttribs, FALSE, FALSE, NULL);
  hCompletionEvent = CreateEvent(&CompletionEventAttribs, FALSE, FALSE, NULL);
  // Start a data receiving thread...
  _beginthread(ReadDevice, 0, NULL);

  FindBootloader();
  // Wait here for all detection to happen: 3 sec. are plenty
  Sleep(3000);
  // Check status: if we had a disconnect/wrong state we need to EXIT
  // and power-cycle the device. Else we continue.
  if (bStateOk) {
    if (CheckBootloaderOnly) {
      WaitForSingleObjectEx(hCompletionEvent, INFINITE, 0);
      CleanSavedDeviceInfo(); // the stored objects data is not good any more...
      return(iRetCode);
    }
    // Here we have config file on CfgFile and FW file on EncFile
    _stprintf(PrintBuffer, _T("\nLoading firmware file %s\n\n"), EncFile);
    _TPRINTF(PrintBuffer);
    if (ProgramChip()) {
      WaitForSingleObjectEx(hCompletionEvent, INFINITE, 0);
      // When here, FW has been updated...
      CleanSavedDeviceInfo(); // the stored objects data is not good any more...
      for (i = 0; i < 3; i++) {
        Sleep(2000);
        if (FindDiscoverableDevice() == USB_NORMAL) {
          iRetCode = 0;
          break;
        }
      }
    } else {
      _stprintf(PrintBuffer, _T("Firmware update process failed. Please check the binary file provided and try again. Exiting...\n"));
      _TPRINTF(PrintBuffer);
      return(iRetCode);
    }
  }
  return iRetCode;
}

BOOL RediscoverDevice()
{
  CleanSavedDeviceInfo();
  if (IdentifyConnectFWType() < 0) {
    _tprintf(_T("Could not identify any maXTouch device. Please power-cycle and run again.\n\n"));
    return(FALSE);
  }
  if (!ReadMemoryMap(TRUE)) {
    _stprintf(PrintBuffer, _T("Cannot access device. Exiting.\n"));
    _TPRINTF(PrintBuffer);
    return(FALSE);
  }
  return TRUE;
}

#else
  // Now search for Atmel devices - if not done already the automatic way
  _stprintf(PrintBuffer, _T("Searching for maxTouch devices...\n"));
  _TPRINTFD(PrintBuffer);
  // See if there is any device attached; if not EXIT (this saves also the index - 0 or 1 - 1386 or 768E etc...
  if(FindAtmelDevice()) {
    ZeroMemory(PrintBuffer, 512);
    if (XMLFilePresent) {
      _stprintf(PrintBuffer, _T("%s\n"), FromXmlFileMsg);
    }
    if (DeviceAppFromXML) {
      _stprintf(PrintBuffer, _T("%sFound %s device #%d\n"), PrintBuffer, DeviceAppNameFromXML, DeviceIndex);
    } else {
      _stprintf(PrintBuffer, _T("Found %s device #%d\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
    }
    _TPRINTFD(PrintBuffer);
    _stprintf(PrintBuffer, _T("%s\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
    _TPRINTFD(PrintBuffer);

#ifdef AUTO_UPDATER
    if (!IsFWUpdateRequested || IsFWVersionSame()) {
      FWNotNeeded = TRUE;
      if (IsCfgVersionSame() && IsCfgChecksumSame()) {
        CfgNotNeeded = TRUE;
      }
      if (ExpectedPid == AllPids[0] && IsThqaSame()) {
        ThqaNotNeeded = TRUE;
      }
    }
    if (FWNotNeeded && CfgNotNeeded && ThqaNotNeeded) {
#ifdef ADD_MFC
      AfxMessageBox(_T("The touch-screen controller is already up-to-date."), MB_OK | MB_ICONINFORMATION);
#else
      _TPRINTF(_T("\nThe touch-screen controller is already up-to-date. Exiting...\n"));
      if (!SilentMode && PauseBeforeExit) {
        _tprintf(_T("Press any key to continue . . .\n"));
        std::getchar();
      }
#endif
      CleanSavedDeviceInfo();
      iRetCode = 0;
      exit(iRetCode);
    } else {
      if (CheckACPower) {
        SYSTEM_POWER_STATUS SystemPowerStatus2;
        if (GetSystemPowerStatus(&SystemPowerStatus2)) {
          if (!SystemPowerStatus2.ACLineStatus) {
            _TPRINTF(_T("\nTo perform this update, your system must be connected to AC Power.\n"));
            if (!SilentMode && PauseBeforeExit) {
              _tprintf(_T("Press any key to continue . . .\n"));
              std::getchar();
              exit(iRetCode);
            }
          }
        }
      }
    }
#endif

  } else {
	  if (!FindAtmelDeviceBootloaderMode(FIRST_DISCOVERY, TRUE)) {
      if (BootloaderId) {
        _stprintf(PrintBuffer, _T("MaXTouch device is in bootloader mode, but not responding. Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        if (ErrorCodes) iRetCode = FOUND_DEVICE_BOOTLOADER;
      } else {
        _stprintf(PrintBuffer, _T("No supported maXTouch device found. Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        if (ErrorCodes) iRetCode = NO_DEVICE_FOUND;
      }
      CleanSavedDeviceInfo();
      EXIT(iRetCode);
    } else {
      if (DeviceConnected) {
        _stprintf(PrintBuffer, _T("Found %s device #%d in bootloader mode\n"), DEVICE_NAME[DeviceConnected], DeviceIndex);
        _TPRINTFD(PrintBuffer);
        _stprintf(PrintBuffer, _T("%s\n\n"), IDENTIFY_CONNECTION_MSG[ConnectFWType]);
        _TPRINTFD(PrintBuffer);
      } else {
        if (BootloaderId) {
          if (DeviceBlrFromXML) {
            // print up the chip info 
            _stprintf(PrintBuffer, _T("Found %s in bootloader mode.\n"), DeviceBtlNameFromXML);
          } else {
            BootloaderId = 0;
            // print up the chip info 
            _stprintf(PrintBuffer, _T("Found %s in bootloader mode.\n"), version_decoder_name[BootloaderId]);
          }
          _TPRINTFD(PrintBuffer);
        } else {
          // here if DeviceConnected = 0 and BootloaderId = 0
          _stprintf(PrintBuffer, _T("Found unknown device in bootloader mode\n"));
          _TPRINTFD(PrintBuffer);
        }
      }
      bBootloaderMode = TRUE;
      if (bCfgOnly || bCfgRead || bPreserveCfg) {
        _stprintf(PrintBuffer, _T("When the device is in bootloader mode, only firmware update is possible\n"));
        _TPRINTFD(PrintBuffer);
        _stprintf(PrintBuffer, _T("If you like to attempt saving the device's configuration, please try power-cycling it. Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        if (ErrorCodes) iRetCode = FOUND_DEVICE_BOOTLOADER;
        EXIT(iRetCode);
      }
    }
  }

  if (bHIDBLFW) {
	  _stprintf(PrintBuffer, _T("Enter HID Bootloader programming...\n"));
	  _TPRINTFD(PrintBuffer);
	  iRetCode = FWUpgrade_HIDBootloader();
	  EXIT(iRetCode);
  }

  // If it is a configuration update only...
  if (bCfgOnly) {
    if (bIsXCfg) {
      _stprintf(PrintBuffer, _T("\nLoading extended configuration file %s\n\n"), CfgFile);
      _TPRINTFD(PrintBuffer);
    } else {
      _stprintf(PrintBuffer, _T("\nLoading configuration file %s\n\n"), CfgFile);
      _TPRINTFD(PrintBuffer);
    }
    iRetCode = ConfigUpdate();
    CleanSavedDeviceInfo();
    EXIT(iRetCode);
  }
  // If it is a configuration reading only...
  if (bCfgRead) {
    if (ReadConfig()) {
      _stprintf(PrintBuffer, _T("\nDevice configuration has been successfully written to %s file. Exiting...\n"), CfgFile);
      _TPRINTFD(PrintBuffer);
      iRetCode = 0; // Success...
    } else {
      _stprintf(PrintBuffer, _T("\nFailed to write configuration to file %s. Exiting...\n"), CfgFile);
      _TPRINTFD(PrintBuffer);
      DeleteFile(CfgFile);
      if (ErrorCodes) iRetCode = CFG_READING_FAILED;
    }
    CleanSavedDeviceInfo();
    EXIT(iRetCode);
  }
  // If it is a fw update, preserving the configuration
  // we need to save the configuration on a temp file
  if (bPreserveCfg) {
    // Attempt to create a temp file and save device configuration in it...
    if (!(GetTempFile(CfgFile) && ReadConfig())) {
      _stprintf(PrintBuffer, _T("Unable to save device configuration. Please try saving it explicitly using /r switch. Exiting...\n"));
      _TPRINTFD(PrintBuffer);
      CleanSavedDeviceInfo();
      EXIT(iRetCode);
    }
  }

  Sleep(500);
  // In case we have an I2C device already in bootloader mode...
//  if (ConnectFWType != HIDUSB && !DeviceConnected) {
  if (!DeviceAlreadyIdentified) {
    // I2C device in bootloader mode?
    for (i = 0; i < 3; i++) {
      Sleep(1000);
      if (FindAtmelDeviceBootloaderMode()) {
        break;
      }
    }
    if (i != 3) {
      bBootloaderMode = TRUE;
    }
  }

#ifdef AUTO_UPDATER
    if (FWNotNeeded) {
      if (CfgNotNeeded) {
        iRetCode = 0;
        goto DoThqa;
      } else {
        goto DoConfig;
      }
    }
#endif
  if (!bBootloaderMode) {
    // Go in bootloader mode
    bReturncode = SetmXTDeviceInBootloadMode();
    if (!bReturncode) {
      _stprintf(PrintBuffer, _T("Command to set device in Bootloader mode failed.\nPlease try power-cycling the device. Exiting...\n"));
      _TPRINTFD(PrintBuffer);
      EXIT(iRetCode);
    }
	else {	//MCG - 2016-08-24 -- Added to indicate Flash command was sent
	 _stprintf(PrintBuffer, _T("Switch to bootloader command sent successfully.\n"));
	 _TPRINTFD(PrintBuffer);
	 bFlashCmdSent = TRUE;
	}

    Sleep(1000);
    // forget previous discovery...
//    CleanSavedDeviceInfo();
	for (i = 0; i < 3; i++) {
		Sleep(4000);
//		bQuiet = TRUE;
		retError = (FindAtmelDeviceBootloaderMode(SECOND_DISCOVERY, TRUE));  // Check why SECOND_DISCOVERY here, MCG 2016-08-24, set to Quiet	
		  if (retError == TRUE) {
			break;
		}
	}

	if (i == 3) {
		if (retError == -1) {
			_stprintf(PrintBuffer, _T("MxtBootBridge.sys not installed. Skipping . . . \n"));
			_TPRINTFD(PrintBuffer);
			EXIT(iRetCode);
		}
		else if (retError == 0) {
			_stprintf(PrintBuffer, _T("Cannot detect if the device is in Bootloader mode.\nThere may be a problem with the ACPI table addresses. Exiting...\n"));
			_TPRINTFD(PrintBuffer);
			_stprintf(PrintBuffer, _T("Attempt to reset the bootloader failed.  Exiting...\n"));
			_TPRINTFD(PrintBuffer);
			EXIT(iRetCode);
		}
		else {
		}
	 }
  }

  // Check if HID-USB...
  if (ConnectFWType == HIDUSB) {
    // Create an event to synchronize device access...
    ReadWriteEventAttribs.bInheritHandle = TRUE;
    ReadWriteEventAttribs.lpSecurityDescriptor = NULL;
    ReadWriteEventAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);

    // Create an event to synchronize device access...
    CompletionEventAttribs.bInheritHandle = TRUE;
    CompletionEventAttribs.lpSecurityDescriptor = NULL;
    CompletionEventAttribs.nLength = sizeof(SECURITY_ATTRIBUTES);

    hReadWriteEvent = CreateEvent(&ReadWriteEventAttribs, FALSE, FALSE, NULL);
    hCompletionEvent = CreateEvent(&CompletionEventAttribs, FALSE, FALSE, NULL);
    // Start a data receiving thread...
    _beginthread(ReadDevice, 0, NULL);

    FindBootloader();
    // Wait here for all detection to happen: 3 sec. are plenty
    Sleep(3000);
    // Check status: if we had a disconnect/wrong state we need to EXIT
    // and power-cycle the device. Else we continue.
    if (bStateOk) {
      if (CheckBootloaderOnly) {
        WaitForSingleObjectEx(hCompletionEvent, INFINITE, 0);
        CleanSavedDeviceInfo(); // the stored objects data is not good any more...
        EXIT(iRetCode);
      }
      // Here we have config file on CfgFile and FW file on EncFile
      _stprintf(PrintBuffer, _T("\nLoading firmware file %s\n\n"), EncFile);
      _TPRINTFD(PrintBuffer);
      if (ProgramChip()) {
        WaitForSingleObjectEx(hCompletionEvent, INFINITE, 0);
        // When here, FW has been updated...
        CleanSavedDeviceInfo(); // the stored objects data is not good any more...
      } else {
        _stprintf(PrintBuffer, _T("Firmware update process failed. Please check the binary file provided and try again. Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        EXIT(iRetCode);
      }
      // If only FW update was requested, still CRC needs to be saved, otherwise we get a CfgErr
      int retCode;
      Sleep(5000);
      if (bFWOnly) {
        // Wait for chip to come back from FW update...
        for (i = 0; i < 3; i++) {
          Sleep(2000);
          retCode = FindDiscoverableDevice();
          if (retCode == USB_NORMAL) {
            if (ReadMemoryMap(TRUE)) {
              break;
            }
          }
          CleanSavedDeviceInfo();
        }
        if (i == 3) {      
          _stprintf(PrintBuffer, _T("Unable to reset the device. Exiting...\n"));
          _TPRINTFD(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("Resetting the device and exiting...\n"));
          _TPRINTFD(PrintBuffer);
          ResetDevice();
          iRetCode = 0; // Success...
        }
      } else {
        // Update configuration...
        if (!bPreserveCfg) {
          if (bIsXCfg) {
            _stprintf(PrintBuffer, _T("\nLoading extended configuration file %s\n\n"), CfgFile);
            _TPRINTFD(PrintBuffer);
          } else {
            _stprintf(PrintBuffer, _T("\nLoading configuration file %s\n\n"), CfgFile);
            _TPRINTFD(PrintBuffer);
          }
        }
        for (i = 0; i < 3; i++) {
          Sleep(2000);
          retCode = FindDiscoverableDevice();
          if (retCode == USB_NORMAL) {
            if (ReadMemoryMap(TRUE)) {
              iRetCode = ConfigUpdate();
              break;
            }
          }
          // Clean and perform a rediscovery
          CleanSavedDeviceInfo();
        }
        if (i == 3) {
          _stprintf(PrintBuffer, _T("Unable to write configuration to the device. Exiting...\n"));
          _TPRINTFD(PrintBuffer);
        }
      }
    }
    // We get here if HID-I2C...
  } else {
    if (CheckBootloaderOnly) {
      iRetCode = 0;
      if (!LeaveInBootloaderMode) {
        ExitBootloader();
      }
    } else {
      _stprintf(PrintBuffer, _T("Device is in Bootloader mode.\n"));
      _TPRINTFD(PrintBuffer);

      BootWorkerUnlock();

      _stprintf(PrintBuffer, _T("\nLoading firmware file %s\n\n"), EncFile);
      _TPRINTFD(PrintBuffer);

      iRetCode = WriteFWToDevice();
      if (iRetCode) {
        _stprintf(PrintBuffer, _T("Firmware update process failed. Please check the binary file provided and try again.\nYou should also power-cycle the device. Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        EXIT(iRetCode);
      }
      // New FW has been loaded: memory map is likely changed
      CleanSavedDeviceInfo();

      // temporary patch: exits and re-run mxtupdater -de, since disable/enable after FW update does not work.
      if (DisableEnableDevice) {
        EXIT(iRetCode);
      }

#if 0
      Sleep(10000);
      // this is not working yet...
      if (DisableEnableDevice) {
        UINT retCode = DisableDevice(_T("ACPI\\ATML1000"), TRUE);
        if (!retCode) {
          Sleep(2000);
          retCode = DisableDevice(_T("ACPI\\ATML1000"), TRUE);
        }
        if (!retCode) { // success
          Sleep(5000);
          if (!DisableDevice(_T("ACPI\\ATML1000"), FALSE)) {
            _stprintf(PrintBuffer, _T("\nTouch Device was successfully disabled and enabled\n"));
            _TPRINTFD(PrintBuffer);
          } else {
            _stprintf(PrintBuffer, _T("\nThere was an error re-enabling Touch Device\n"));
            _TPRINTFD(PrintBuffer);
             EXIT(-1);
          }
        } else {
          switch (retCode) {
          case 1:
            _stprintf(PrintBuffer, _T("\nDevice Enumeration failed: unable to disable/enable device\n"));
            _TPRINTFD(PrintBuffer);
            break;
          case 2:
            _stprintf(PrintBuffer, _T("\nDevice not found: unable to disable/enable device\n"));
            _TPRINTFD(PrintBuffer);
            break;
          case 3:
            _stprintf(PrintBuffer, _T("\nInsufficient rights: you must run in an elevated command prompt (Administrator prompt)\n"));
            _TPRINTFD(PrintBuffer);
            break;
          default:
            _stprintf(PrintBuffer, _T("\nError %d: unable to disable/enable device\n"), retCode);
            _TPRINTFD(PrintBuffer);
          }
          EXIT(-1);
        }
      }
#endif

      // If only FW update was requested, still CRC needs to be saved, otherwise we get a CfgErr
      Sleep(5000);
      for (i = 0; i < 3; i++) {
        Sleep(2000);
        if (IdentifyConnectFWType() >= 0) {
          break;
        }
        CleanSavedDeviceInfo(); // in case we are saving old info
      }
      if (i == 3) {
        _stprintf(PrintBuffer, _T("Cannot communicate with the maXTouch device: make sure that proper drivers are loaded. Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        EXIT(iRetCode);
      }
      if (!ValidateDevice()) {
        _stprintf(PrintBuffer, _T("Please verify that you have the appropriate Windows driver installed (you can use Device Manager). Exiting...\n"));
        _TPRINTFD(PrintBuffer);
        EXIT(iRetCode);
      }
      if (bFWOnly) {
        // Wait for chip to come back from FW update...
        for (i = 0; i < 3; i++) {
          Sleep(2000);
          if (FindAtmelDevice()) {
            _stprintf(PrintBuffer, _T("Resetting the device and exiting...\n"));
            _TPRINTFD(PrintBuffer);
            ResetDevice();
            iRetCode = 0; // Success...
            break;
          }
          CleanSavedDeviceInfo();
        }
        if (i == 3) {
          for (i = 0; i < 3; i++) {
            Sleep(2000);
            if (FindAtmelDevice()) {
              _stprintf(PrintBuffer, _T("Resetting the device and exiting...\n"));
              _TPRINTFD(PrintBuffer);
              ResetDevice();
              iRetCode = 0; // Success...
              break;
            }
            CleanSavedDeviceInfo();
          }
          if (i == 3) {
            _stprintf(PrintBuffer, _T("Unable to reset the device. Exiting...\n"));
            _TPRINTFD(PrintBuffer);
            if (ErrorCodes) iRetCode = FW_UPDATE_FAILED;
          }
        }
      } else {
#ifdef AUTO_UPDATER
DoConfig:
         _stprintf(PrintBuffer, _T("\nNow updating configuration: please wait...\n\n"));
         _TPRINTF(PrintBuffer);
#endif
        // Update configuration...
        if (!bPreserveCfg) {
          if (bIsXCfg) {
            _stprintf(PrintBuffer, _T("\nLoading extended configuration file %s\n\n"), CfgFile);
            _TPRINTFD(PrintBuffer);
          } else {
            _stprintf(PrintBuffer, _T("\nLoading configuration file %s\n\n"), CfgFile);
            _TPRINTFD(PrintBuffer);
          }
        }
        for (i = 0; i < 3; i++) {
          Sleep(5000);
          if (FindAtmelDevice()) {
            iRetCode = ConfigUpdate();
            break;
          }
          CleanSavedDeviceInfo();
        }
        if (i == 3) {      
          for (i = 0; i < 3; i++) {
            Sleep(2000);
            if (FindAtmelDevice()) {
              iRetCode = ConfigUpdate();
              break;
            }
            CleanSavedDeviceInfo();
          }
          if (i == 3) {
            _stprintf(PrintBuffer, _T("Unable to write configuration to the device.\n"));
            _TPRINTFD(PrintBuffer);
            if (ErrorCodes) iRetCode = CFG_WRITING_FAILED;
          }
        }
      }
    }
  }

#ifdef AUTO_UPDATER
DoThqa:
  // Make sure that FW and Config update was successful
  if (iRetCode == 0) {
    if (WriteThqa) {
      iRetCode = -1;
      Sleep(5000);
      CleanSavedDeviceInfo();
      Sleep(3000);
      if (IdentifyConnectFWType() < 0) {
        _stprintf(PrintBuffer, _T("Cannot locate the touch-screen controller.\n"));
        _TPRINTFD(PrintBuffer);
        CleanSavedDeviceInfo();
        EXIT(iRetCode);
      }
//DoThqa2:
      if (WriteThqaString()) {
        _stprintf(PrintBuffer, _T("\nPID and THQA blob were successfully written to maXTouch device.\n"));
        _TPRINTFD(PrintBuffer);
#ifndef END_USER
        _TPRINTF(PrintBuffer);
#endif
        iRetCode = 0;
      } else {
        _stprintf(PrintBuffer, _T("\nError saving PID and THQA blob.\n"));
        _TPRINTFD(PrintBuffer);
#ifndef END_USER
        _TPRINTF(PrintBuffer);
#endif
      }
    }
    if (TurnOnThqa) {
      Sleep(1000);
      CleanSavedDeviceInfo();
      if (IdentifyConnectFWType() < 0) {
        _stprintf(PrintBuffer, _T("Cannot locate the touch-screen controller.\n"));
        _TPRINTFD(PrintBuffer);
        CleanSavedDeviceInfo();
        EXIT(iRetCode);
      }
      ActivatePidThqa(TRUE);
    }
  }
#endif

  CleanSavedDeviceInfo();
#ifdef OCULAR_TOSHIBA
  if  (!bStateOk) {
    bStateOk = TRUE;
    ++FWRetries;
    if (FWRetries < 10) {
      goto startover;
    }
  }
#endif
  EXIT(iRetCode);
}

#endif


BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if ( !LookupPrivilegeValue( 
            NULL,            // lookup privilege on local system
            lpszPrivilege,   // privilege to lookup 
            &luid ) )        // receives LUID of privilege
    {
      _stprintf(PrintBuffer, _T("LookupPrivilegeValue error: %u\n"), GetLastError());
      _TPRINTFD(PrintBuffer); 
      return FALSE; 
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.

    if ( !AdjustTokenPrivileges(
           hToken, 
           FALSE, 
           &tp, 
           sizeof(TOKEN_PRIVILEGES), 
           (PTOKEN_PRIVILEGES) NULL, 
           (PDWORD) NULL) )
    { 
      _stprintf(PrintBuffer, _T("AdjustTokenPrivileges error: %u\n"), GetLastError());
      _TPRINTFD(PrintBuffer); 
      return FALSE; 
    } 

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

    {
      _stprintf(PrintBuffer, _T("The token does not have the specified privilege.\n"));
      _TPRINTFD(PrintBuffer); 
      return FALSE;
    } 
    return TRUE;
}

void SystemReboot(BOOL Restart)
{
  HANDLE hCurrProc, hToken;

  hCurrProc = GetCurrentProcess();

  if (OpenProcessToken(hCurrProc, TOKEN_WRITE, &hToken)) {
    if (SetPrivilege(hToken, _T("SeShutdownPrivilege"), TRUE)) {
      InitiateSystemShutdownEx(NULL, NULL, 10, FALSE, Restart, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED);
      return;
    }
  }
  _TPRINTF(_T("\nYou need to restart the system to apply the changes.\n"));
}

void ShowUsage()
{
  _tprintf(_T("USAGE:\n\n"));
  _tprintf(_T("  MxtUpdater FWFile.enc Config.cfg -> updates FW and configuration\n"));
  _tprintf(_T("  MxtUpdater -f FWFile.enc ---------> updates firmware\n"));
  _tprintf(_T("  MxtUpdater -fs=nn FWFile.enc -----> updates firmware, introducing a delay of nn millisec per frame\n"));
  _tprintf(_T("  MxtUpdater -fi FWFile.enc --------> updates firmware using 32-bit buffers\n"));
  _tprintf(_T("  MxtUpdater -fi=nn FWFile.enc -----> updates firmware using nn-bit buffers\n"));
  _tprintf(_T("  MxtUpdater -fd FWFile.enc --------> updates firmware using delay of 20 ms\n"));
  _tprintf(_T("  MxtUpdater -fd=nn FWFile.enc -----> updates firmware using delay of nn ms\n"));
  _tprintf(_T("  MxtUpdater -fid FWFile.enc -------> updates firmware using 32-bit buffers and delays\n"));
  _tprintf(_T("  MxtUpdater -fid=nn FWFile.enc ----> updates firmware using nn-bit buffers and delays\n"));
  _tprintf(_T("  MxtUpdater -fh FWFile.enc --------> updates firmware for HIDI2C devices using S2 bootloader\n"));
  _tprintf(_T("  MxtUpdater -f FWFile.enc -de -----> after FW flashing, exits to avoid getting stuck\n"));
  _tprintf(_T("  MxtUpdater -de -------------------> disable and re-enable I2C controller (like an HW reset)\n"));
  _tprintf(_T("  MxtUpdater -u FWFile.enc ---------> restores configuration after updating the firmware\n"));
  _tprintf(_T("  MxtUpdater -w Config.cfg ---------> updates configuration\n"));
  _tprintf(_T("  MxtUpdater -wv Config.cfg --------> updates configuration and verifies it\n"));
  _tprintf(_T("  MxtUpdater -r Config.cfg ---------> saves device configuration to the specified file\n"));
  _tprintf(_T("  MxtUpdater -wx Config.xcfg -------> updates configuration using extended format\n"));
  _tprintf(_T("  MxtUpdater -wxv Config.xcfg ------> updates configuration using extended format and verifies it\n"));
  _tprintf(_T("  MxtUpdater -wxk Config.xcfg ------> updates configuration without clearing first the existing one\n"));
  _tprintf(_T("  MxtUpdater -rx Config.xcfg -------> saves device extended configuration to the specified file\n"));
  _tprintf(_T("  MxtUpdater -wraw Config.raw ------> updates configuration using extended format\n"));
  _tprintf(_T("  MxtUpdater -wrawv Config.raw -----> updates configuration using extended format and verifies it\n"));
  _tprintf(_T("  MxtUpdater -wrawk Config.raw -----> updates configuration without clearing first the existing one\n"));
  _tprintf(_T("  MxtUpdater -rraw Config.raw ------> saves device extended configuration to the specified file\n"));
  _tprintf(_T("  MxtUpdater -x2raw Config.xcfg ----> converts a .xcfg config to raw format and saves it to: Config.raw\n"));
  _tprintf(_T("  MxtUpdater -reboot ---------------> restart your Windows system\n"));
  _tprintf(_T("  MxtUpdater -wcc ------------------> clears configuration in memory (run '-backup' to make it permanent)\n"));
  _tprintf(_T("  MxtUpdater -i --------------------> identifies and provides info on maXTouch device\n"));
  _tprintf(_T("  MxtUpdater -l --------------------> lists all the Atmel HID devices and interfaces connected\n"));
  _tprintf(_T("  MxtUpdater -ld--------------------> lists all the Atmel HID device control interfaces connected\n"));
  _tprintf(_T("  MxtUpdater -ld=n -----------------> selects device #n (from the \"-ld\" list) for operations\n"));
  _tprintf(_T("  MxtUpdater -i2c ------------------> selects only a 'i2c' connected device for operations\n"));
  _tprintf(_T("  MxtUpdater -usb ------------------> selects only a 'usb' connected device for operations\n"));
  _tprintf(_T("  MxtUpdater -cs=\"ANYSTRING\" -------> selects device with custom string: \"ANYSTRING\" (if present)\n"));
  _tprintf(_T("  MxtUpdater -c --------------------> displays configuration checksum in NVM\n"));
  _tprintf(_T("  MxtUpdater -cm -------------------> displays configuration checksum in-memory\n"));
  _tprintf(_T("  MxtUpdater -b --------------------> displays bootloader version\n"));
  _tprintf(_T("  MxtUpdater -bl -------------------> displays bootloader version and leave device in bootloader\n"));
  _tprintf(_T("  MxtUpdater -reset ----------------> resets maXTouch device\n"));
  _tprintf(_T("  MxtUpdater -calib ----------------> calibrates maXTouch device\n"));
  _tprintf(_T("  MxtUpdater -backup ---------------> backs up maXTouch device (standard)\n"));
  _tprintf(_T("  MxtUpdater -backup=0xNN ----------> backs up with code 0xNN (only 0x33, 0x44 and 0x55 allowed)\n"));
  _tprintf(_T("  MxtUpdater -gr -------------------> stores golden references on device\n"));
  _tprintf(_T("  MxtUpdater -gr=n -----------------> stores golden references, retries 'n' times if fails\n"));
  _tprintf(_T("  MxtUpdater -t68 params.bin -------> stores heat map parameters on OpenTouch device\n"));
  _tprintf(_T("  MxtUpdater -pid=0xNNNN -thqa=thqa.bin --> stores PID and THQA blob on device\n"));
  _tprintf(_T("  MxtUpdater -sw <string> ----------> write Serial Number string to device\n"));
  _tprintf(_T("  MxtUpdater -sr serialnum.txt -----> read Serial Number string to file.\n"));
  _tprintf(_T("  MxtUpdater -tw pid_thqa.txt-------> stores PID and THQA blob on device\n"));
  _tprintf(_T("  MxtUpdater -tr thqa.bin-----------> reads PID and THQA blob from device\n"));
  _tprintf(_T("  MxtUpdater -thqa ON --------------> reports PID and THQA blob downloaded\n"));
  _tprintf(_T("  MxtUpdater -thqa OFF -------------> reports preloaded PID and THQA blob\n"));
  _tprintf(_T("  MxtUpdater -pid ------------------> reads and reports PID from device\n"));
  _tprintf(_T("  MxtUpdater -refs refs.csv --------> saves references values to file: refs.csv\n"));
  _tprintf(_T("  MxtUpdater -deltas deltas.csv ----> saves deltas values to file: deltas.csv\n"));
  _tprintf(_T("  MxtUpdater -refsc refsc.csv ------> saves self-cap references values to file: refsc.csv\n"));
  _tprintf(_T("  MxtUpdater -deltasc deltasc.csv --> saves self-cap deltas values to file: deltasc.csv\n"));
  _tprintf(_T("  MxtUpdater -refs=n refs.csv ------> saves n snapshots of references values to file: refs_n.csv\n"));
  _tprintf(_T("  MxtUpdater -deltas=n deltas.csv --> saves n snapshots of deltas values to file: deltas_n.csv\n"));
  _tprintf(_T("  MxtUpdater -refsc=n refsc.csv ----> saves n snapshots of self-cap references values to file: refs_n.csv\n"));
  _tprintf(_T("  MxtUpdater -deltasc=n deltasc.csv > saves n snapshots of self-cap deltas values to file: deltas_n.csv\n"));
  _tprintf(_T("  MxtUpdater -mtnn=xx --------------> reads messages from Tnn for xx seconds\n"));
  _tprintf(_T("  MxtUpdater -mt*=xx ---------------> reads messages from any objects for xx seconds\n"));
  _tprintf(_T("  MxtUpdater -mtnn=xx -regw=Tnn[0xmm] 0xn1 -> executes -regw, reads messages from Tnn for xx seconds\n"));
  _tprintf(_T("  MxtUpdater -mt*=xx  -regw=Tnn[0xmm] 0xn1 -> executes -regw, reads messages from any objects for xx seconds\n"));
  _tprintf(_T("  MxtUpdater -regr 0xaddr 0xn ------> reads 0xn bytes from device address 0xaddr and prints them on screen\n"));
  _tprintf(_T("  MxtUpdater -regr=Tnn[0xmm] 0xn ---> reads 0xn bytes from offset 0xmm of object Tnn\n"));
  _tprintf(_T("  MxtUpdater -regw 0xaddr 0xn1 0xn2-> writes 0xn1 0xn2... values starting at device address 0xaddr\n"));
  _tprintf(_T("  MxtUpdater -regw=Tnn[0xmm] 0xn1 0xn2-> writes obj Tnn at offset 0xmm values 0xn1 0xn2...\n\n"));
  _tprintf(_T("where FWFile.enc is the FW binary file and \nConfig.cfg or Config.xcfg are the configuration files.\n"));

  _tprintf(_T("Important:\nTo perform firmware update on any I2C-connected devices, MxtBootBridge.sys\ndriver MUST be always loaded and running.\n\n"));
  _tprintf(_T("Some devices may require smaller buffers for flashing FW.\nYou can try to use '-fi FWFile.enc' if '-f' failed.\n\n"));
  _tprintf(_T("If when flashing a USB-connected device, the process gets stuck,\ntry to use '-fd'.\n\n"));
  _tprintf(_T("When maXTouch device is already in Bootloader mode and MxtUpdater fails\nto communicate with the device, try to run MxtUpdater again.\n"));
  _tprintf(_T("After few unsuccessful attempts, try power-cycling your device.\n\n"));
}

void CloseLog(int iRetCode)
{
  if ((LogMode) && LogFile) {
    LogFile.close();
    if (iRetCode < 0) {
      CopyFile(LogFileName, LogFileFail, FALSE);
    } else {
      CopyFile(LogFileName, LogFileSuccess, FALSE);
    }
    //DeleteFile(LogFileName);	//Why delete the file
  }
}

void CloseLogSvc(int iRetCode)
{
  UNREFERENCED_PARAMETER (iRetCode);
  if ((LogModeSvc) && LogFile) {
    LogFile.close();
  }
}

int ConfigUpdate()
{
  BOOL bRetcode = TRUE;
  int iRetCode = -1; //set as error
  if (DeviceAppFromXML) {
    _stprintf(PrintBuffer, _T("Attempting to update configuration of %s.\n"), DeviceAppNameFromXML);
  } else {
    _stprintf(PrintBuffer, _T("Attempting to update configuration of %s.\n"), DEVICE_NAME[DeviceConnected]);
  }
  _TPRINTFD(PrintBuffer);
  // Disable T70 dynamic configuration
  BackupDevice(0x33);
  Sleep(1000);

#ifdef AUTO_UPDATER_PANASONIC
  if(FindAtmelDevice()) {
    if (!SpecialWrite(18, 4)) {
      _tprintf(_T("Error setting T18\n"));
    }
  }
#endif

#if 0
//  UINT32 checksum = 0; //, nVMChecksum;
//  UINT8 CmdProcReportId;
//  The following should be 1
//  CmdProcReportId = GetReportIdFromType(GEN_COMMANDPROCESSOR_T6);
    // Read config checksum... 
//  Commented out: not reliable the first time after writing config
//    UINT8 msg[16], timeoutCounter;
//    UINT32 rcvdChecksum;
//    for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
//      if (ReadMessage(msg)) {
//        if (msg[0] == CmdProcReportId) {
//          rcvdChecksum = *((UINT32 *)&msg[2]);
//          break;
//        }
//      }
//    }
//    _stprintf(PrintBuffer, _T("NVM configuration checksum is: \t\t\t\t\t0x%06X.\n"), rcvdChecksum);
//    DBGPRINT(PrintBuffer);

//    if ( timeoutCounter == MAX_COUNT ) {
//      _stprintf(PrintBuffer, _T("Cannot verify written configuration:\nError getting configuration checksum.\n"));
//      _TPRINTF(PrintBuffer);
//    }
//    if (VerifyConfigEnabled) {
//      CalculateObjectsCRC(&checksum, &ObjectsInFile);
//      _stprintf(PrintBuffer, _T("CalculateObjectsCRC(&checksum, &ObjectsInFile)  -> checksum: 0x%06X.\n"), checksum);
//      DBGPRINT(PrintBuffer);
//    }
//    CalculateInFWChecksum(&inFWChecksum);
//    _stprintf(PrintBuffer, _T("CalculateChecksum(&inFWChecksum) \t\t-> checksum: 0x%06X.\n"), inFWChecksum);
//    _TPRINTF(PrintBuffer);
#endif

  if (WriteConfigToDevice(CfgFile)) {
    iRetCode = 0; // Success
    Sleep(2000);

    _stprintf(PrintBuffer, _T("\nBacking up configuration and resetting the device\n"));
    _TPRINTFD(PrintBuffer);
    if (BackupAndResetDevice()) {
      Sleep(3000);
      if (ConfigObjectNotFoundError) {
        _stprintf(PrintBuffer, _T("\nConfiguration was updated with potential errors.\nMake sure you are not using a configuration file for an older version of the FW.\n"));
        _TPRINTFD(PrintBuffer);
        if (ErrorCodes) iRetCode = CFG_WRITING_FAILED;
      } else {
        if (!bRetcode) {
          _stprintf(PrintBuffer, _T("\nConfiguration update may not be successful.\n"));
          _TPRINTF(PrintBuffer);
          if (ErrorCodes) iRetCode = CFG_WRITING_FAILED;
          return iRetCode;
        }
#if 0
        else if (VerifyConfigEnabled) {
          Sleep(4000);
          ReadNVMChecksum(&nVMChecksum);
//          _tprintf(_T("Expected Checksum = %x, NVM checksum = %x"), ExpectedChecksum, checksum);
#ifdef AUTO_UPDATER
          checksum = ExpectedChecksum;
#endif
          if (nVMChecksum != checksum) {
            _stprintf(PrintBuffer, _T("\nChecksums do not match:\nconfiguration may not have been written correctly.\n"));
            if (ErrorCodes) iRetCode = CFG_VERIFY_FAILED;
            _TPRINTF(PrintBuffer);
            iRetCode = -1;
            return iRetCode;
          }
          _stprintf(PrintBuffer, _T("\nChecksums match: verify was successful.\n"));
          _TPRINTF(PrintBuffer);
          _stprintf(PrintBuffer, _T("\nConfiguration was updated successfully.\n"));
          _TPRINTF(PrintBuffer);
          iRetCode = 0;
          return iRetCode;
        }
#endif
        else {
          if (VerifyConfigEnabled) {
            // Added for Dan Wax issue with 1664S Toshiba
            Sleep(7000);
            bRetcode = VerifyConfig();
            if (!bRetcode) {
              _stprintf(PrintBuffer, _T("\nChecksums do not match:\nconfiguration may not have been written correctly.\n"));
              if (ErrorCodes) iRetCode = CFG_VERIFY_FAILED;
            } else {
              _stprintf(PrintBuffer, _T("\nChecksums match: verify was successful.\nConfiguration was updated successfully.\n"));
              iRetCode = 0;
            }
            _TPRINTFD(PrintBuffer);
          }
          return iRetCode;
        }
      }
      _stprintf(PrintBuffer, _T("\nCould not backup configuration. Please re-try.\n"));
      _TPRINTF(PrintBuffer);
    } else {
      _stprintf(PrintBuffer, _T("\nFailed to write configuration to the device.\n"));
      _TPRINTF(PrintBuffer);
    }
  } else {
    // Reset the device to eliminate potentially partially stored config
    ResetDevice();
    if (ErrorCodes) iRetCode = CFG_WRITING_FAILED;
  }
  return iRetCode;
}

BOOL ReadConfig()
{
  _stprintf(PrintBuffer, _T("Reading configuration from device...\n"));
  _TPRINTFD(PrintBuffer);
  // Disable dynamic config (needed for T70)
  // and restore settings from NVM
  BackupDevice(0x33);
  Sleep(2000);
  if (ReadConfigFromDevice()) {
    if (bIsXCfg) {
      return(WriteXConfigFile(CfgFile));
    }
    if (bIsRaw) {
      return(WriteRawConfigFile(CfgFile));
    }
    return(WriteConfigFile(CfgFile));
  }
  return FALSE;
}

BOOL VerifyConfig()
{
  BOOL bRetcode;
  UINT32 checksum, inFWChecksum = 0;
  UINT8 CmdProcReportId = 1;
  UINT8 msg[16], timeoutCounter;
  UINT16 address;
  DWORD bytesToWrite = 1;
  UINT8 tempBuffer[8];

//  checksum = ExpectedChecksum;

  // Disable T70 dynamic configuration
  BackupDevice(0x33);
  Sleep(2000);

  // The following should be 1
  CmdProcReportId = GetReportIdFromType(GEN_COMMANDPROCESSOR_T6);
  // Do a report all
  if (!GetObjectAddressAndSize(GEN_COMMANDPROCESSOR_T6, &address)) {
    _stprintf(PrintBuffer, _T("Error on GetReportIdFromType()\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  if(FindAtmelDevice(FIRST_DISCOVERY, TRUE)) {
    tempBuffer[0] = 1;
    bRetcode = WriteToDevice(address+3, tempBuffer, &bytesToWrite);
    if (!bRetcode) {
      _stprintf(PrintBuffer, _T("Write failed.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
  }

  // Read config checksum... 
  for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
    if (ReadMessage(msg)) {
      if (msg[0] == CmdProcReportId) {
        inFWChecksum = *((UINT32 *)&msg[2]);
        break;
      } 
    }
  }

  if ( timeoutCounter == MAX_COUNT ) {
    _stprintf(PrintBuffer, _T("Cannot verify written configuration:\nError getting configuration checksum.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

   CalculateObjectsCRC(&checksum, &ObjectsInFile);
//  _tprintf(_T("checksum = %x, inFWChecksum = %x\n"), checksum, inFWChecksum);
  return (checksum == inFWChecksum);
}


BOOL FindAtmelDevice(int Order, BOOL Always)
{
  BOOL bReturnCode = FALSE;

  if (!Always && DeviceAlreadyIdentified) {
    return TRUE;
  }
  switch (ConnectFWType) {

  case HIDUSB:
    if (FindDiscoverableDevice(Order) == USB_NORMAL) {
      if (ReadMemoryMap(TRUE)) {
        bReturnCode = TRUE;
      } else {
        _stprintf(PrintBuffer, _T("Found a USB-connected device. Error communicating with it. Try power-cycling it.\n"));
        _TPRINTFD(PrintBuffer);
      }
    }
    break;

  case HIDI2C:
    if (FindDiscoverableDevice(Order) == I2C_NORMAL) {
      AtmelReportId = ATMEL_REPORT_ID_I2C;
      ConnectFWType = HIDI2C;
	  if (DevicePID != 0x211D) {			// 2016/06/06. Miracle. HIDI2C BL.
		  if (ReadMemoryMap(TRUE)) {
			  bReturnCode = TRUE;
		  }
	  }
	  else {
		  bReturnCode = TRUE;				// 2016/06/06. Miracle. HIDI2C BL.
	  }
      break;
    }
    if (IsCustomDevicePath && FindHIDI2CDeviceByDevPath(Custom_DevicePath)) {
		AtmelReportId = ATMEL_REPORT_ID_I2C;
      ConnectFWType = HIDI2C;
      if (ReadMemoryMap(TRUE)) {
        bReturnCode = TRUE;
      }
      break;
    }
    while (HIDI2C_DEVICE_STRING[DevicePathId]) {
      if (FindHIDI2CDeviceByDevPath(HIDI2C_DEVICE_STRING[DevicePathId])) {
        AtmelReportId = ATMEL_REPORT_ID_I2C;
        ConnectFWType = HIDI2C;
        if (ReadMemoryMap(TRUE)) {
          bReturnCode = TRUE;
		  break;
        }
      }
      ++DevicePathId;
    }

    break;

  case HIDI2CNOINBOX:
  case I2CSTOCK:
    // Try to init communication with our device
    if (!InitI2CNormal(TRUE)) {
      break;
    }
    if (ReadMemoryMap(TRUE)) {
      bReturnCode = TRUE;
    }
    break;

  default:
    bReturnCode = FALSE;
    break;
  }

  return bReturnCode;
}

INT FindAtmelDeviceBootloaderMode(int Order, BOOL Quiet)
{
  if (ConnectFWType == HIDUSB) {
    if (FindDiscoverableDevice(Order) == USB_BOOTLOADER) {
      return TRUE;
    }
  } else {
    // Try to init communication with our Bootloader device
    if (!InitI2CBootloader(Quiet)) {	//MCG - Modify to be Quiet so no message is printed for secondary discovery
      return -1;	//MCG 2016-08-24 -- Generate different errors
    }
    // Check if in already bootloader mode
    if (BootWorkerWaitForState(MXT_WAITING_BOOTLOAD_CMD, Quiet)) {
      // device is ready in bootloader mode
      return TRUE;	//MCG 2016-08-24 -- Generate different error messages
    }
    // Attempt to reset Bootloader mode
    if (!ResetBootloaderMode()) {
      return FALSE;
    }
    // do some retries; it could have been interrupted waiting for a frame
    for (int i = 0; i < 3; ++i) {
      Sleep(2000*i);
      _stprintf(PrintBuffer, _T("Check if Bootloader mode is ready. Retrying...\n"));
      _TPRINTFD(PrintBuffer);
      if (BootWorkerWaitForState(MXT_WAITING_BOOTLOAD_CMD, TRUE)) {
        return TRUE;
      }
    }
  }
  // Reset attempt failed
  return FALSE;
}

#ifndef AUTO_UPDATER
BOOL LoadXMLFile()
{
  HGLOBAL     res_handle = NULL;
  HRSRC       res;

  // NOTE: providing g_hInstance is important, NULL might not work
  res = FindResource(NULL, MAKEINTRESOURCE(XML_FILE), RT_RCDATA);
  if (!res) {
    return FALSE;
  }
  res_handle = LoadResource(NULL, res);
  if (!res_handle) {
    return FALSE;
  }
  XMLFileRes = (UINT8*)LockResource(res_handle);
  XMLFileSize = SizeofResource(NULL, res);
  return TRUE;
}
#else

BOOL WorkOnResources()
{
  HGLOBAL     res_handle = NULL;
  HRSRC       res;
  TCHAR       buffer[32];
  UINT        value;
  int i;

//#ifdef NEW_OLD_BLDR
//  LoadString(GetModuleHandle(NULL), IDS_FWVERSION2, FWVersion, 1024);
//#else
//  LoadString(GetModuleHandle(NULL), IDS_FWVERSION, FWVersion, 1024);
//#endif

  // NOTE: providing g_hInstance is important, NULL might not work
  LoadString(GetModuleHandle(NULL), IDS_NUM_PAYLOADS, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);

  NumberOfPayloads = (UINT8)value;

  res = FindResource(NULL, MAKEINTRESOURCE(FW_FILE), RT_RCDATA);
  if (!res) {
    return FALSE;
  }
  res_handle = LoadResource(NULL, res);
  if (!res_handle) {
    return FALSE;
  }
  FWFileRes = (UINT8*)LockResource(res_handle);
  FWFileSize = SizeofResource(NULL, res);

  // you can now use the resource data
  for (i = 0; i < NumberOfPayloads; ++i) {
    res = FindResource(NULL, MAKEINTRESOURCE(CFG_FILE+i), RT_RCDATA);
    if (!res) {
      return FALSE;
    }
    res_handle = LoadResource(NULL, res);
    if (!res_handle) {
      return FALSE;
    }
    CfgFileResAll[i] = (UINT8 *)LockResource(res_handle);
    CfgFileSizeAll[i] = SizeofResource(NULL, res);
  }

  for (i = 0; i < NumberOfPayloads; ++i) {
    res = FindResource(NULL, MAKEINTRESOURCE(THQA_FILE+i), RT_RCDATA);

    if (!res) return FALSE;

    res_handle = LoadResource(NULL, res);
    if (!res_handle) {
      return FALSE;
    }
    ThqaFileResAll[i] = (UINT8 *)LockResource(res_handle);
    ThqaFileSizeAll[i] = SizeofResource(NULL, res);
  }

  for (i = 0; i < NumberOfPayloads; ++i) {
    LoadString(GetModuleHandle(NULL), IDS_PID + i, buffer, 12);
    _stscanf(buffer, _T("%x"), &PidAll[i]);
  }

  PidToWrite = PidAll[0];
  ExpectedPid = PidAll[0];

  for (i = 0; i < NumberOfPayloads; ++i) {
    LoadString(GetModuleHandle(NULL), IDS_CHECKSUM + i, buffer, 12);
    _stscanf(buffer, _T("%x"), &ExpectedChecksumAll[i]);
  }

  for (i = 0; i < NumberOfPayloads; ++i) {
    LoadString(GetModuleHandle(NULL), IDS_COMBOVERSION+i, CombinedVersion, 32);
    _stscanf(CombinedVersion, _T("%x"), &ExpectedVersionAll[i]);
  }

  // This is the AutoUpdater custom message
  LoadString(GetModuleHandle(NULL), IDS_STRING101, OpenMsg, 1024);

  LoadString(GetModuleHandle(NULL), IDS_SILENTMODE, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  SilentMode = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_CREATELOG, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  LogMode = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_VERIFYCFG, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  VerifyConfigEnabled = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_CHECKACPOWER, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  CheckACPower = (BOOL)value;
  
  LoadString(GetModuleHandle(NULL), IDS_REBOOT, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  RebootNeeded = value;

  LoadString(GetModuleHandle(NULL), IDS_PAUSE, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  PauseBeforeExit = (BOOL)value;
  
  LoadString(GetModuleHandle(NULL), IDS_FWON, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  IsFWUpdateRequested = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_THQAON, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  WriteThqa = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_TURNON_THQA, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);
  TurnOnThqa = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_CHECK_PID_ON, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);

  CheckDevicePid = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_PID_OR_GPIO, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);

  PidOrGPIOForCfg = (BOOL)value;

  LoadString(GetModuleHandle(NULL), IDS_GPIO_MASK, buffer, 12);
  _stscanf(buffer, _T("%d"), &value);

  GPIOMask = (UINT8)value;

  return TRUE;
}

#if 0
void ShowVersion()
{
  DWORD dwRetVal = 0;
  UINT uRetVal   = 0;
  TCHAR szTempFileName[MAX_PATH];  
  TCHAR lpTempPathBuffer[MAX_PATH];
  HANDLE hTempFile = INVALID_HANDLE_VALUE;
  TCHAR line[LINE_MAX_SIZE];
  UINT size = LINE_MAX_SIZE-1;
  DWORD dwBytesWritten;

  _stprintf(PrintBuffer, PRODUCT_NAME_VERSION, MXTUPDATER_VERSION, MXTUPDATER_DATE);
  _tprintf(PrintBuffer);
  _tprintf(_T("\nEmbedded file FW version(s):\n\n"));
  _tprintf(FWVersion);

  //  Gets the temp path env string (no guarantee it's a valid path).
  dwRetVal = GetTempPath(MAX_PATH,      // length of the buffer
    lpTempPathBuffer);                  // buffer for path 
  if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
    _tprintf(_T("Could not create temp file for configuration: using std output instead...\n"));
  }

  //  Generates a temporary file name. 
  uRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
    TEXT("Cfg"),   // temp file name prefix 
    0,                // create unique name 
    szTempFileName);  // buffer for name 
  if (uRetVal == 0) {
    _tprintf(_T("Could not create temp file for configuration: using std output instead...\n"));
  }
  //  Creates the new file to write to for the upper-case version.
  hTempFile = CreateFile((LPTSTR) szTempFileName, // file name 
    GENERIC_WRITE,        // open for write 
    0,                    // do not share 
    NULL,                 // default security 
    CREATE_ALWAYS,        // overwrite existing
    FILE_ATTRIBUTE_NORMAL,// normal file 
    NULL);                // no template 
  if (hTempFile == INVALID_HANDLE_VALUE) {
    _tprintf(_T("Could not create temp file for configuration: using std output instead...\n\n"));
    hTempFile = GetStdHandle(STD_OUTPUT_HANDLE);
  } else {
    _tprintf(_T("Embedded configuration file has been saved to:\n\n%s\n\n"),  szTempFileName);
  }
  while(GetLine(line, &size)) {
#ifdef _UNICODE
    char buffer2[512];
    int numChar;
    numChar = WideCharToMultiByte(CP_UTF8, NULL, (WCHAR *)line, -1, buffer2, 512, NULL, NULL);
    if (numChar <= 0) {
      _tprintf(_T("Error on writing config file. Exiting...\n\n"));
      CloseHandle(hTempFile);
      return;
    }
    // Add line termination
    buffer2[numChar-1] = 0x0D;
    buffer2[numChar++] = 0x0A;
    if (!WriteFile(hTempFile, buffer2, numChar, &dwBytesWritten, NULL) || numChar != (int)dwBytesWritten) {
      _tprintf(_T("Error on writing config file. Exiting...\n\n"));
    }
#else
    line[size-1] = 0x0D;
    line[size++] = 0x0A;
    if (!WriteFile(hTempFile, line, size, &dwBytesWritten, NULL) || size != (int)dwBytesWritten) {
      _tprintf(_T("Error on writing config file. Exiting...\n\n"));
    }
#endif
  }
  CloseHandle(hTempFile);
}
#endif

#if 0
void ShowVersion()
{
  _stprintf(PrintBuffer, PRODUCT_NAME_VERSION, MXTUPDATER_VERSION, MXTUPDATER_DATE);
  _tprintf(PrintBuffer);
  _tprintf(_T("Combined version: \t%s\n"), CombinedVersion);
  if (WriteThqa) {
    _tprintf(_T("PID: \t\t\t0x%x\n"), PidToWrite);
    _tprintf(_T("Thqa file: \t\t%s\n"), ThqaFileName);
  }
}
#endif

BOOL IsFWVersionSame()
{
  UINT32 fwVersion;
  UINT32 expectedVersion;
  if (!ReadMemoryMap()) return FALSE;
  fwVersion = GetFWVersion();
  if (fwVersion == -1) return FALSE;
  expectedVersion = (ExpectedVersion >> 16) & 0x0000FFFF;

  return (fwVersion == expectedVersion);
}

BOOL IsCfgVersionSame()
{
  UINT32 cfgVersion;
  UINT32 expectedVersion;
  if (!ReadMemoryMap()) return FALSE;
  cfgVersion = GetConfigVersion();
  if (cfgVersion == -1) return FALSE;
  expectedVersion = ExpectedVersion & 0x0000FFFF;

  return (cfgVersion == expectedVersion);
}

BOOL IsCfgChecksumSame()
{
  UINT32 checksum;
  if (!ReadMemoryMap()) return FALSE;
  if (!ReadNVMChecksum(&checksum)) return FALSE;

  return (checksum == ExpectedChecksum);
}

BOOL IsThqaSame()
{
  UINT8 actualThqa[512];

  if (!ReadThqaString(actualThqa)) return FALSE;
  if (memcmp(actualThqa, ThqaFileRes, 0x100)) {
    return FALSE;
  }
  if (ExpectedPid != AllPids[0]) return FALSE; 
  return TRUE;
}

BOOL SpecialWrite(UINT8 ObjectType, UINT8 Value)
{
  UINT16 address = 0;
  DWORD bytesToWrite = 1;
  UINT8 buffer[10];
  ZeroMemory(buffer,10);
  BOOL rCode;

  buffer[0] = Value;

  if (GetObjectAddressAndSize(ObjectType, &address)) {
    for (int i=0; i<10; ++i) {
      rCode = WriteToDevice(address, buffer, &bytesToWrite);
      if (rCode) break;
      Sleep(300);
    }
    return rCode;
  }
  return FALSE;
}
#endif

//
// This function return 0 if unsuccessful; else the sensor to select Config/PID/THQA
//
BOOL ReadGPIOPins(UINT8 *GPIOPins)
{
  UINT objType = 19;
  UINT8 objSize = 0;
  UINT16 address = 0;
  UINT offset = 0;
  UINT typeMsg = 19;
  UINT timeoutMsg = 3; // 3 seconds
  DWORD bytesToWrite = 0;
  UINT8 tempBuffer[64];
  SYSTEMTIME lt;
  UINT startTime, endTime, currentTime;
  UINT reportId = 0;
  UINT8 msgBuffer[16];
  BOOL bReturncode;

  if (!GetObjectAddressAndSize((UINT8)objType, &address, &objSize)) {
    return FALSE;
  }
  offset = 1; // Write all 0s to the MASK, just in case
  bytesToWrite = 1;
  tempBuffer[0] = 0;

  bReturncode = WriteToDevice(address+(UINT16)offset, tempBuffer, &bytesToWrite);
  if (!bReturncode) return FALSE;

  offset = 0; // Write 7 to query GPIO
  bytesToWrite = 1;
  tempBuffer[0] = 7;

  bReturncode = WriteToDevice(address+(UINT16)offset, tempBuffer, &bytesToWrite);
  if (!bReturncode) return FALSE;

  GetLocalTime(&lt);
  startTime = lt.wMinute*60+lt.wSecond;
  endTime = startTime + timeoutMsg;
  // check if all msg are requested
  reportId = GetReportIdFromType((UINT8)typeMsg);

  for (;;) {
    ReadMessage(msgBuffer);
    if (msgBuffer[0] == 0xFF) {
      GetLocalTime(&lt);
      currentTime =  lt.wMinute*60+lt.wSecond;
      if (currentTime < endTime) break;
    } else {
      if (msgBuffer[0] == reportId) {
        *GPIOPins = msgBuffer[1];
        return TRUE;
      }
    }
  }
  return FALSE;
}
