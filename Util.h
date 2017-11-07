/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file Util.h
@brief Maxtouch Util header file 
@details Utility functions header file
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/

#pragma warning (disable: 4995)

#include <basetyps.h>
#include <stdlib.h>
#include <wtypes.h>
#include <tchar.h>
#include <string.h>
extern "C" {
#include <hidsdi.h>
}
#include "hid.h"
#include <Devguid.h>
#include <strsafe.h>
#include <fstream>
#include <vector>
#include <atlstr.h>

// Needed to support XML parsing
#include "pugixml.hpp"

#define MAX_NAME_SIZE 64
#define MXT_INVALID_ADDRESS   0xFFFF

#define MAX_COUNT  90  // RB : added 2012/12/17, to be fine-tuned to about 5-10 seconds in Golden references
                       // When device does not send msg, mxtupdater hangs in endless loop; this way, exit with error

#define LINE_MAX_SIZE 1024  // used when parsing raw file

#define ATMEL_REPORT_ID_I2CBridge 0x00    // Add by Miracle.
#define ATMEL_REPORT_ID_USB       0x01
#define ATMEL_REPORT_ID_I2C       0x06
#define ATMEL_REPORT_ID2_I2C      0x07

#define NOT_FOUND      -1
#define USB_NORMAL      0
#define USB_BOOTLOADER  1
#define I2C_NORMAL      2
#define I2C_BOOTLOADER  3

#define FIRST_DISCOVERY 0
#define SECOND_DISCOVERY 1

// Object types

