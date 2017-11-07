/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file hid.h
@brief hid user mode clien header file
@details This module contains the declarations and definitions for use with the hid user mode client sample driver.
		 Environment:Kernel & user mode
@Copyright (c) Microsoft 1998, All Rights Reserved
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/


#ifndef HID_H
#define HID_H

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201) // nameless struct/union

#include "hidsdi.h"
#include "setupapi.h"



typedef struct _SP_FNCLASS_DEVICE_DATA {
   DWORD cbSize;
   GUID  FunctionClassGuid;
   TCHAR DevicePath [ANYSIZE_ARRAY];
} SP_FNCLASS_DEVICE_DATA, *PSP_FNCLASS_DEVICE_DATA;

BOOLEAN
SetupDiGetFunctionClassDeviceInfo (
   IN    HDEVINFO                DeviceInfoSet,
   IN    PSP_DEVINFO_DATA        DeviceInfoData,
   OUT   PSP_FNCLASS_DEVICE_DATA FunctionClassDeviceData,
   IN    DWORD                   FunctionClassDeviceDataSize,
   OUT   PDWORD                  RequiredSize
   );

#ifndef ASSERT 
#define ASSERT(x)
#endif

//
// A structure to hold the steady state data received from the hid device.
// Each time a read packet is received we fill in this structure.
// Each time we wish to write to a hid device we fill in this structure.
// This structure is here only for convenience.  Most real applications will
// have a more efficient way of moving the hid data to the read, write, and
// feature routines.
//
typedef struct _HID_DATA {
   BOOLEAN     IsButtonData;
   UCHAR       Reserved;
   USAGE       UsagePage;   // The usage page for which we are looking.
   ULONG       Status;      // The last status returned from the accessor function
                            // when updating this field.
   ULONG       ReportID;    // ReportID for this given data structure
   BOOLEAN     IsDataSet;   // Variable to track whether a given data structure
                            //  has already been added to a report structure

   union {
      struct {
         ULONG       UsageMin;       // Variables to track the usage minimum and max
         ULONG       UsageMax;       // If equal, then only a single usage
         ULONG       MaxUsageLength; // Usages buffer length.
         PUSAGE      Usages;         // list of usages (buttons ``down'' on the device.

      } ButtonData;
      struct {
         USAGE       Usage; // The usage describing this value;
         USHORT      Reserved;

         ULONG       Value;
         LONG        ScaledValue;
      } ValueData;
   };
} HID_DATA, *PHID_DATA;

typedef struct _HID_DEVICE {   
    TCHAR *              DevicePath;
    HANDLE               HidDevice; // A file handle to the hid device.
    BOOL                 OpenedForRead;
    BOOL                 OpenedForWrite;
    BOOL                 OpenedOverlapped;
    BOOL                 OpenedExclusive;
    
    PHIDP_PREPARSED_DATA Ppd; // The opaque parser info describing this device
    HIDP_CAPS            Caps; // The Capabilities of this hid device.
    HIDD_ATTRIBUTES      Attributes;

    TCHAR *               InputReportBuffer;
    __field_ecount(InputDataLength) 
    PHID_DATA            InputData; // array of hid data structures
    ULONG                InputDataLength; // Num elements in this array.
    PHIDP_BUTTON_CAPS    InputButtonCaps;
    PHIDP_VALUE_CAPS     InputValueCaps;

    TCHAR *               OutputReportBuffer;
    __field_ecount(OutputDataLength) 
    PHID_DATA            OutputData;
    ULONG                OutputDataLength;
    PHIDP_BUTTON_CAPS    OutputButtonCaps;
    PHIDP_VALUE_CAPS     OutputValueCaps;

    TCHAR *               FeatureReportBuffer;
    __field_ecount(FeatureDataLength) PHID_DATA            FeatureData;
    ULONG                FeatureDataLength;
    PHIDP_BUTTON_CAPS    FeatureButtonCaps;
    PHIDP_VALUE_CAPS     FeatureValueCaps;
} HID_DEVICE, *PHID_DEVICE;


BOOLEAN
OpenHidDevice (
    __in     LPTSTR         DevicePath,
    __in     BOOL           HasReadAccess,
    __in     BOOL           HasWriteAccess,
    __in     BOOL           IsOverlapped,
    __in     BOOL           IsExclusive,
    __inout  PHID_DEVICE    HidDevice
);

BOOLEAN
FindKnownHidDevices (
   OUT PHID_DEVICE * HidDevices, // A array of struct _HID_DEVICE
   OUT PULONG        NumberDevices // the length of this array.
   );

BOOLEAN
FillDeviceInfo(
    IN  PHID_DEVICE HidDevice
);

VOID
CloseHidDevices (
   OUT PHID_DEVICE   HidDevices, // A array of struct _HID_DEVICE
   OUT ULONG         NumberDevices // the length of this array.
   );

VOID
CloseHidDevice (
    IN PHID_DEVICE   HidDevice
    );


BOOLEAN
Read (
   PHID_DEVICE    HidDevice
   );

BOOLEAN
ReadOverlapped (
    PHID_DEVICE     HidDevice,
    HANDLE          CompletionEvent
   );
   
BOOLEAN
Write (
   PHID_DEVICE    HidDevice
   );

BOOLEAN
UnpackReport (
   __in_bcount(ReportBufferLength)TCHAR* ReportBuffer,
   IN       USHORT               ReportBufferLength,
   IN       HIDP_REPORT_TYPE     ReportType,
   IN OUT   PHID_DATA            Data,
   IN       ULONG                DataLength,
   IN       PHIDP_PREPARSED_DATA Ppd
   );

BOOLEAN
PackReport (
   __out_bcount(ReportBufferLength)TCHAR * ReportBuffer,
   IN       USHORT               ReportBufferLength,
   IN       HIDP_REPORT_TYPE     ReportType,
   IN       PHID_DATA            Data,
   IN       ULONG                DataLength,
   IN       PHIDP_PREPARSED_DATA Ppd
   );

BOOLEAN
SetFeature (
   PHID_DEVICE    HidDevice
   );

BOOLEAN
GetFeature (
   PHID_DEVICE    HidDevice
   );

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif

#endif