#define INVALID_OBJECT_TYPE                      -1
#define RESERVED_T0                               0
#define RESERVED_T1                               1
#define DEBUG_DELTAS_T2                           2
#define DEBUG_REFERENCES_T3                       3
#define DEBUG_SIGNALS_T4                          4
#define GEN_MESSAGEPROCESSOR_T5                   5
#define GEN_COMMANDPROCESSOR_T6                   6
#define GEN_POWERCONFIG_T7                        7
#define GEN_ACQUISITIONCONFIG_T8                  8
#define TOUCH_MULTITOUCHSCREEN_T9                 9
#define TOUCH_SINGLETOUCHSCREEN_T10               10
#define TOUCH_XSLIDER_T11                         11
#define TOUCH_YSLIDER_T12                         12
#define TOUCH_XWHEEL_T13                          13
#define TOUCH_YWHEEL_T14                          14
#define TOUCH_KEYARRAY_T15                        15
#define PROCG_SIGNALFILTER_T16                    16
#define PROCI_LINEARIZATIONTABLE_T17              17
#define SPT_COMMSCONFIG_T18                       18
#define SPT_GPIOPWM_T19                           19
#define PROCI_GRIPFACESUPPRESSION_T20             20
#define RESERVED_T21                              21
#define PROCG_NOISESUPPRESSION_T22                22
#define TOUCH_PROXIMITY_T23                       23
#define PROCI_ONETOUCHGESTUREPROCESSOR_T24        24
#define SPT_SELFTEST_T25                          25
#define DEBUG_CTERANGE_T26                        26
#define PROCI_TWOTOUCHGESTUREPROCESSOR_T27        27
#define SPT_CTECONFIG_T28                         28
#define SPT_GPI_T29                               29
#define SPT_GATE_T30                              30
#define TOUCH_KEYSET_T31                          31
#define TOUCH_XSLIDERSET_T32                      32
#define RESERVED_T33                              33
#define GEN_MESSAGEBLOCK_T34                      34
#define SPT_PROTOTYPE_T35                         35
#define RESERVED_T36                              36
#define DEBUG_DIAGNOSTIC_T37                      37
#define SPT_USERDATA_T38                          38
#define GEN_REMOTEACQUISITION_T39                 39
#define PROCI_GRIPSUPPRESSION_T40                 40
#define PROCI_PALMSUPPRESSION_T41                 41
#define PROCI_TOUCHSUPPRESSION_T42                42
#define SPT_DIGITIZER_T43                         43
#define SPT_MESSAGECOUNT_T44                      44
#define SPARE_T45                                 45
#define SPT_CTECONFIG_T46                         46
#define PROCI_STYLUS_T47                          47
#define PROCG_NOISESUPPRESSION_T48                48
#define SPARE_T49                                 49
#define SPARE_T50                                 50
#define SPARE_T51                                 51
#define TOUCH_PROXKEY_T52                         52
#define GEN_DATASOURCE_T53                        53
#define PROCG_NOISESUPPRESSION_T54                54
#define PROCI_ADAPTIVETHRESHOLD_T55               55
#define PROCI_SHIELDLESS_T56                      56
#define PROCI_EXTRATOUCHSCREENDATA_T57            57
#define SPT_PRODUCTIONCUSTOM_T60                  60
#define SPT_TIMER_T61                             61
#define PROCG_NOISESUPPRESSION_T62                62
#define PROCI_ACTIVESTYLUS_T63                    63
#define PROCI_LENSBENDING_T65                     65
#define SPT_GOLDENREFERENCES_T66                  66
#define PROCI_CUSTOMGESTUREPROCESSOR_T67          67
#define SPT_SERIALDATACOMMAND_T68                 68
#define PROCI_PALMGESTUREPROCESSOR_T69            69
#define SPT_DYNAMICCONFIGURATIONCONTROLLER_T70    70
#define SPT_DYNAMICCONFIGURATIONCONTAINER_T71     71
#define PROCG_NOISESUPPRESSION_T72                72
#define PROCI_ZONEINDICATION_T73                  73
#define MOTION_MOTIONSENSING_T75                  75
#define PROCI_MOTIONGESTURES_T76                  76
#define SPT_CTESCANCONFIG_T77                     77
#define PROCI_GLOVEDETECTION_T78                  78
#define SPT_TOUCHEVENTTRIGGER_T79                 79
#define PROCI_RETRANSMISSIONCOMPENSATION_T80      80
#define PROCI_UNLOCKGESTURE_T81                   81
#define SPT_NOISESUPEXTENSION_T82                 82
#define ENVIRO_LIGHTSENSING_T83                   83
#define PROCI_GESTUREPROCESSOR_T84                84
#define PEN_ACTIVESTYLUSPOWER_T85                 85
#define PROCG_NOISESUPACTIVESTYLUS_T86            86
#define PEN_ACTIVESTYLUSDATA_T87                  87
#define PEN_ACTIVESTYLUSRECEIVE_T88               88
#define PEN_ACTIVESTYLUSTRANSMIT_T89              89
#define PEN_ACTIVESTYLUSWINDOW_T90                90
#define DEBUG_CUSTOMDATACONFIG_T91                91
#define PROCI_SYMBOLGESTUREPROCESSOR_T92          92
#define PROCI_TOUCHSEQUENCELOGGER_T93             93
#define SPT_PTCCONFIG_T95                         95
#define SPT_PTCTUNINGPARAMS_T96                   96
#define TOUCH_PTCKEYS_T97                         97
#define PROCG_PTCNOISESUPPRESSION_T98             98
#define TOUCH_MULTITOUCHSCREEN_T100              100
#define SPT_TOUCHSCREENHOVER_T101                101
#define SPT_SELFCAPHOVERCTECONFIG_T102           102
#define PROCI_SCHNOISESUPPRESSION_T103           103
#define SPT_AUXTOUCHCONFIG_T104                  104
#define SPT_DRIVENPLATEHOVERCONFIG_T105          105
#define SPT_ACTIVESTYLUSMMBCONFIG_T106           106
#define PROCI_ACTIVESTYLUS_T107                  107
#define PROCG_NOISESUPSELFCAP_T108               108
#define SPT_SELFCAPGLOBALCONFIG_T109             109
#define SPT_SELFCAPTUNINGPARAMS_T110             110
#define SPT_SELFCAPCONFIG_T111                   111
#define PROCI_SELFCAPGRIPSUPPRESSION_T112        112
#define SPT_SELFCAPMEASURECONFIG_T113            113
#define SPT_ACTIVESTYLUSMEASCONFIG_T114          114
#define PROCI_SYMBOLGESTURE_T115                 115
#define SPT_SYMBOLGESTURECONFIG_T116             116
#define SPT_DATACONTAINER_T117                   117
#define SPT_DATACONTAINERCTRL_T118               118
#define PROCI_KEYSTATEPROCESSOR_T119             119
#define GEN_SLAVEPOWERCONFIG_T120                120

// moved here from Util.cpp
#define NUM_BOOTLOADER_IDS  153  // one of the versions is just to deal with the unknown case
//#define NUM_I2C_ADDRESSES   13u   // 7u // Modified by Miracle.
#define NUM_APPLICATIONS    11   // one of the apps is for the unknown id

#define BOOTLOADER_STATE_NOT_FOUND                0
#define BOOTLOADER_STATE_NOT_FOUND_SEARCH_ADDR    1
#define BOOTLOADER_STATE_FOUND_CHECK_VER          4
#define BOOTLOADER_STATE_APPCRC_FAIL              5
#define BOOTLOADER_STATE_WAIT_UNLOCK_CMD          6
#define BOOTLOADER_STATE_READ_BEFORE_WAIT_FRAME   7
#define BOOTLOADER_STATE_WAIT_FRAME               8
#define BOOTLOADER_STATE_SENDING_FRAME_DATA       9
#define BOOTLOADER_STATE_WAIT_CRC_CHECK           10
#define BOOTLOADER_STATE_WAIT_CRC_RESULT          11
#define BOOTLOADER_STATE_WAIT_CHANGE_LINE         12

// How many bytes the id string is?
#define ID_STRING_LENGTH                          8
// How many bytes in header in total (including id string)?
#define VERSION_INFO_BYTES                        22

#define MAX_FRAMES     4096
#define MAX_CHARS      (500000)

// Used for Golden references
#define T66_CONFIG_CTRL_ENABLE               0x01u
#define T66_CONFIG_CTRL_RPTEN                0x02u
#define T66_CONFIG_CTRL_FCALCMD              0x0Cu

#define T66_CONFIG_CTRL_FCALCMD_NONE         0x00u
#define T66_CONFIG_CTRL_FCALCMD_PRIME        0x04u
#define T66_CONFIG_CTRL_FCALCMD_GENERATE     0x08u
#define T66_CONFIG_CTRL_FCALCMD_STORE        0x0Cu

#define T66_MSG_STATUS_BADSTOREDDATA         0x01u
#define T66_MSG_STATUS_FCALSTATE             0x06u
#define T66_MSG_STATUS_FCALSEQERR            0x08u
#define T66_MSG_STATUS_FCALSEQTO             0x10u
#define T66_MSG_STATUS_FCALSEQDONE           0x20u
#define T66_MSG_STATUS_FCALPASS              0x40u
#define T66_MSG_STATUS_FCALFAIL              0x80u

#define T66_MSG_STATUS_FCALSTATE_IDLE        0x00u
#define T66_MSG_STATUS_FCALSTATE_PRIMED      0x02u
#define T66_MSG_STATUS_FCALSTATE_GENERATED   0x04u

// Object T68
#define T68_CMD_START       0x01
#define T68_CMD_CONTINUE    0x02
#define T68_CMD_END         0x03
#define DATA_TYPE_LOW       0x01
#define DATA_TYPE_HIGH      0x00

// Object t37 - THQA
#define T37_THQA            0xF9
#define T37_THQA_3432       0xFA
#define T37_PAGE_UP         0x01
#define T37_PAGE_DOWN       0x02
#define T37_DELTAS_MODE     0x10
#define T37_REFS_MODE       0x11
#define T37_DEVID_MODE      0x80
#define T37_SC_DELTAS_MODE  0xF7
#define T37_SC_REFS_MODE    0xF8
#define T37_SERIAL_NUM		0x82

// MxtUpdater version...
#define MXTUPDATER_VERSION  _T("3.55")
#define MXTUPDATER_DATE     _T("2017-27-01")

// Error codes
#define SUCCESS                  0
#define NO_DEVICE_FOUND         -2
#define FOUND_DEVICE_BOOTLOADER -3
#define CFG_READING_FAILED      -4    // From the device, -rx
#define CFG_WRITING_FAILED      -5    // To the device, -wx
#define CFG_VERIFY_FAILED       -6
#define FW_UPDATE_FAILED_FRAMES -7    // timing related (try using -fd=nn or -fs=nn)
#define FW_UPDATE_FAILED        -8
#define FW_FILE_OPEN_ERROR      -9
#define ERROR_OPENING_LOGFILE   -10

#ifdef AUTO_UPDATER
#define MAX_PAYLOADS              6
#endif

using namespace std;

class OPRegister{
public:
  OPRegister(TCHAR* regName, UINT regOffset, UINT regSize = 1, BOOL regIsSigned = FALSE){
    _tcscpy_s(name, MAX_NAME_SIZE, regName);
    offset = regOffset;
    value = 0;
    size = regSize;
    isSigned = regIsSigned;
  }
  TCHAR name[MAX_NAME_SIZE];
  UINT offset;
  UINT value;
  BOOL isSigned;
  UINT size;
};

class OPObject{
public:
  OPObject(TCHAR* objectName, UINT objectType, UINT objectInstance = 0){
    _tcscpy_s(name, MAX_NAME_SIZE, objectName);
    type = objectType;
    start_address = MXT_INVALID_ADDRESS;
    instance = objectInstance;
    objectSize = 0;
  }
  TCHAR name[MAX_NAME_SIZE];
  UINT type;
  UINT instance;
  UINT objectSize;
  vector<OPRegister*> registers;
  UINT16 start_address;
};

struct ObjectTableElement
{
  UINT8 Type;
  UINT8 LSB;
  UINT8 MSB;
  UINT8 Size;
  UINT8 Instances;
  UINT8 NumberOfReportIDs;
};

struct MemoryMap
{
  UINT8 FamilyID;
  UINT8 VariantID;
  UINT8 Version;
  UINT8 Build;
  UINT8 MatrixSizeX;
  UINT8 MatrixSizeY;
  UINT8 NumberOfObjects;
  struct ObjectTableElement Element1;
};

struct UsbHidDevice
{
  HANDLE   HidDevice;
  USHORT   InputReportByteLength;
  USHORT   OutputReportByteLength;
};

struct ObjectNameType
{
  TCHAR ObjectName[MAX_NAME_SIZE];
  UINT8 ObjectType; 
};

typedef struct _MAXTOUCHDEVICES
{
  UINT DeviceIndex;
  HID_DEVICE *DeviceData;
  UINT DeviceType;
  UINT ConnectFWType;
  UINT ReportId;
} MAXTOUCHDEVICES, PMAXTOUCHDEVICES;

BOOL ReadFromDevice(UINT16 Address, UINT8 *Buffer, DWORD *ByteRead);
BOOL WriteToDevice(UINT16 Address, UINT8 *Buffer, DWORD *BytesToWrite);
BOOL PrintChipInfo();
BOOL ReadMemoryMap(BOOL ReadAlways = FALSE);
void IdentifyDevice();

  // Added by Miracle. 2014/02/10.
BOOL ReadMemoryMap_BridgeBoard();
BOOL USBCMD(INT wlen, UINT8 *wbuf, INT rlen, UINT8 *rbuf);
BOOL I2CBridge_ReadWrite(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf);
BOOL I2CBridge_I2C_ReadWrite(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf);
BOOL BridgeBoard_HIDI2C_ReadData(UINT8 *Msg);
BOOL I2CBridge_ReadWriteNonStop(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf);
UINT8 I2CBridge_FindAddr();
BOOL I2CBridge_SetAddr(UINT8 addr);

BOOL ZeroConfigMemory();
void CleanSavedDeviceInfo();
BOOL GetObjectAddressAndSize(UINT8 Type, UINT16 *Address, UINT8 *Size = NULL);
BOOL GetObjectIndex(UINT8 Type, UINT8 *Index);
UINT8 GetObjectSize(UINT8 Type);
BOOL SendCmdToDevice(UINT8 CmdType, UINT8 Code);
BOOL SendCmdToDeviceWithAck(UINT8 CmdType, UINT8 Code);
BOOL BackupAndResetDevice();
BOOL BackupDevice(UINT8 Code);
BOOL ResetDevice();
BOOL CalibrateDevice();
BOOL SetmXTDeviceInBootloadMode();
void FindBootloader();
BOOL IsObjectConfigurable(UINT Index);
BOOL ReadConfigFromDevice();
BOOL FillValueInObject(UINT Index, UINT Instance);
BOOL WriteConfigFile(TCHAR *FilePath);
BOOL WriteXConfigFile(TCHAR *FilePath);
BOOL WriteRawConfigFile(TCHAR *FilePath, BOOL IsFromDevice = TRUE);
BOOL WriteConfigToDevice(TCHAR *FilePath);
BOOL ParseReadConfigFile(TCHAR *FilePath);
BOOL ParseReadConfigXFile(TCHAR *FilePath);
BOOL ParseReadConfigXFileOnly(TCHAR *FilePath);
BOOL ParseReadConfigRawFile(TCHAR *FilePath);
BOOL GetLine(TCHAR *Line, UINT *Size);
BOOL WriteConfigToDeviceUSB(TCHAR *FilePath);
BOOL WriteConfigToDeviceI2C(TCHAR *FilePath);

//BOOL VerifyConfigWritten();
//BOOL VerifyConfig();

int WriteData(int B0, int B1, int B2, int B3, int B4, int B5, int B6, int B7);
BOOL ProgramChip();
void HIDDeviceData(UINT8 ReportID, UINT8 *Data, UINT Size);
void DisplayEnumeratedDevices();
void DisplayMaxTouchControlDevices();
void DisplayMaXTouchPIDs();
void __cdecl ReadDevice (void *dummy);

UINT32 CRC_soft24(UINT32 crc, UINT8 byte1, UINT8 byte2);
UINT32 CalculateCRC(UINT8 *PData, UINT Size, UINT32 Crc = 0);
BOOL CalculateInFWChecksum(UINT32 *Checksum);
BOOL ReadNVMChecksum(UINT32 *Checksum);
BOOL CalculateObjectsCRC(UINT32 * Checksum, vector<OPObject*> *PObiects);
BOOL DisplayChecksum(BOOL InMemory = FALSE);

OPObject* CreateAndInitObject(int Type, int Instance, UINT16 Address = 0, UINT8 Size = 0);
BOOL GetTempFile(TCHAR *FilePath);
int FindDiscoverableDevice(int Order = FIRST_DISCOVERY);
BOOL FindHidDevices(int Order = FIRST_DISCOVERY);
VOID FreeMemHidDevices(int Order = FIRST_DISCOVERY);
BOOL SavePIDBridgeDevices(int Order);
BOOL FindHIDI2CDeviceByDevPath(const TCHAR *AtmelString);
BOOL IsI2CDeviceBootloaderMode(BOOL Quiet = FALSE);

//BOOL EnableT5AutoReturn(BOOL Enable = TRUE);

int IdentifyConnectFWType();
BOOL ValidateDevice();
BOOL StoreGoldenRefs();
enum GRSTATUS GoldenRefsStatus();
void DisplayGRStatus();
UINT8 GetReportIdFromType(UINT8 Type);
BOOL WriteThqaString(fstream *File = NULL, UINT Pid = 0, BOOL DummyThqa = FALSE);
BOOL ActivatePidThqa(BOOL Enable);
BOOL WriteT68(fstream *File);
BOOL WriteSerialNum(fstream *File, BOOL ClearMem);
void ReplaceChar(char *Line);
BOOL SetDiagnosticField(UINT8 Value);
BOOL ReadDiagnosticData(UINT8 *PData);
BOOL ReadDeltasRefs(fstream *File, UINT8 Mode, BOOL Page = FALSE);
BOOL ReadSCDeltasRefs2952(fstream *File, UINT8 Mode);
BOOL ReadSCDeltasRefs640(fstream *File, UINT8 Mode);
BOOL ReadPageDeltasRefs(int PreviousPage, UINT PageRequested, UINT8 *PData);
/*
////////////
BOOL ReadT37(fstream *File, UINT TotalValues, UINT8 Mode);
////////////
*/
BOOL GetDeviceDebugParams(UINT8 *T37Size, UINT8 *XLines, UINT8 *YLines);
int _tprintflog(TCHAR *Buffer);
BOOL WriteLogFile(TCHAR *Buffer);
BOOL RegReadFromAddress(UINT16 Address, UINT16 NumBytes);
BOOL ReadMessage(UINT8 *Msg);
BOOL FW2952TWorkaround();
BOOL ActivateT7ForThqa();
int GetConfigVersion();
int GetCfgInternalVersion();
int GetFWVersion();
BOOL GetT60String(MAXTOUCHDEVICES *MxtDevice, char *CustomStringT60);
int GetIndexFromCustomString(char *StringToFind);
void NormalizeCustomString(char *InputString);

int ReadDeviceUniqueID();

UINT DisableDevice(TCHAR* DeviceName, BOOL Disable);
BOOL FindString(const TCHAR *s, const TCHAR *t, int size);

void DecodeXml(UINT v[2]);
BOOL Decrypt(string *ciphertext, string *plaintext);
string *ReadDevicesDefinitionFile(const TCHAR *DevicesInfo);
INT GetDeviceAppName(UINT FamilyId, UINT VariantId, string &Name);
INT GetDeviceBtlName(UINT BootloaderId, string &Name);

// Added by Miracle
BOOL ShowActiveArea();
BOOL MessageMode();

#ifdef AUTO_UPDATER
BOOL GetPIDAndVersion();
BOOL ReadThqaString(UINT8 *Thqa);
#else
BOOL ReadThqaString(fstream *File);
#endif
BOOL ReadSerialNumString(fstream *File);

//BOOL AddDynamicObjects();
//int CompareDynamicObjects(UINT Index);
//BOOL CreateDynamicObjects();

//HIDI2C stuff
BOOL InitI2CNormal(BOOL Quiet = FALSE);
BOOL InitI2CBootloader(BOOL Quiet = FALSE);
int ReadWithRetryFromMxt(HANDLE File, UINT16 Address, UINT16 Numbytes, UINT8 *Buffer);
int WriteWithRetryToMxt(HANDLE File, UINT16 Address, UINT16 Numbytes, UINT8 *Buffer);
BOOL ReadHIDDescriptorWithRetry(HANDLE file, int Retries);

BOOL FWUpgrade_HIDBootloader();		// 2016/06/03. Miracle. HID BL support.

#ifdef LENOVO_COLTRANE
BOOL Is1664();
#endif

  // Added by Miracle
// Define HID header.
enum Enum_HID_HEADER {
  HIDDSC_Length_LSB = 0,
  HIDDSC_Length_MSB,
  HIDDSC_bcdVersion_LSB,
  HIDDSC_bcdVersion_MSB,
  HIDDSC_ReportLength_LSB,
  HIDDSC_ReportLength_MSB,
  HIDDSC_ReportRegister_LSB,
  HIDDSC_ReportRegister_MSB,
  HIDDSC_InputRegister_LSB,
  HIDDSC_InputRegister_MSB,
  HIDDSC_InputLength_LSB,     // 10
  HIDDSC_InputLength_MSB,
  HIDDSC_OutputRegister_LSB,
  HIDDSC_OutputRegister_MSB,
  HIDDSC_OutputLength_LSB,
  HIDDSC_OutputLength_MSB,    // 15
  HIDDSC_CommandRegister_LSB,
  HIDDSC_CommandRegister_MSB,
  HIDDSC_DataRegister_LSB,
  HIDDSC_DataRegister_MSB,
  HIDDSC_Vendor_LSB,        // 20
  HIDDSC_Vendor_MSB,
  HIDDSC_Product_LSB,
  HIDDSC_Product_MSB,
  HIDDSC_Version_LSB,
  HIDDSC_Reserve_1,
  HIDDSC_Reserve_2,
  HIDDSC_Reserve_3,
  HIDDSC_Reserve_4,
  HIDDSC_MaxItems
};

// Miracle. 2014/05/28. Support HIDI2C over bridge board touch event report.
void Read_TouchEvent();   
BOOL ShowMessage(int typeMsg, int tm);

