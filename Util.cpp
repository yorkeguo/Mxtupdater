/*======================================================================================================================
Project: MaxTouch Updater Tool
-------------------------------------------------------------------------------------------------------------------*//**
@file Util.cpp
@brief Maxtouch Util cpp file
@details
@copyright Copyright 2017 Solomon Systech Limited. All rights reserved.
*//*--------------------------------------------------------------------------------------------------------------------
This file contains trade secrets of Solomon Systech Limited. No part may be reproduced or transmitted in any form by
any means or for any purpose without the express written permission of Solomon Systech Limited.
Solomon Systech Limited is not liable for any errors or damages arising from using the source code.
======================================================================================================================*/

#include "Util.h"
#include "HIDHelper.h"
#include "i2cBridge.h"
#include "BootWorker.h"

UINT8 i2c_addresses[] = {0x24u, 0x25u, 0x26u, 0x27u, 0x34u, 0x35u, 0x4Au, 0x4Bu, 0x4Cu, 0x4Du, 0x5Au, 0x5Bu, 0x5Fu};
int i2c_address_index;

const TCHAR *version_decoder_name[NUM_BOOTLOADER_IDS] =  {
  _T("UNKNOWN BOOTLOADER"),  /* covers code zero and anything not in list */
  _T("Obsolete ID - Bootloader (ID:1)"),
  _T("Obsolete ID - Bootloader (ID:2)"),
  _T("Custom Project (M324P) - Bootloader ID:3"),
  _T("Custom Project (XM64A1) - Bootloader ID:4"),
  _T("Obsolete ID - Bootloader (ID:5)"),
  _T("Obsolete ID - Bootloader (ID:6)"),
  _T("maXTouch mXT224 (pre-production) - Bootloader ID:7"),
  _T("Obsolete ID - Bootloader (ID:8)"),
  _T("maXTouch mXT224 (pre-production) - Bootloader ID:9"),
  _T("maXTouch mXT224 - Bootloader ID:10"),
  _T("maXTouch mXT165 - Bootloader ID:11"),
  _T("maXTouch mXT140 (pre-production) - Bootloader ID:12"),
  _T("maXTouch mXT140 - Bootloader ID:13"),
  _T("maXTouch mXT25C09 - Bootloader ID:14"),
  _T("Custom Project (XM32T0) - Bootloader ID:15"),
  _T("Custom Project (XM32T0) - Bootloader ID:16"),
  _T("Custom Project (XM32T0) - Bootloader ID:17"),
  _T("Custom Project (XM32T0) - Bootloader ID:18"),
  _T("Custom Project (XM32T0) - Bootloader ID:19"),
  _T("Custom Project (XM32T0) - Bootloader ID:20"),
  _T("UNKNOWN BOOTLOADER"), /* Spare ID21 */
  _T("maXTouch mXT540S - Bootloader ID:22"),
  _T("UNKNOWN BOOTLOADER"), /* Spare ID23 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID24 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID25 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID26 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID27 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID28 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID29 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID30 */
  _T("UNKNOWN BOOTLOADER"), /* Spare ID31 */
  _T("UNKNOWN BOOTLOADER"), /* Base ID for Extended mode ID's */
  _T("maXTouch mXT1386 - Bootloader ID:1"),
  _T("maXTouch mXT616 - Bootloader ID:2"),
  _T("maXTouch mXT1386 - Bootloader ID:3"),
  _T("maXTouch mXT768e (pre-production) - Bootloader ID:4"),
  _T("maXTouch mXT540e (pre-production) - Bootloader ID:5"),
  _T("maXTouch mXT224e - Bootloader ID:6"),
  _T("maXTouch mXT143e - Bootloader ID:7"),
  _T("maXTouch mXT112e - Bootloader ID:8"),
  _T("maXTouch mXT768e - Bootloader ID:9"),
  _T("maXTouch mXT540e - Bootloader ID:10"),
  _T("maXTouch mXT1716EEGV - Bootloader ID:11"),
  _T("Custom Project (XM32TE) - Bootloader ID:12"),
  _T("maXTouch mXT384e - Bootloader ID:13"),
  _T("Custom Project (Chipset) - Bootloader ID:14"),
  _T("Custom Project (Chipset) - Bootloader ID:15"),
  _T("maXTouch mXT1386E/mXT1716E - Bootloader ID:16"),
  _T("maXTouch mXT1536E - Bootloader ID:17"),
  _T("maXTouch mXT1536E - Bootloader ID:18"),
  _T("maXTouch mXT286E - Bootloader ID:19"),
  _T("maXTouch mXT1664S - Bootloader ID:20"),
  _T("maXTouch mXT336S (pre-production) - Bootloader ID:21"),
  _T("maXTouch mXT540ET - Bootloader ID:22"),
  _T("maXTouch mXT224S (pre-production) - Bootloader ID:23"),
  _T("Custom Project (mXT768E) - Bootloader ID:24"),
  _T("maXTouch mXT224SL (production) - Bootloader ID:25"),
  _T("maXTouch mXT336S (production) - Bootloader ID:26"),
  _T("maXTouch mXT224S (production) - Bootloader ID:27"),
  _T("maXTouch mXT3432S - Bootloader ID:28"),
  _T("maXTouch mXT1188S - Bootloader ID:29"),
  _T("maXTouch mXT112S - Bootloader ID:30"),
  _T("Custom Project (XM32TS) - Bootloader ID:31"),
  _T("Obsolete ID - Bootloader ID:32"),
  _T("maXTouch mXT540S (pre-production) - Bootloader ID:33"),
  _T("Custom Project (UC3L) - Bootloader ID:34"),
  _T("Custom Project (UC3L) - Bootloader ID:35"),
  _T("maXTouch mXT1386E/mXT1716E - Bootloader ID:36"),
  _T("maXTouch mXT1664S (2nd release) - Bootloader ID:37"),
  _T("maXTouch mXT1188S (2nd release) - Bootloader ID:38"),
  _T("maXTouch mXT1188S (incell eng release) - Bootloader ID:39"),
  _T("maXTouch mXT336S (2nd release) - Bootloader ID:40"),
  _T("maXTouch mXT224S (2nd release) - Bootloader ID:41"),
  _T("Custom Project (224E) - Bootloader ID:42"),
  _T("Custom Project (224S) - Bootloader ID:43"),
  _T("Custom Project (336S) - Bootloader ID:44"),
  _T("maXFusion mXF1664SI - Bootloader ID:45"),
  _T("maXFusion mXF1664SU - Bootloader ID:46"),
  _T("maXFusion mXF0100 - Bootloader ID:47"),
  _T("Obsolete ID - Bootloader ID:48"),
  _T("Obsolete ID - Bootloader ID:49"),
  _T("Custom Project (112S) - Bootloader ID:50"),
  _T("maXTouch mXT540S (production) - Bootloader ID:51"),
  _T("Custom Project (112E) - Bootloader ID:52"),
  _T("maXTouch mXT2952T - Bootloader ID:53"),
  _T("maXTouch mXT2952T (engineering version) - Bootloader ID:54"),
  _T("Custom Project (224SL) - Bootloader ID:55"),
  _T("Custom Project (336S) - Bootloader ID:56"),
  _T("maXTouch mXT1664TC2U - Bootloader ID:57"),
  _T("maXTouch mXT1664S - Bootloader ID:58"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:59"), 
  _T("maXFusion MXF0100i - Bootloader ID:60"),
  _T("maXStylus mXTS200 - Bootloader ID:61"), 
  _T("maXTouch mXTP540S - Bootloader ID:62"), 
  _T("maXTouch mXC576T - Bootloader ID:63"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:64"), 
  _T("maXTouch mXT640T - Bootloader ID:65"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:66"), 
  _T("maXTouch mXT224SC17 - Bootloader ID:67"), 
  _T("maXTouch mXT2952T2 - Bootloader ID:68"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:69"), 
  _T("maXTouch mXT336T - Bootloader ID:70"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:71"), 
  _T("maXTouch mXT1664T2C2U - Bootloader ID:72"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:73"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:74"), 
  _T("maXTouch mXT2405T - Bootloader ID:75"), 
  _T("maXTouch mXT336T - Bootloader ID:76"), 
  _T("maXTouch mXT224T - Bootloader ID:77"), 
  _T("maXTouch mXT3432SAT - Bootloader ID:78"), 
  _T("maXTouch mXT1066T2 - Bootloader ID:79"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:80"), 
  _T("maXTouch mXT1066T2 - Bootloader ID:81"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:82"), 
  _T("maXTouch mXT2954T2 - Bootloader ID:83"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:84"), 
  _T("maXTouch mXT2405T2 - Bootloader ID:85"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:86"), 
  _T("maXTouch mXT2407T2 - Bootloader ID:87"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:88"), 
  _T("maXTouch mXT1666T2C2U - Bootloader ID:89"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:90"),
  _T("maXTouch mXT1068T2 - Bootloader ID:91"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:92"), 
  _T("maXTouch mXT337T   - Bootloader ID:93"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:94"), 
  _T("maXTouch mXT225T   - Bootloader ID:95"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:96"), 
  _T("maXTouch mXT1664T3 1.0 - Bootloader ID:97"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:98"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:99"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:100"),
  _T("maXTouch mXT1068T2 - Bootloader ID:101"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:102"), 
  _T("maXTouch mXT874U - Bootloader ID:103"), 
  _T("maXTouch mXT1664T3 1.0 - Bootloader ID:104"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:105"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:106"), 
  _T("maXTouch mXT641T-AT - Bootloader ID:107"), 
  _T("maXTouch mXT449T-AT - Bootloader ID:108"), 
  _T("maXTouch mXT641TP-AT - Bootloader ID:109"), 
  _T("maXTouch mXT449TP-AT - Bootloader ID:110"),
  _T("maXTouch mXT1667T2 - Bootloader ID:111"),
  _T("UNKNOWN BOOTLOADER - Bootloader ID:112"), 
  _T("maXTouch mXT1668T2 - Bootloader ID:113"),
  _T("UNKNOWN BOOTLOADER - Bootloader ID:114"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:115"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:116"), 
  _T("UNKNOWN BOOTLOADER - Bootloader ID:117"), 
  _T("maXTouch mXT225T-AT - Bootloader ID:118"), 
  _T("maXTouch mXT2954TO - Bootloader ID:119"), 
  _T("maXTouch mXT1665TAT - Bootloader ID:120"), 
};

// buffer size is Page size + 20 generally, allow upto 512byte paged device for unknown number 
const int version_decoder_max_frame_size[NUM_BOOTLOADER_IDS] =      { 532,
  276,
  276,
  148,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276,
  276, 
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Spare ... */
  532, /* Unused - base extended ID index */
  532,
  532,
  276, /* 1386 on uc3d*/
  276, /* 768 */
  276, /* 540 */
  276, /* 224e */
  276, /* 143e */
  276, /* 112e */
  276, /* 768e revE */
  276, /* 540e revE */
  276, /* mXT1716 on UC3D */
  276, /* 224e custom product */
  276, /* 384e */
  532, /* 1386c1 */
  532, /* 1386c2 */
  532, /* 1716p */
  276, /* mXT1536e */
  276, /* mXT1536e */
  276, /* 286e */
  276, /* 1664 */
  276, /* 336s */
  276, /* 540ET */
  276, /* 224S */
  276, /* 768EC02 */
  276, /* 224sl */
  276, /* 336s */
  276,  /* 224s */
  532, /* "maXTouch mXT3432S - Bootloader ID:28" */
  276, /* "maXTouch mXT1188S - Bootloader ID:29" */
  276, /* "maXTouch mXT112S - Bootloader ID:30"  */
  276, /* "Custom Project (XM32TS) - Bootloader ID:31" */
  532, /* "maXTouch mXT1386E - Bootloader ID:32" */
  276, /* "maXTouch mXT540S - Bootloader ID:33" */
  276, /* "Custom Project (UC3L) - Bootloader ID:34" */
  276, /* "Custom Project (UC3L) - Bootloader ID:25" */
  532,  /* "maXTouch mXT1386E - Bootloader ID:36" */
  276, /* "maXTouch mXT1664S (2nd release) - Bootloader ID:37", */
  276, /* "maXTouch mXT1188S (2nd release) - Bootloader ID:38", */
  276, /* "maXTouch mXT1188S (incell eng release) - Bootloader ID:39", */
  276, /* "maXTouch mXT336S (2nd release) - Bootloader ID:40", */
  276, /* "maXTouch mXT224S (2nd release) - Bootloader ID:41", */
  276, /* "Custom Project (224E) - Bootloader ID:42", */
  276, /* "Custom Project (224S) - Bootloader ID:43", */
  276, /* "Custom Project (336S) - Bootloader ID:44", */
  532, /* "maXFusion mXF1664SI - Bootloader ID:45", */
  532, /* "maXFusion mXF1664SU - Bootloader ID:46", */
  532, /* "maXFusion mXF0100 - Bootloader ID:47", */
  532, /* "Obsolete ID - Bootloader ID:48", */
  532, /* "Obsolete ID - Bootloader ID:49", */
  532, /* "Custom Project (112S) - Bootloader ID:50", */
  532, /* "maXTouch mXT540S (production) - Bootloader ID:51", */
  532, /* "Custom Project (112E) - Bootloader ID:52", */
  532, /* "maXTouch mXT2952T - Bootloader ID:53", */
  532, /* "maXTouch mXT2952T (engineering version) - Bootloader ID:54", */
  532, /* "Custom Project (224SL) - Bootloader ID:55", */
  532, /* "Custom Project (336S) - Bootloader ID:56", */
  532,  /* "maXTouch mXT1664T - Bootloader ID:57", */
  532,  /* "Unknown - Bootloader ID:58", */
  532,  /* "Unknown - Bootloader ID:59", */
  532,  /* "maXTouch mXT1664T - Bootloader ID:60", */
  532,  /* "Unknown - Bootloader ID:61", */
  532,  /* "maXTouch mXT540S - Bootloader ID:62", */
  532,  /* "Unknown - Bootloader ID:63", */
  532,  /* "Unknown - Bootloader ID:64", */
  532,  /* "Unknown - Bootloader ID:65", */
  532,  /* "Unknown - Bootloader ID:66", */
  532,  /* "Unknown - Bootloader ID:67", */
  532,  /* "maXTouch mXT2952T2 - Bootloader ID:68", */
  532,  /* "Unknown - Bootloader ID:69", */
  532,  /* "Unknown - Bootloader ID:70", */
  532,  /* "Unknown - Bootloader ID:71", */
  532,  /* "Unknown - Bootloader ID:72", */
  532,  /* "Unknown - Bootloader ID:73", */
  532,  /* "Unknown - Bootloader ID:74", */
  532,  /* "Unknown - Bootloader ID:75", */
  532,  /* "Unknown - Bootloader ID:76", */
  532,  /* "Unknown - Bootloader ID:77", */
  532,  /* "maXTouch mXT3432SAT - Bootloader ID:78", */
  532,  /* "Unknown - Bootloader ID:79", */
  532,  /* "Unknown - Bootloader ID:80", */
  532,  /* "Unknown - Bootloader ID:81", */
  532,  /* "Unknown - Bootloader ID:82", */
  532,  /* "Unknown - Bootloader ID:83", */
  532,  /* "Unknown - Bootloader ID:84", */
  532,  /* "Unknown - Bootloader ID:85", */
  532,  /* "Unknown - Bootloader ID:86", */
  532,  /* "Unknown - Bootloader ID:87", */
  532,  /* "Unknown - Bootloader ID:88", */
  532,  /* "Unknown - Bootloader ID:89", */
  532,  /* "Unknown - Bootloader ID:90", */
  532,  /* "Unknown - Bootloader ID:91", */
  532,  /* "Unknown - Bootloader ID:92", */
  532,  /* "Unknown - Bootloader ID:93", */
  532,  /* "Unknown - Bootloader ID:94", */
  532,  /* "Unknown - Bootloader ID:95", */
  532,  /* "Unknown - Bootloader ID:96", */
  532,  /* "Unknown - Bootloader ID:97", */
  532,  /* "Unknown - Bootloader ID:98", */
  532,  /* "Unknown - Bootloader ID:99", */
  532,  /* "Unknown - Bootloader ID:100", */
  532,  /* "Unknown - Bootloader ID:101", */
  532,  /* "Unknown - Bootloader ID:102", */
  532,  /* "Unknown - Bootloader ID:103", */
  532,  /* "Unknown - Bootloader ID:104", */
  532,  /* "Unknown - Bootloader ID:105", */
  532,  /* "Unknown - Bootloader ID:106", */
  532,  /* "Unknown - Bootloader ID:107", */
  532,  /* "Unknown - Bootloader ID:108", */
  532,  /* "Unknown - Bootloader ID:109", */
  532,  /* "Unknown - Bootloader ID:110", */
  532,  /* "Unknown - Bootloader ID:111", */
  532,  /* "Unknown - Bootloader ID:112", */
  532,  /* "Unknown - Bootloader ID:113", */
  532,  /* "Unknown - Bootloader ID:114", */
  532,  /* "Unknown - Bootloader ID:115", */
  532,  /* "Unknown - Bootloader ID:116", */
  532,  /* "Unknown - Bootloader ID:117", */
  532,  /* "Unknown - Bootloader ID:118", */
  532,  /* "Unknown - Bootloader ID:119", */
  532,  /* "Unknown - Bootloader ID:120", */
};

int max_frame_size = version_decoder_max_frame_size[0];

// FamilyID | VariantID 
const int application_identifier[NUM_APPLICATIONS]            =       {0x0000,
  0x3810,
  0x3101,
  0x8000,
  0x8001,
  0x8002,
  0x8003,
  0x8006,
  0x8007,
  0x800C,
  0x4F00};

const TCHAR *application_information[NUM_APPLICATIONS]    =       {_T("Unknown Chip"),
  _T("5199518K01"),
  _T("51001211002"),
  _T("maXTouch mXT224 (pre-production)"),
  _T("maXTouch mXT224"),
  _T("maXTouch mXT224 (Dual Touchscreen)"),
  _T("maXTouch mXT165"),
  _T("maXTouch mXT140 (pre-production)"),
  _T("maXTouch mXT140"),
  _T("maXTouch mXT25C09"),
  _T("AT42QT25C03")
};

int application_supported_bootloader[NUM_APPLICATIONS][NUM_BOOTLOADER_IDS] =
{
  {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F,0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,},
  {0x00,0x04,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x03,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x09,0x07,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0A,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0B,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0C,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0E,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
  {0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
};

// Used to emulate QTServer when writing .xcfg files
const TCHAR *ShortDays[] = { _T("Sun"),
  _T("Mon"),
  _T("Tue"),
  _T("Wed"),
  _T("Thu"),
  _T("Fri"),
  _T("Sat")};

const TCHAR *ShortMonths[] = { _T("Dummy"),
  _T("Jan"),
  _T("Feb"),
  _T("Mar"),
  _T("Apr"),
  _T("May"),
  _T("Jun"),
  _T("Jul"),
  _T("Aug"),
  _T("Sep"),
  _T("Oct"),
  _T("Nov"),
  _T("Dec")};

// unknown bootloader can accept any app, unknown app can go in any bootloader 
// HID-I2C variables

// #define DBGPRINT if (DebugMode) _tprintf

extern HANDLE hI2CDriver;
extern HANDLE hI2CDriverBootloader;
extern UINT8 OutputRegisterLow;
extern UINT8 OutputRegisterHigh;

BOOL bIsXCfg = FALSE;
BOOL bIsRaw = FALSE;

BOOL InitI2CNormalDone = FALSE;
BOOL InitI2CBootloaderDone = FALSE;
BOOL ConfigObjectNotFoundError = FALSE;

// Flag to get extra debug printf
BOOL DebugMode = FALSE;
// Flag for special log requirements of Samsung
BOOL LogMode = FALSE;
fstream LogFile;
// Lof file for Service version
BOOL LogModeSvc = FALSE;

// no printouts
BOOL SilentMode = FALSE;

// Flag for MaxTouchAnalyzer
BOOL MaxTouchAnalyzer = FALSE;

// Flag for MaxTouchStudio
BOOL MaxTouchStudio = FALSE;

// Flag for setting return error codes
BOOL ErrorCodes = FALSE;

// List the Object type that do are not configurable
// Note that DEBUG_DIAGNOSTIC_T37 is read-only but we want to save its value to config file
int NonConfigurableObjects[] = {SPT_MESSAGECOUNT_T44,
  GEN_MESSAGEPROCESSOR_T5,
  GEN_COMMANDPROCESSOR_T6,
  GEN_DATASOURCE_T53,
  NULL
};

int bootloader_state = 0;
int bootloader_next_state = 0;
USHORT realbootloader_id = 0;
unsigned short bootloader_id = 0;
unsigned short bootloader_version = 0;
int bootloader_i2c_address = 0;
bool app_crc_fail = false;
bool bFirstTime = true;

bool ok_to_use_change_line = true;    // Initialize it to true for mXT1386/768E (our only case now) - USB bootloading
bool ok_to_use_400khz = false;
bool bridge_in_HS_mode = true;        // Initialize it to true for mXT1386/768E (our only case now)
bool force_flash_supported = false;
bool chg_is_driven_low = false;
int number_of_bytes_per_write = 60;   // Initialize it to true for mXT1386/768E (our only case now)

#define BOOTLOADING_MODE__I2C   0u
#define BOOTLOADING_MODE__USB   1u
int bootloading_mode = BOOTLOADING_MODE__USB;   // Set USB mode for mXT1386/768E

UINT8 usbData[9];
UINT8 usbData_64[65];
char temp;
int EncData[MAX_CHARS];             // Limits to around 128kB Flash code
int ByteData[MAX_CHARS >> 1u];      // Bytes for sending
int CharCounter = 0;
int TotalCharCount = 0;
int DataBytes[7];
int FrameNumber = 0;

int crc_fail_counter = 0;
int retry_counter = 0;
int change_line_high_counter = 0;

int current_frame = 0;
int current_byte_index = 0;
int current_frame_size = 0;
int current_frame_index = 0;
int total_frames = 0;
int largest_frame_size = 0;

enum CMDFIELDS {
  eReset,
  eBackupNV,
  eCalibrate,
  eReportAll,
  eReserved,
  eDiagnostic,
} CmdFields;

enum GRSTATUS {
  eCouldNotGet,
  eNotSupported,
  eBadStoredData,
  eGRStoredFailed,
  eGoodGRStored,
} GRStatus;

TCHAR EncFile[MAX_PATH];
TCHAR CfgFile[MAX_PATH];
TCHAR ThqaFile[MAX_PATH];
TCHAR SerialNumFile[MAX_PATH];	//Added MCG - 20160907 for -SW option
//These are the vendor and product IDs to look for.
//Uses ATMEL's Vendor ID.
const int VENDORID = 0x03EB;

const int BOOTLOADER_ID = 0x211D;

// Unique ID info, device-dependent
int UniqueIDPage = -1;  // if not set, unsupported
UINT UniqueIDStart = 64; // pre-initialize to T and T2 series
UINT UniqueIDSize = 16;
int UniqueIDRevIDPage = -1;
UINT UniqueIDRevIDStart = 3; //Revision ID for 224S etc..

// if you update this enum, make sure you update TCHAR *DEVICE_NAME[] 
enum MXTDEVICES {
  eUnrecognized,
  eMXT1386,
  eMXT112E,
  eMXT224E,
  eMXT768E,
  eMXT1386E,
  eMXT1716E,
  eMXT1664S,
  eMXT3432S,
  eMXTP540S,
  eMXT2952T,
  eMXT1664T,
  eMXF0100i,
  eMXT336T,
  eMXT2952T2,
  eMXT1664T2,
  eMXT640T,
  eMXT641T,
  eMXT2405T,
  eMXT112S,
  eMXT1188S,
  eMXT224T,
  eMXT1066T2,
  eMXT2954T2,
  eMXT2405T2,
  eMXT2407T2,
  eMXT1666T2,
  eMXT224S,
  eMXT336S,
  eMXT540S,
  eMXC576T,
  eMXT337T,
  eMXT225T,
  eMXT336TO,
  eMXT641T_AT,
  eMXT449T_AT,
  eMXT641TP_AT,
  eMXT449TP_AT,
  eMXT1068T2,
  eMXT1667T2,
  eMXT1668T2,
  eMXTS200,
  eMXTS220,
  eMXT874U,
  emXT1664T3,
  emXT1668T2C01,
  emXT225T_AT,
  emXT643T,
  emXT2952T2C01,
  emXT2954TO,
  emXT1665TAT,
  emXT1664T2C2UC01,
  emXT2952TM,
} MxtDevices;

enum OBJECT70 {
  eCtrl,
  eEvent,
  eObjtype,
  eReserved1,
  eObjinst,
  eDstoffset,
  eSrcoffset,
  eLength,
} Object70;

const TCHAR *IDENTIFY_CONNECTION_MSG[] = {_T("HID-USB compliant"), _T("HID-I2C compliant"), _T("HID-I2C compliant; no support for Atmel Protocol via Windows inbox driver"), _T("I2C connected; Windows inbox driver unsupported (Standard Atmel FW)"), _T("USB-I2C Bridge Board")};

// if you update this enum, make sure you update enum MXTDEVICES 
const TCHAR *DEVICE_NAME[] = {_T("Unrecognized"), _T("maXTouch mXT1386"), _T("maXTouch mXT112E"), _T("maXTouch mXT224E"), _T("maXTouch mXT768E"), _T("maXTouch mXT1386E"), _T("maXTouch mXT1716E"), _T("maXTouch mXT1664S"),  _T("maXTouch mXT3432S"),  
                              _T("maXTouch mXT540S"), _T("maXTouch mXT2952T"), _T("maXTouch mXT1664T"), _T("maXFusion MXF0100i"), _T("maXTouch mXT336T"), _T("maXTouch mXT2952T2"), _T("maXTouch mXT1664T2"), 
                              _T("maXTouch mXT640T"), _T("maXTouch mXT641T"), _T("maXTouch mXT2405T"), _T("maXTouch mXT112S"), _T("maXTouch mXT1188S"),   _T("maXTouch mXT224T"), _T("maXTouch mXT1066T2"), 
                              _T("maXTouch mXT2954T2"), _T("maXTouch mXT2405T2"), _T("maXTouch mXT2407T2"), _T("maXTouch mXT1666T2"), _T("maXTouch mXT224S"), _T("maXTouch mXT336S"), _T("maXTouch mXT540S"), 
                              _T("maXTouch mXC576T"), _T("maXTouch mXT337T"), _T("maXTouch mXT225T"), _T("maXTouch mXT336TO"), _T("maXTouch mXT641T-AT"), _T("maXTouch mXT449T-AT"), _T("maXTouch mXT641TP-AT"), 
                              _T("maXTouch mXT449TP-AT"), _T("maXTouch mXT1068T2"), _T("maXTouch mXT1667T2"), _T("maXTouch mXT1668T2"), _T("maXStylus mXTS200"), _T("maXStylus mXTS220"), _T("maXTouch mXT874U"),
                              _T("maXTouch emXT1664T3"), _T("maXTouch emXT1668T2C01"), _T("maXTouch emXT225T-AT"), _T("maXTouch emXT643T"), _T("maXTouch emXT2952T2C01"), _T("maXTouch emXT2954TO"), _T("maXTouch emXT1665TAT"), 
                              _T("maXTouch mXT1664T2C2UC01"), _T("maXTouch mXT2952TM")};

const TCHAR *ATMEL_CTRL_STRING[] = {_T("Atmel maXTouch Control"), _T("QRG-I/F"), _T("5055")};

const TCHAR *ATMEL_EXCL_STRING[] = {_T("JTAG")};

const TCHAR *FromXmlFileMsg = _T("Using Device Definition file to get device info.");

const TCHAR *DevicesDefinition = _T("DevicesInfo.bin");

BOOL Protocol_HID = FALSE;  
UINT8 HIDHEADER[64];
 
const TCHAR *ATMEL_BOOTLOADER_STRING[] = {_T("Atmel_maXTouch_Bootloader"), _T("Atmel maXTouch Bootloader")};

const TCHAR *HIDI2C_DEVICE_STRING[] = {_T("\\\\?\\hid#atml1000&col02"), _T("\\\\?\\hid#msft2002&col02"), _T("\\\\?\\hid#atml1hid&col02"), NULL};

const USHORT NORMAL_USAGE_PAGE     = 0xFF00;
const USHORT BOOTLOADER_USAGE_PAGE = 0xFFFF;

const TCHAR *LogFileName = _T("MxtUpdater.log");
const TCHAR *LogFileSuccess = _T("Successful.log");
const TCHAR *LogFileFail = _T("Failed.log");
const TCHAR *LogFileAuto = _T("TouchUpdater.log");

// To be used if there is a MxtUpdater.ini file
TCHAR Custom_DevicePath[64];
BOOL IsCustomDevicePath = FALSE;

const UINT8 BOOTLOADER = 0xA5;
const UINT8 RESET = 0x01;
const UINT8 BACKUPNVM = 0x55;
TCHAR PrintBuffer[512];

const UINT8 TouchSelfCapObjects[] = {9, 15, 23, 52, 100, 97, 0};
UINT TouchSelfCapTotalInstances = 0;
// Max number of devices attached (not a hard limit, it can be changed)
const UINT MaxDevicesAttached = 32;

UINT8 AtmelReportId = ATMEL_REPORT_ID_USB;

BOOL CheckBootloaderOnly = FALSE;
BOOL LeaveInBootloaderMode = FALSE;
// Parameter introduced for solving Compal problem
BOOL USBReadDelay = FALSE;

// Use a second vector to remember the device number after the first discovery
// When setting in Bootloader mode the order may change during the second discovery
vector<HID_DEVICE *> HidDevice[2];
vector<MAXTOUCHDEVICES *> MxtCtrlDevice[2];
UINT DeviceIndex = 1;
UINT DevicePID = 0x0000;

// keep track of the devices in bootloader mode during discovery
UINT DeviceInBootloader[2] = {0, 0};

UsbHidDevice mXTDevice;
BOOL DeviceAlreadyIdentified = FALSE;
HANDLE HandleDeviceRead;

SECURITY_ATTRIBUTES ReadWriteEventAttribs;
SECURITY_ATTRIBUTES CompletionEventAttribs;
HANDLE hReadWriteEvent;
HANDLE hCompletionEvent;
// Used to terminate when we got out of sync...
BOOL bStateOk = TRUE;
UINT DeviceConnected;
UINT AllPids[MaxDevicesAttached];

// If TRUE, DevicesInfo.bin is present (MaxTouchStudio), if FALSE data is coming from embedded DevicesInfo.bin
BOOL XMLFilePresent = FALSE;
// If TRUE, it means that the name in DeviceAppNameFromXML is valid
BOOL DeviceAppFromXML = FALSE;
// If TRUE, it means that the name in DeviceBtlNameFromXML is valid
BOOL DeviceBlrFromXML = FALSE;
TCHAR DeviceAppNameFromXML[32];
TCHAR DeviceBtlNameFromXML[32];

BOOL UseCustomString = FALSE;

BOOL DeltasRefsFirstTime = TRUE;
BOOL SelfCapFirstTime = TRUE;

int DeviceDiscovered;

UINT ConnectFWType = HIDUSB;

struct MemoryMap DeviceMemoryMap;
vector<ObjectTableElement*> AllObjects;
ULONG NumberOfObjects = 0;

BOOL VerifyConfigEnabled = FALSE;
BOOL ClearConfig = TRUE;

UINT UsbI2c = 0;    // for Samsung

// Exclude T37, T68 and any dynamic objects
//UINT CfgExcludedObj[32] = {37, 68, 0};
//vector<UINT> CfgDynamicObjects;
#ifdef MXT_VERIFIER
extern UINT Version;
extern UINT16 ProductID;
#endif

vector<OPObject*> ObjectsInFile, ObjectsInFW; //, DynamicObjects;
UINT8 ReportIds[256];

UINT32 ConfigChecksum; // Holds the checksum calculated or read from the config file to be written

// Hold the values read from config file
UINT CfgFamilyId;
UINT CfgVariant;
UINT CfgVersion;
UINT CfgBuild;

#ifdef AUTO_UPDATER
UINT8 *FWFileRes;
UINT8 *CfgFileRes;
UINT8 *ThqaFileRes;
DWORD FWFileSize;
DWORD CfgFileSize;
DWORD ThqaFileSize;
//UINT8 *FWFileRes2;
UINT8 *CfgFileResAll[MAX_PAYLOADS];
UINT8 *ThqaFileResAll[MAX_PAYLOADS];
DWORD FWFileSizeAll[MAX_PAYLOADS];
DWORD CfgFileSizeAll[MAX_PAYLOADS];
DWORD ThqaFileSizeAll[MAX_PAYLOADS];
BOOL WriteThqa = TRUE;
BOOL CheckACPower = FALSE;
UINT RebootNeeded = 0;
BOOL PauseBeforeExit = FALSE;
BOOL TurnOnThqa = FALSE;
UINT PidAll[MAX_PAYLOADS];
#else
UINT8 *XMLFileRes;
DWORD XMLFileSize;
#endif

UINT PidToWrite = 0x8205;

UINT16 MsgProcAddress, MsgCntAddress;
UINT8 MsgProcSize;
UINT32 InfoBlockChecksum;

UINT8 ShowObjMsg = 0xFF;  // Miracle. 2014/05/29.

using namespace std;

int IdentifyConnectFWType()
{
  UINT i = 0;
  // Check if I2C device in Bootloader mode
  if (IsI2CDeviceBootloaderMode(TRUE)) {
    // It can be any type of I2C if in Bootloader mode
    DeviceDiscovered = I2C_BOOTLOADER;
    ConnectFWType = HIDI2CNOINBOX;
    return ConnectFWType;
  }
  // Check if standard USB or discoverable I2C
  DeviceDiscovered = FindDiscoverableDevice();

  if (DeviceDiscovered == NOT_FOUND) return NOT_FOUND;

  // Added by Miracle.
  if (DeviceDiscovered == USB_NORMAL && ConnectFWType == I2CBRIDGE) {
    return ConnectFWType;
  } else if (DeviceDiscovered == I2C_BOOTLOADER && ConnectFWType == I2CBRIDGE) {
    DBGPRINT(_T("\nBridge board connect and connected device in BL mode\n"));
    return ConnectFWType;
  }
  else if (DeviceDiscovered == USB_NORMAL || DeviceDiscovered == USB_BOOTLOADER) {

    ConnectFWType = HIDUSB;
    if (DeviceDiscovered == USB_NORMAL) {
      if (!ReadMemoryMap(TRUE)) {
        _stprintf(PrintBuffer, _T("Found a USB-connected device. Error communicating with it. Try power-cycling it.\n"));
        _TPRINTF(PrintBuffer);
        return -1;
      }
    }
    return ConnectFWType;
  }
  else if (DeviceDiscovered == I2C_NORMAL && ConnectFWType == HIDI2C) {
	  // Miracle. 2016/06/06. For HIDI2C BL.
	  return ConnectFWType;
  }

  if (DeviceDiscovered == I2C_NORMAL) {
    ConnectFWType = HIDI2C;
    if (ReadMemoryMap(TRUE)) {
      return ConnectFWType;
    }
  }

  // Check if I2C with partial (digitizer only) Inbox support
  // Set as non-HID device
  ConnectFWType = HIDI2CNOINBOX;
  if (InitI2CNormal(TRUE)) {
    if (ReadMemoryMap(TRUE) && ValidateDevice()) {
      return ConnectFWType;
    }
  }
  // Check if I2C with full Inbox support
  AtmelReportId = ATMEL_REPORT_ID_I2C;
  ConnectFWType = HIDI2C;
  if (IsCustomDevicePath && FindHIDI2CDeviceByDevPath(Custom_DevicePath)) {
    if ((mXTDevice.InputReportByteLength == 18) && (mXTDevice.OutputReportByteLength == 18)) {
      if (ReadMemoryMap(TRUE)) {
        return ConnectFWType;
      }
    } else {
      // This is I2C without object protocol inbox support (partial, digitizer-only Inbox support)
      ConnectFWType = HIDI2CNOINBOX;
      if (ReadMemoryMap(TRUE)) {
        return ConnectFWType;
      }
      _stprintf(PrintBuffer, _T("Found a I2C-connected device. Error communicating with it.\nCheck if you have the correct drivers installed and try power-cycling it.\n"));
      _TPRINTF(PrintBuffer);
      return -1;
    }
  }
  while (HIDI2C_DEVICE_STRING[i]) {
    if (FindHIDI2CDeviceByDevPath(HIDI2C_DEVICE_STRING[i])) {
      if ((mXTDevice.InputReportByteLength == 18) && (mXTDevice.OutputReportByteLength == 18)) {
        if (ReadMemoryMap(TRUE)) {
          return ConnectFWType;
        }
      } else {
        // This is I2C without object protocol inbox support (partial, digitizer-only Inbox support)
        ConnectFWType = HIDI2CNOINBOX;
        if (ReadMemoryMap(TRUE) && ValidateDevice()) {
          return ConnectFWType;
        }
      }
    }
    ++i;
  }
  // Check if I2C with no Inbox support
  // Set as non-HID device
  ConnectFWType = I2CSTOCK;
  if (ReadMemoryMap(TRUE)) {
    return ConnectFWType;
  }
  return -1;
}

BOOL IsI2CDeviceBootloaderMode(BOOL Quiet)
{
  // Try to init communication with our Bootloader device
  if (!InitI2CBootloader(Quiet)) {
    return FALSE;
  }
  // Check if in already bootloader mode
  if (BootWorkerAnyState()) {
    // device is ready in bootloader mode
    return TRUE;
  }
  return FALSE;
}

// Miracle. 2014/04/14. For HIDI2C data access via inbox driver. Sometimes reading data follow WriteFile will fail. So implement error try.
UINT8 Data_Last[80];
BOOL ReadWriteDevice_InboxDriver_DuplicateCheck(UINT8 *writeBuffer, DWORD *bytesWrittenUSB, UINT8 *readBuffer, DWORD *bytesReadUSB, UINT8 readsize)
{
	BOOL bReturncode;
	int i;

	//printf("Inbox: InputLen=%d OutLen=%d\n", mXTDevice.OutputReportByteLength, mXTDevice.InputReportByteLength);

	for (i = 0; i <= 5; i++) {
		if (i == 5) {
			return FALSE;
		}
		if (i > 0)
		{
			// printf("ReadWriteDevice_InboxDriver: 1. Error try %d\n", i);
			Sleep(10);
		}
		*bytesWrittenUSB = 0;
		*bytesReadUSB = 0;
		bReturncode = WriteFile(mXTDevice.HidDevice,
			writeBuffer,
			mXTDevice.OutputReportByteLength,
			bytesWrittenUSB,
			NULL) && (*bytesWrittenUSB == mXTDevice.OutputReportByteLength);
		if (!bReturncode)
			continue;
		break;
	}

	if (readsize <= 0)		// 2016/03/28. Support write only without read. For sending ENC payload without read.
		return TRUE;

	//printf("Enter next step 11 input data: %d rpt id: %d\n", mXTDevice.InputReportByteLength, AtmelReportId);
	//mXTDevice.InputReportByteLength = 16;
	BOOL isTheSame = FALSE;		// We only read one more time.
	for (i = 0; i <= 5; i++) {
		if (i == 5) {
			return FALSE;
		}
		if (i > 0)
		{
			// printf("ReadWriteDevice_InboxDriver: 2. Error try %d\n", i);
			Sleep(10);
		}

		if (!MaxTouchAnalyzer)
			Sleep(5);		// Miracle. 2014/11/18. One issue is found mxt2952T2 when write ReportAll command but more than one message back too fast.

		//printf("Before ReadFile: inp len=%d. Out len=%d.\n", mXTDevice.InputReportByteLength, mXTDevice.OutputReportByteLength);
		bReturncode = ReadFile(mXTDevice.HidDevice,
			readBuffer,
			mXTDevice.InputReportByteLength,
			bytesReadUSB,
			NULL) && (*bytesReadUSB == mXTDevice.InputReportByteLength) && (readBuffer[0] == AtmelReportId);

#if FALSE	// 2016/03/21. Miracle. Data debug.
		printf("ReadFile: %d byteread: %d\n", bReturncode, *bytesReadUSB);
		for (int idx = 0; idx < mXTDevice.InputReportByteLength; idx++)
			printf("%02x ", readBuffer[idx]);
		printf("\n");

		printf("After ReadFile: inp len=%d. Out len=%d.\n", mXTDevice.InputReportByteLength, mXTDevice.OutputReportByteLength);
#endif
		if (!bReturncode || readBuffer[2] < readsize) {
			continue;
		}

		for (int idx = 0; idx < mXTDevice.InputReportByteLength && idx < 80; idx++) {
			if (Data_Last[idx] != readBuffer[idx]) {
				isTheSame = FALSE;
				Data_Last[idx] = readBuffer[idx];
			}
		}
		if (isTheSame == TRUE)
			continue;		// If the read package the same as previous one. We default think it is duplicate package read by inbox driver.

		break;
	}

	return TRUE;
}


// Miracle. 2014/04/14. For HIDI2C data access via inbox driver. Sometimes reading data follow WriteFile will fail. So implement error try.
BOOL ReadWriteDevice_InboxDriver(UINT8 *writeBuffer, DWORD *bytesWrittenUSB, UINT8 *readBuffer, DWORD *bytesReadUSB, UINT8 readchk)
{
  BOOL bReturncode;
  int i;

  for(i=0; i<5; i++) {
    *bytesWrittenUSB = 0;
    *bytesReadUSB = 0;
    bReturncode = WriteFile (mXTDevice.HidDevice,
              writeBuffer,
              mXTDevice.OutputReportByteLength,
              bytesWrittenUSB,
              NULL) && (*bytesWrittenUSB == mXTDevice.OutputReportByteLength);
	
	if (bReturncode) {
      if (!MaxTouchAnalyzer) Sleep(5);
      bReturncode = ReadFile(mXTDevice.HidDevice,
              readBuffer,
              mXTDevice.InputReportByteLength,
              bytesReadUSB,
              NULL) && (*bytesReadUSB == mXTDevice.InputReportByteLength) && (readBuffer[0] == AtmelReportId) && (readBuffer[1] == readchk);
    }

    if (!bReturncode) {
      Sleep(10);
      continue;
    }

    return TRUE;
  }
  if(i==5)
    return FALSE;

  return TRUE;
}

BOOL ReadFromDevice(UINT16 Address, UINT8 *Buffer, DWORD *ByteRead)
{
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];
  DWORD bytesWrittenUSB, bytesReadUSB, bytesRemaining, totalBytesRead;
  UINT8 bytesRead;
  //BOOL bReturncode;
  UINT8 *ptrBuffer;
  UINT bufferSize = *ByteRead;

  UINT8 num_bytes;
  UINT8 num_payload = 15;


  bytesRemaining = *ByteRead;
  ptrBuffer = Buffer;
  totalBytesRead = 0;

  // Added by Miracle.
  if(DeviceDiscovered == I2C_BOOTLOADER || DeviceDiscovered == USB_BOOTLOADER)
    num_payload = 50;   // BL mode, we can send more data in one package.
  if (DeviceDiscovered == USB_NORMAL && ConnectFWType == I2CBRIDGE) {
    //DBGPRINT(_T("\nReadFromDevice: %d\n"), bytesRemaining);
    for(;bytesRemaining > 0; ) {
      writeBuffer[0] = Address & 0xFF;
      writeBuffer[1] = (Address>>8) & 0xFF;
      if(bytesRemaining > num_payload)
        num_bytes = num_payload;
      else
        num_bytes = (UINT8)(bytesRemaining&0xFF);
      if(! I2CBridge_ReadWrite(2, writeBuffer, num_bytes, readBuffer))
        return FALSE;
      for(UINT i=0; i<num_bytes; i++) {
        *ptrBuffer = readBuffer[i];
        ptrBuffer++;
      }
      totalBytesRead += num_bytes;
      Address += num_bytes;
      bytesRemaining -= num_bytes;
    }

    *ByteRead = totalBytesRead;
    return TRUE;
  }
  else if (ConnectFWType == HIDI2CNOINBOX || ConnectFWType == I2CSTOCK) {
    bytesRead = (UINT8)ReadWithRetryFromMxt(hI2CDriver, Address, (UINT16)*ByteRead, Buffer);
    return (*ByteRead == bytesRead);
  }

  while (bytesRemaining) {
    // max readable in one shot are InputReportByteLength - 3
    UINT8 bytesToRead =(UINT8) min(mXTDevice.InputReportByteLength-3, (USHORT)bytesRemaining);
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;  // ReportID - USB stuff
    writeBuffer[1] = 0x51;    
    writeBuffer[2] = 0x02;        // write 2 bytes for the address
    writeBuffer[3] = bytesToRead; // number of bytes to read
    writeBuffer[4] = Address & 0x00ff;
    writeBuffer[5] = (Address >> 8) & 0x00ff;

    // Replaced by Miracle. Read failures seen in HP Cedar 1.1
    if(!ReadWriteDevice_InboxDriver(writeBuffer, &bytesWrittenUSB, readBuffer, &bytesReadUSB, 0)) {
      if (MaxTouchAnalyzer) {
        _stprintf(PrintBuffer, _T("ReadFromDevice: ReadWriteDevice_InboxDriver error\n"));
        _TPRINTF(PrintBuffer);
      } else {
        DBGPRINT(_T("ReadFromDevice: ReadWriteDevice_InboxDriver\n"));
      }     
      return FALSE;
    }
    bytesRead = readBuffer[2];
    // Sanity check
    if (bytesRead > bufferSize) return FALSE;
    for (UINT i = 0; i < bytesRead; ++i) {
      *ptrBuffer = readBuffer[3+i];
      ptrBuffer++;
    }
    bufferSize -= bytesRead;
    bytesRemaining -= bytesRead;
    Address += bytesRead;
    totalBytesRead += bytesRead;
  }
  *ByteRead = totalBytesRead;
  return TRUE;
}

BOOL WriteToDevice(UINT16 Address, UINT8 *Buffer, DWORD *BytesToWrite)
{
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];
  DWORD bytesRead, bytesWritten, bytesRemaining;
  //BOOL bReturncode;
  UINT16 address;

  // Added by Miracle.
  UINT8 num_bytes;
  UINT8 num_payload = 12;

  bytesRemaining = *BytesToWrite;
  UINT totalBytesSent = 0;

  address = Address;
  UINT i = 0, j = 1, k = 0;

  // Added by Miracle.
  if(DeviceDiscovered == I2C_BOOTLOADER || DeviceDiscovered == USB_BOOTLOADER)
    num_payload = 50;   // BL mode, we can send more data in one package.
  if (DeviceDiscovered == USB_NORMAL && ConnectFWType == I2CBRIDGE) {
    DBGPRINT(_T("\nWriteToDevice: %d\n"), bytesRemaining);
    for(;bytesRemaining > 0; ) {
      j=0;
      writeBuffer[0] = Address & 0xFF;
      writeBuffer[1] = (Address>>8) & 0xFF;
      if(bytesRemaining > num_payload)
        num_bytes = num_payload;
      else
        num_bytes = (UINT8)(bytesRemaining&0xFF);

      for(i=0; i<num_bytes; i++) {
        writeBuffer[i+2] = Buffer[totalBytesSent];
        totalBytesSent++;
      }
      // Miracle. For HID protocol. All write command should read back.
      if(Protocol_HID) {
        if(! I2CBridge_ReadWrite(num_bytes+2, writeBuffer, HIDHEADER[HIDDSC_InputLength_LSB], readBuffer))
          return FALSE;
      }
      else {
        if(! I2CBridge_ReadWrite(num_bytes+2, writeBuffer, 0, NULL))
          return FALSE;
      }

      Address += num_bytes;
      bytesRemaining -= num_bytes;
    }

    *BytesToWrite = totalBytesSent;
    return TRUE;
  }

  while (i < *BytesToWrite) {
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;
    writeBuffer[1] = 0x51;
    writeBuffer[3] = 0;
    writeBuffer[4] = address & 0x00FF;
    writeBuffer[5] = (address >> 8) & 0x00FF;
    k = 6;
    // (mXTDevice.OutputReportByteLength - 6) is the maximum number of bytes that can be sent to the device
    while (i < *BytesToWrite && i < j*(mXTDevice.OutputReportByteLength-6)) {
      writeBuffer[k++] = Buffer[i++];
    }
    writeBuffer[2] = (UINT8)(2 + k - 6);
    totalBytesSent += (k - 6);

    // Replaced by Miracle. Read failures seen in HP Cedar 1.1
    if(!ReadWriteDevice_InboxDriver(writeBuffer, &bytesWritten, readBuffer, &bytesRead, 4)) {
      return FALSE;
    }
    ++j;
    address = Address + (UINT16)totalBytesSent; // update address
  }
  return TRUE;
}

#if 0
BOOL EnableT5AutoReturn(BOOL Enable)
{
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];
  DWORD bytesRead, bytesWritten;
  BOOL bReturncode;
  UINT8 enableCode = 0;

  if (Enable) {
    enableCode = 0x88;
  }
  ZeroMemory(writeBuffer, 65);
  writeBuffer[0] = AtmelReportId;
  writeBuffer[1] = enableCode;

  bReturncode = WriteFile (mXTDevice.HidDevice,
    writeBuffer,
    mXTDevice.OutputReportByteLength,
    &bytesWritten,
    NULL) && (bytesWritten == mXTDevice.OutputReportByteLength);

  if (!bReturncode) return FALSE; // Write failed    
  // read response if "Enable" command; disable does not acknowledge
  if (Enable) {
    bReturncode = ReadFile(mXTDevice.HidDevice,
      readBuffer,
      mXTDevice.InputReportByteLength,
      &bytesRead,
      NULL) && (bytesRead == mXTDevice.InputReportByteLength) && (readBuffer[0] == AtmelReportId) && (readBuffer[1] == enableCode && (readBuffer[2] == 0));
  }
  return bReturncode;
}
#endif

//
// This function returns the first two bytes of T38 which represent the cfg version
// -1 if error
//
int GetConfigVersion()
{
  UINT16 address;
  UINT8 buffer[4];
  int version = -1;
  DWORD byteRead = 2;

  if (!ReadMemoryMap()) return version;
  if (!GetObjectAddressAndSize(SPT_USERDATA_T38, &address)) return version;
  if (!ReadFromDevice(address, buffer, &byteRead)) return version;
  version = (buffer[0] << 8 | buffer[1]) & 0x0000FFFF;
  return version;
}

//
// This function returns the second two bytes of T38 which represent the internal cfg version
// -1 if error
//
int GetCfgInternalVersion()
{
  UINT16 address;
  UINT8 buffer[4];
  int version = -1;
  DWORD byteRead = 4;

  if (!ReadMemoryMap()) return version;
  if (!GetObjectAddressAndSize(SPT_USERDATA_T38, &address)) return version;
  if (!ReadFromDevice(address, buffer, &byteRead)) return version;
  version = (buffer[2] << 8 | buffer[3]) & 0x0000FFFF;
  return version;
}

int GetFWVersion()
{
  int version;
  if (!ReadMemoryMap()) return -1;
  version = (DeviceMemoryMap.Version << 8 | DeviceMemoryMap.Build) & 0x0000FFFF;
//  _stprintf(buffer, _T("0x%02X%02X"), DeviceMemoryMap.Version, DeviceMemoryMap.Build);
  return(version);
}

BOOL PrintChipInfo()
{
  int i, j;
  UINT32 checksum=0;
  UINT16 version, internalVersion;
  if (!ReadMemoryMap()) return FALSE;

  if (!ValidateDevice()) {
    _stprintf(PrintBuffer, _T("Please verify that you have the correct Windows driver installed (You can use Device Manager).\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  // Moved here because, if read after checksum, we get an error when reading - very strange
  version = (UINT16)GetConfigVersion();
  internalVersion = (UINT16)GetCfgInternalVersion();

  if (!ReadNVMChecksum(&checksum)) {
    checksum = 0;   // we got an error
  }
  _stprintf(PrintBuffer, _T("\nDevice Information:\n"));
  _TPRINTF(PrintBuffer);
  i = (DeviceMemoryMap.FamilyID & 0xF0) >> 4;
  j = DeviceMemoryMap.FamilyID & 0x0F;
  _stprintf(PrintBuffer, _T("\nFamily ID: \t\t\t0x%X%X\n"), i, j);
  _TPRINTF(PrintBuffer);
  i = (DeviceMemoryMap.VariantID & 0xF0) >> 4;
  j = DeviceMemoryMap.VariantID & 0x0F;
  _stprintf(PrintBuffer, _T("Variant ID: \t\t\t0x%X%X\n"), i, j);
  _TPRINTF(PrintBuffer);
  i = (DeviceMemoryMap.Version & 0xF0) >> 4;
  j = DeviceMemoryMap.Version & 0x0F;
  _stprintf(PrintBuffer, _T("Version: \t\t\t%d.%d\n"), i, j);
  _TPRINTF(PrintBuffer);
  i = (DeviceMemoryMap.Build & 0xF0) >> 4;
  j = DeviceMemoryMap.Build & 0x0F;
  _stprintf(PrintBuffer, _T("Build: \t\t\t\t0x%X%X\n"), i, j);
  _TPRINTF(PrintBuffer);
  // Check if different than 0 (maXFusion has x = 0 and y = 0)
  if (DeviceMemoryMap.MatrixSizeX || DeviceMemoryMap.MatrixSizeY) {
    _stprintf(PrintBuffer, _T("Matrix Size X: \t\t\t%d\n"), DeviceMemoryMap.MatrixSizeX);
    _TPRINTF(PrintBuffer);
    _stprintf(PrintBuffer, _T("Matrix Size Y: \t\t\t%d\n"), DeviceMemoryMap.MatrixSizeY);
    _TPRINTF(PrintBuffer);
  }
  _stprintf(PrintBuffer, _T("Number of Objects: \t\t%d\n"), DeviceMemoryMap.NumberOfObjects);
  _TPRINTF(PrintBuffer);
// Removing checksum from display: creates confusion (HP request)
// There are two checksums: in-memory and NVM that can be displayed with "-c"
  if (checksum) {
    _stprintf(PrintBuffer, _T("Configuration Checksum: \t0x%06X\n"), checksum);
    _TPRINTF(PrintBuffer);
  }
//  CalculateChecksum(&checksum);
//  _stprintf(PrintBuffer, _T("Current Config Checksum: \t0x%06X\n"), checksum);
//  _TPRINTF(PrintBuffer);
  if (version != -1) {
    _stprintf(PrintBuffer, _T("Configuration Version: \t\t0x%04X\n"), version);
    _TPRINTF(PrintBuffer);
    _stprintf(PrintBuffer, _T("Combined Version: \t\t0x%02X%02X%04X\n"), DeviceMemoryMap.Version, DeviceMemoryMap.Build, version);
    _TPRINTF(PrintBuffer);
  }
  if (version != -1) {
    _stprintf(PrintBuffer, _T("Internal Configuration Version: 0x%04X\n"), internalVersion);
    _TPRINTF(PrintBuffer);
  }
  _stprintf(PrintBuffer, _T("Device PID:  \t\t\t0x%04X\n"), AllPids[DeviceIndex-1]);
  _TPRINTF(PrintBuffer);
  // Not yet debugged...
//  DisplayGRStatus();
  return TRUE;
}

#ifdef MXT_VERIFIER
BOOL GetPIDAndVersion()
{
  UINT16 version;
  UINT8 *ptr;
  if (!ReadMemoryMap()) return FALSE;

  version = (UINT16)GetConfigVersion();
  ptr = (UINT8 *)&version;
  if (version == -1) {
    return FALSE;
  }
  ProductID = (UINT16)AllPids[DeviceIndex-1];
  Version = (DeviceMemoryMap.Version << 24) + (DeviceMemoryMap.Build << 16) + (ptr[1] << 8) + ptr[0];
  return TRUE;
}

#endif

void DisplayGRStatus()
{
  UINT status;

  status = GoldenRefsStatus();

  switch (status) {

  case eNotSupported:
    _stprintf(PrintBuffer, _T("maXStartup (GR) status: \tGolden References not supported\n"));
    _TPRINTF(PrintBuffer);
    break;

  case eBadStoredData:
    _stprintf(PrintBuffer, _T("maXStartup (GR) status: \tBad Golden References stored\n"));
    _TPRINTF(PrintBuffer);
    break;

  case eGRStoredFailed:
    _stprintf(PrintBuffer, _T("maXStartup (GR) status: \tGolden References threshold failed\n"));
    _TPRINTF(PrintBuffer);
    break;

  case eGoodGRStored:
    _stprintf(PrintBuffer, _T("maXStartup (GR) status: \tGood Golden References stored\n"));
    _TPRINTF(PrintBuffer);
    break;
    
  case eCouldNotGet:
  default:
    _stprintf(PrintBuffer, _T("maXStartup (GR) status: \tCould not get Golden References\n"));
    _TPRINTF(PrintBuffer);
  }
}

BOOL DisplayChecksum(BOOL InMemory)
{
  UINT32 checksum;
  BOOL bError = FALSE;
  if (!ReadMemoryMap()) return FALSE;

  if (!ValidateDevice()) {
    _stprintf(PrintBuffer, _T("Please verify that you have the correct Windows driver installed (You can use Device Manager).\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  if (InMemory) {
    if (CalculateInFWChecksum(&checksum)) {
      _stprintf(PrintBuffer, _T("\nConfiguration in-memory checksum is: 0x%06X\n"), checksum);
      _TPRINTF(PrintBuffer);
    } else {
      _stprintf(PrintBuffer, _T("\nCannot calculate in-memory checksum.\n"));
      _TPRINTF(PrintBuffer);
      bError = TRUE;
    }
  } else {
    if (ReadNVMChecksum(&checksum)) {
      _stprintf(PrintBuffer, _T("\nConfiguration checksum in NVM: 0x%06X\n"), checksum);
      _TPRINTF(PrintBuffer);
    } else {
      _stprintf(PrintBuffer, _T("\nCannot calculate NVM configuration checksum.\n"));
      _TPRINTF(PrintBuffer);
      bError = TRUE;
    }
  }
  if (bError) return FALSE;
  return TRUE;
}

// This function verifies if the Memory Map data read is reliable.
// In some case (when moving from FW not supporting Inbox to fully compliant FW)
// it reads some zeros because Windows driver is not installed
BOOL ValidateDevice()
{
  return (DeviceMemoryMap.FamilyID && DeviceMemoryMap.NumberOfObjects);
}

//
// Added by Miracle.
//

BOOL USBCMD(int wlen, UINT8 *wbuf, int rlen, UINT8 *rbuf)
{
  UINT8 writeBuffer[128];
  UINT8 readBuffer[128];

  BOOL bReturncode;
  DWORD bytesWritten;
  DWORD bytesRead;

  int i;

  ZeroMemory(writeBuffer, 65);
  if(wlen > 0) {
    //writeBuffer[0] = AtmelReportId;
#if FALSE
    DBGPRINT(_T("USBCMD: 1. Write data: "));
    for(int i=0; i<65; i++) {
      writeBuffer[i] = wbuf[i];
      DBGPRINT(_T("%02X "), writeBuffer[i]);
    }
#else
    for(i=0; i<65; i++) {
      writeBuffer[i] = wbuf[i];
    }
#endif
    bReturncode = WriteFile (mXTDevice.HidDevice,
      writeBuffer,
      mXTDevice.OutputReportByteLength,
      &bytesWritten,
      NULL) && (bytesWritten == mXTDevice.OutputReportByteLength);
    if (!bReturncode) {
      DBGPRINT(_T("USBCMD: 1. Error on first WriteFile, code = %d\n"), GetLastError());
      return FALSE;
    }
  }
// This optimization leads to "No device found" // Commented out by Marco 2014/11/13
//  if (rlen <= 0)  // Miracle. 2014/06/25. If no response required, return it.
//    return TRUE;

  for(i = 0; i < 10; i++) {
    bReturncode = ReadFile(mXTDevice.HidDevice,
      readBuffer,
      mXTDevice.InputReportByteLength,
      &bytesRead,
      NULL);

  if (!bReturncode) {
    DBGPRINT(_T("USBCMD: 2. Error on first WriteFile, code = %d\n"), GetLastError());
    return FALSE;
  }
    if(readBuffer[0] == AtmelReportId /*&& (readBuffer[1]==writeBuffer[1])*/ )
      break;

    DBGPRINT(_T("USBCMD: 3. Data not come. Wait for 200 ms\n"));
    Sleep(100);   // Miracle's change from 200
  }
  if(i == 10) {
    return FALSE;
  }
#if FALSE
  DBGPRINT(_T("\nUSBCMD: Read data: "));
  for(i=0; i<65 && i<rlen ; i++) {
    rbuf[i] = readBuffer[i];
    DBGPRINT(_T("%02X "), readBuffer[i]);
  }
  DBGPRINT(_T("\nAll above data.\n"));
#else
  for(i=0; i<65 && i<rlen ; i++) {
    rbuf[i] = readBuffer[i];
  }
#endif

  return TRUE;

}

BOOL I2CBridge_ReadWrite_Physical(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf) {

  UINT8 bufw[65];
  UINT8 bufr[65];


  ZeroMemory(bufw, 65);
  int idx = 0;
  bufw[idx++] = AtmelReportId;
  for(int i=0; i<wlen; i++)
    bufw[idx++] = wbuf[i];

  if(! USBCMD(idx, bufw, 65, bufr))
    return FALSE;

  if(bufr[0] != AtmelReportId)
    return FALSE;

  for(int i=0; i<rlen; i++)
    rbuf[i] = bufr[i];

  return TRUE;

}

BOOL I2CBridge_I2C_ReadWrite(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf)
{
  BOOL ret;
  UINT8 bufw[65];
  UINT8 bufr[65];

  UINT8 idx = 0;
  bufw[idx++] = 0x51;
  bufw[idx++] = wlen;
  bufw[idx++] = rlen;
  if (wbuf != NULL) {
    for(int i=0; i<wlen; i++) {
      bufw[idx++] = wbuf[i];
    }
  }
  ret = I2CBridge_ReadWrite_Physical(idx, bufw, rlen+3, bufr);

  if(! ret)
    return FALSE;

//  if(bufr[1] != 0x04 && bufr[1] != 0x00)
  if ((bufr[1]&0x03) != 0)
    return FALSE;

  // return i2c data.
  if(rlen > 0) {
    for(int i=0; i<rlen; i++)
      rbuf[i] = bufr[i+3];
  }

  return TRUE;
}


BOOL I2CBridge_HIDI2C_ReadWrite(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf)
{
  BOOL ret;
  UINT8 bufw[65];
  UINT8 bufr[65];
  //UINT8 idx = 0;
  bufw[0] = HIDHEADER[HIDDSC_OutputRegister_LSB]; // HID Output register.
  bufw[1] = HIDHEADER[HIDDSC_OutputRegister_MSB]; // HID output register.
  bufw[2] = HIDHEADER[HIDDSC_OutputLength_LSB]; // HID length.
  bufw[3] = HIDHEADER[HIDDSC_OutputLength_MSB]; // HID length.
  bufw[4] = ATMEL_REPORT_ID_I2C;  // Report ID.
  bufw[5] = 0x51;       // Atmel report ID.
  bufw[6] = wlen;
  bufw[7] = rlen;
  for(int i=0; i<wlen; i++) {
    bufw[i+8] = wbuf[i];
  }
  bufw[2] = 6 + wlen;

  ret = I2CBridge_I2C_ReadWrite(wlen+8, bufw, rlen, bufr);

  if(! ret)
    return FALSE;

  // return i2c data.
  if(rlen > 0) {
    for(int i=0; i<rlen; i++)
      rbuf[i] = bufr[i+3];
  }

  return TRUE;
}



// If data over 15 bytes, need to call this function to split the data.
BOOL I2CBridge_HIDI2C_Write(UINT16 addr, UINT16 wlen, UINT8 *wbuf)
{
  BOOL ret;
  UINT8 bufw[65];
  UINT8 len;
  int idx = 0;

  for(;wlen > 0;) {
    if(wlen > 12)
      len = 12;
    else
      len = (UINT8)wlen;

    ZeroMemory(bufw, 65);
    idx = 0;
    bufw[idx++] = (UINT8)addr & 0xFF;
    bufw[idx++] = (UINT8)((addr>>8) & 0xFF);
    for(int i=0; i<len; i++) {
      bufw[idx++] = wbuf[0];
      wbuf++;
      wlen--;
    }

    ret = I2CBridge_HIDI2C_ReadWrite(len+2, bufw, 0, NULL);
    if(! ret)
      return FALSE;

    addr += len;
  }

  return TRUE;
}


BOOL I2CBridge_HIDI2C_CleanMsg()
{
  UINT8 bufw[65];
  UINT8 bufr[65];
  UINT8 buf_backup[65];
  //BOOL isSame;
  UINT8 idx;

  if(!ReadMessage(bufr))
    return FALSE;

  ZeroMemory(bufw, 65);
  idx = 0;
  bufw[idx++] = HIDHEADER[HIDDSC_OutputRegister_LSB];
  bufw[idx++] = HIDHEADER[HIDDSC_OutputRegister_MSB];
  bufw[idx++] = 0x04;
  bufw[idx++] = 0x00;
  bufw[idx++] = 0x06;
  bufw[idx++] = 0x88;
  if(! I2CBridge_I2C_ReadWrite(idx, bufw, 0, NULL))
    return FALSE;

  for(int i=0; i<65; i++)
    buf_backup[i] = 0xFF;
  for(int i=0; i<10; i++) {
    if(!ReadMessage(bufr))
      return FALSE;
#if FALSE
    isSame = TRUE;
    for(int j=0; j<65; j++) {
      if(buf_backup[j] != rbuf[j])
        isSame = FALSE;
      buf_backup[j] = rbuf[j];
    }
    if(isSame)
      break;
#endif

  }
  return TRUE;
}


BOOL I2CBridge_ReadWriteNonStop(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf) {
  UINT8 bufr[65];

  if(!Protocol_HID || DeviceDiscovered == I2C_BOOTLOADER) {
    return I2CBridge_I2C_ReadWrite(wlen, wbuf, rlen, rbuf);
  }

  if(! I2CBridge_HIDI2C_ReadWrite(wlen, wbuf, rlen, bufr))
    return FALSE;
  for(int i=0; i<rlen; i++)
    rbuf[i] = bufr[i];

  return TRUE;
}


BOOL I2CBridge_ReadWrite(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf) {
  UINT8 bufw[65];
  UINT8 bufr[65];
  //UINT16 addr;

  UINT8 idx = 0;

  // Native mode support. No HID stack.
  if(!Protocol_HID || DeviceDiscovered == I2C_BOOTLOADER) {
    return I2CBridge_I2C_ReadWrite(wlen, wbuf, rlen, rbuf);
  }

  ZeroMemory(bufw, 65);
  idx = 0;
  bufw[idx++] = HIDHEADER[HIDDSC_OutputRegister_LSB];   // Output register.
  bufw[idx++] = HIDHEADER[HIDDSC_OutputRegister_MSB];
  bufw[idx++] = 6 + wlen;  // HIDHEADER[HIDDSC_OutputLength_LSB];   // Output length.
  bufw[idx++] = HIDHEADER[HIDDSC_OutputLength_MSB];
  bufw[idx++] = ATMEL_REPORT_ID_I2C;  // Report ID.
  bufw[idx++] = 0x51;         // Report ID.
  bufw[idx++] = wlen;
  bufw[idx++] = rlen;

  for(int i=0; i<wlen; i++) {
    bufw[idx++] = wbuf[i];
  }

  if(! I2CBridge_I2C_ReadWrite(idx, bufw, 0, NULL))
    return FALSE;

  if(rlen > 0) {
    //if(! ReadMessage(bufr)) // Miracle. 2014/05/23. When try to read object data. Sometimes other objects message will be received.
  if (!BridgeBoard_HIDI2C_ReadData(bufr))
      return FALSE;
    for(int i=0; i<rlen; i++)
      rbuf[i] = bufr[i];
  }
  return TRUE;
}

BOOL I2CBridge_SetAddr(UINT8 addr)
{
  UINT8 wbuf2[] = { 0x80, 0x30, 0x4A, 0x00, 0x80, 0x00, 0x00, 0xC8, 0x40, 0x54, 0x00, 0x00};
  wbuf2[2] = addr | 0x80;

  if( !I2CBridge_ReadWrite_Physical(sizeof(wbuf2), wbuf2, 0, NULL)) {
    DBGPRINT(_T("\nI2CBridge_SetAddr: Error command to bridge board. Code = %d\n"), GetLastError());
    return FALSE; 
  }
  return TRUE;
}

UINT8 I2CBridge_FindAddr()
{
  UINT8 readBuffer[65];
  int i;

  for(i=0; i<sizeof(i2c_addresses); i++) {
    // Set i2c address.
    if(! I2CBridge_SetAddr(i2c_addresses[i]))
      return 0;

    // Find device address should always use bridge board native protocol.
    if(I2CBridge_I2C_ReadWrite(0, NULL, 3, readBuffer)) {
      //DBGPRINT(_T("\nReadMemoryMap_BridgeBoard: 5 Data: "));
      _stprintf(PrintBuffer, _T("Selected slave Addr: %02X\n"), i2c_addresses[i]);
      DBGPRINT(PrintBuffer);
      break;
    }
  }

  if(i==sizeof(i2c_addresses))
    return 0;

  DBGPRINT(_T("\nI2CBridge_FindAddr: Find device address = 0x%02X\n"), i2c_addresses[i]);
  i2c_address_index = i;

  return i2c_addresses[i];
}

BOOL ReadMemoryMap_BridgeBoard()
{
  struct MemoryMap *ptrMemoryMap;
  struct ObjectTableElement *ptrObjectTableElement;
  struct ObjectTableElement *pObjectTableElement;
  ULONG ulBufferSize;
  ULONG ulTotalObjects;
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];
  UINT8 *pBuffer;
  DWORD bytesToBeRead;
  UINT8 i2cAddress = 0;

//  DWORD bytesWritten;
//  DWORD bytesRead;
  //BOOL bReturncode;
  UINT8 numReportIDs = 1, lastReportID = 1;
  UINT i, k;
  UINT8 idx = 0;

  // Make sure that we clean object vector from previous calls...
  if (AllObjects.size()) {
    vector<ObjectTableElement*>::const_iterator current = AllObjects.begin();
    while (current != AllObjects.end()) {
      delete (*current);
      ++current;
    }
    AllObjects.clear();
  }

  HandleDeviceRead = mXTDevice.HidDevice;

  // Added by Miracle.
  // Reset i2c bridge board.
  UINT8 wbuf1[] = { AtmelReportId, 0x86, 0x00, 0x00, 0x00};
  if(! USBCMD(sizeof(wbuf1), wbuf1, 0, NULL)) {
//  if (!USBCMD(sizeof(wbuf1), wbuf1, 65, readBuffer)) {    // Miracle. 2014/06/25. Fix change with 5055HV board support.
    DBGPRINT(_T("\nReadMemoryMap_BridgeBoard: 1. Error on first WriteFile, code = %d\n"), GetLastError());
    return FALSE;
  }
  // Set i2c address.
  i2cAddress = I2CBridge_FindAddr();
  if (i2cAddress < 0x40) {
    // In bootloader mode
    DeviceDiscovered = I2C_BOOTLOADER;
    return TRUE;
  }
  else {
    DeviceDiscovered = USB_NORMAL;
  }
  Sleep(50);   // Miracle's change from 100

  // Added by Miracle.
  ZeroMemory(writeBuffer, 65);
  idx = 0;
    writeBuffer[idx++] = 0x00;    // 7 bytes description
    writeBuffer[idx++] = 0x00;    // 7 bytes description
  if(!I2CBridge_I2C_ReadWrite(idx, writeBuffer, 0x1E, readBuffer))
    return FALSE;

  if(readBuffer[0] == 0x1E && readBuffer[1] == 0x00) {
    // This is HIDI2C over bridge board.
    Protocol_HID = TRUE;
    for(i=0; i < 0x1E; i++) {
      HIDHEADER[i] = readBuffer[i];
    }

//    I2CBridge_HIDI2C_CleanMsg();  // clean message buffer in chip to prevent mistake.
    _stprintf(PrintBuffer, _T("Protocol: HID\n"));
    DBGPRINT(PrintBuffer);
  }
  else {
    Protocol_HID = FALSE;
    _stprintf(PrintBuffer, _T("Protocol: OBP\n"));
    DBGPRINT(PrintBuffer);
  }

  // read 7 bytes.
  //DBGPRINT(_T("\nReadMemoryMap_BridgeBoard: read 7 bytes from object table\n"));
  writeBuffer[0] = 0;
  writeBuffer[1] = 0;
  if(! I2CBridge_ReadWrite(2, writeBuffer, 7, readBuffer)) {
    DBGPRINT(_T("\nReadMemoryMap_BridgeBoard: 5 Data: "));
    return FALSE;
  }
  else {
    DBGPRINT(_T("Object table: "));
    for(i=0; i < 7; i++) {
      DBGPRINT(_T("%02X "), readBuffer[i]);
    }
    DBGPRINT(_T("\n"));
  }

  // Added by Miracle.
    ptrMemoryMap = (MemoryMap *)&readBuffer[3];
    ulBufferSize = readBuffer[2];
    // Save values read
    DeviceMemoryMap.FamilyID        = readBuffer[0];
    DeviceMemoryMap.VariantID       = readBuffer[1];
    DeviceMemoryMap.Version         = readBuffer[2];
    DeviceMemoryMap.Build           = readBuffer[3];
    DeviceMemoryMap.MatrixSizeX     = readBuffer[4];
    DeviceMemoryMap.MatrixSizeY     = readBuffer[5];
    DeviceMemoryMap.NumberOfObjects = readBuffer[6];

    ptrObjectTableElement = &ptrMemoryMap->Element1;
    // Set the global variable NumberOfObjects
    NumberOfObjects = DeviceMemoryMap.NumberOfObjects;
    // and the local...
    ulTotalObjects = DeviceMemoryMap.NumberOfObjects;
  
  bytesToBeRead = NumberOfObjects*sizeof(ObjectTableElement);
    pBuffer = new UINT8[bytesToBeRead];
  ptrObjectTableElement = (struct ObjectTableElement *)pBuffer;

  for (i = 0; i < DeviceMemoryMap.NumberOfObjects; i++, ptrObjectTableElement++) {

    writeBuffer[0] = (7+i*6) & 0xFF;
    writeBuffer[1] = ((7+i*6)>>8) & 0xFF;
    if(! I2CBridge_ReadWrite(2, writeBuffer, 7, readBuffer)) {
      DBGPRINT(_T("\nReadMemoryMap_BridgeBoard: 5 Data: "));
      delete[] pBuffer;
      return FALSE;
    }
    // DBGPRINT(_T("objet table: %d\n"), readBuffer[0]);
    pObjectTableElement = new ObjectTableElement;
    pObjectTableElement->Type  = readBuffer[0];
    pObjectTableElement->LSB   = readBuffer[1];
    pObjectTableElement->MSB   = readBuffer[2];
    pObjectTableElement->Size  = readBuffer[3];
    pObjectTableElement->Instances = readBuffer[4];
    pObjectTableElement->NumberOfReportIDs = readBuffer[5];
    if (pObjectTableElement->NumberOfReportIDs) {
      numReportIDs = (pObjectTableElement->Instances+1)*pObjectTableElement->NumberOfReportIDs;
      for (k = 0; k < numReportIDs; ++k) {
        ReportIds[lastReportID + k] = pObjectTableElement->Type;
      }
      lastReportID += (UINT8)k;
    }
    AllObjects.push_back(pObjectTableElement);
    for (UINT index = 0;; ++index) {
      if (TouchSelfCapObjects[index] == 0) break;
      if (TouchSelfCapObjects[index] == pObjectTableElement->Type) {
        TouchSelfCapTotalInstances++;
        TouchSelfCapTotalInstances += pObjectTableElement->Instances;
        break;
      }
    }
  }
  delete[] pBuffer;

  // Read the info block checksum
  ZeroMemory(writeBuffer, 65);
  writeBuffer[0] = (7+i*6) & 0xFF;
  writeBuffer[1] = ((7+i*6)>>8) & 0xFF;
  if(! I2CBridge_ReadWrite(2, writeBuffer, 3, readBuffer)) {
    DBGPRINT(_T("\nReadMemoryMap_BridgeBoard: 5 Data: "));
    return FALSE;
  }
  InfoBlockChecksum = readBuffer[0] | ((readBuffer[1] << 8) & 0xFF00) | ((readBuffer[2] << 16) & 0xFF0000);

  IdentifyDevice();

  return TRUE;
}

//
// End of code. Miracle.
//

BOOL ReadMemoryMap(BOOL ReadAlways)
{
  struct MemoryMap *ptrMemoryMap;
  struct ObjectTableElement *ptrObjectTableElement, *pObjectTableElement;
  ULONG ulBufferSize;
  ULONG ulTotalObjects;
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];
  UINT8 *pBuffer;
  DWORD bytesToBeRead, bytesWritten, bytesRead;
  UINT8 numReportIDs = 1, lastReportID = 1;
  UINT k;

  if (!ReadAlways && AllObjects.size()) {
    if (HandleDeviceRead == mXTDevice.HidDevice) {
      return TRUE;
    }
  }
  HandleDeviceRead = mXTDevice.HidDevice;
  // Reset the counter for object T25
  TouchSelfCapTotalInstances = 0;

  // Added by Miracle.
  if (ConnectFWType == I2CBRIDGE) { 
    return ReadMemoryMap_BridgeBoard();
  }

  // Make sure that we clean object vector from previous calls...
  if (AllObjects.size()) {
    vector<ObjectTableElement*>::const_iterator current = AllObjects.begin();
    while (current != AllObjects.end()) {
      delete (*current);
      ++current;
    }
    AllObjects.clear();
  }

  if (ConnectFWType == HIDI2CNOINBOX || ConnectFWType == I2CSTOCK) {
    if (!InitI2CNormal(TRUE)) {
      return FALSE;
    }
    bytesRead = ReadWithRetryFromMxt(hI2CDriver, 0, 7, (UINT8 *)&DeviceMemoryMap);
    if (bytesRead != 7) {
      return FALSE;
    }
    NumberOfObjects = DeviceMemoryMap.NumberOfObjects;

    bytesToBeRead = NumberOfObjects*sizeof(ObjectTableElement);
    pBuffer = new UINT8[bytesToBeRead];

    bytesRead = ReadWithRetryFromMxt(hI2CDriver, 7, (UINT16)bytesToBeRead, pBuffer);
    if (bytesRead != bytesToBeRead) {
      DBGPRINT(_T("Cannot read device all objects. Read returned %d\n"), bytesRead);
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        DBGPRINT(_T("\nPermanent error: check if MxtBridge is correctly installed. You may also try to power-cycle.\n\n"));
      }
      return FALSE;
    }

    ptrObjectTableElement = (struct ObjectTableElement *)pBuffer;
    for (UINT i=0; i < NumberOfObjects; ++i, ptrObjectTableElement++) {
      pObjectTableElement = new ObjectTableElement;
      pObjectTableElement->Type  = ptrObjectTableElement->Type;
      pObjectTableElement->LSB   = ptrObjectTableElement->LSB;
      pObjectTableElement->MSB   = ptrObjectTableElement->MSB;
      pObjectTableElement->Size  = ptrObjectTableElement->Size;
      pObjectTableElement->Instances = ptrObjectTableElement->Instances;
      pObjectTableElement->NumberOfReportIDs = ptrObjectTableElement->NumberOfReportIDs;

      if (pObjectTableElement->NumberOfReportIDs) {
        numReportIDs = (pObjectTableElement->Instances+1)*pObjectTableElement->NumberOfReportIDs;
        for (k = 0; k < numReportIDs; ++k) {
          ReportIds[lastReportID + k] = pObjectTableElement->Type;
        }
        lastReportID += (UINT8)k;
      }
      AllObjects.push_back(pObjectTableElement);
      for (UINT index = 0;; ++index) {
        if (TouchSelfCapObjects[index] == 0) break;
        if (TouchSelfCapObjects[index] == pObjectTableElement->Type) {
          TouchSelfCapTotalInstances++;
          TouchSelfCapTotalInstances += pObjectTableElement->Instances;
          break;
        }
      }
    }
    delete[] pBuffer;
  } else {

    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;
    writeBuffer[1] = 0x51;
    writeBuffer[2] = 0x02;
    writeBuffer[3] = 7;    // 7 bytes description

    // Replaced by Miracle. Read failures seen in HP Cedar 1.1
    if(! ReadWriteDevice_InboxDriver(writeBuffer, &bytesWritten, readBuffer, &bytesRead, 0)) {
      return FALSE;
    }

    ptrMemoryMap = (MemoryMap *)&readBuffer[3];
    ulBufferSize = readBuffer[2];
    // Save values read
    DeviceMemoryMap.FamilyID        = readBuffer[3];
    DeviceMemoryMap.VariantID       = readBuffer[4];
    DeviceMemoryMap.Version         = readBuffer[5];
    DeviceMemoryMap.Build           = readBuffer[6];
    DeviceMemoryMap.MatrixSizeX     = readBuffer[7];
    DeviceMemoryMap.MatrixSizeY     = readBuffer[8];
    DeviceMemoryMap.NumberOfObjects = readBuffer[9];

    ptrObjectTableElement = &ptrMemoryMap->Element1;
    // Set the global variable NumberOfObjects
    NumberOfObjects = readBuffer[9];
    // and the local...
    ulTotalObjects = readBuffer[9];

    ULONG j = 0;
    UINT8 last_type = 0xFF;   // Miracle. 2014/06/14. Sometimes read too fast to inbox driver. It will return duplicate package and cause failure in application. Use this variable to detect failure.
    int cnt=0;                // Miracle. 2014/06/14. Error retry
    do {
      ZeroMemory(writeBuffer, 65);
      writeBuffer[0] = AtmelReportId;
      writeBuffer[1] = 0x51;
      writeBuffer[2] = 0x02;
      writeBuffer[3] = 6;
      // Now set the address where to read
      USHORT address = (USHORT)(7 + j*6);
      writeBuffer[4] = address & 0x00FF;
      writeBuffer[5] = (address>>8) & 0x00FF;

      // Replaced by Miracle. Read failures seen in HP Cedar 1.1
      if(!ReadWriteDevice_InboxDriver(writeBuffer, &bytesWritten, readBuffer, &bytesRead, 0)) {
        return FALSE;
      }
      ptrObjectTableElement = (ObjectTableElement *)&readBuffer[3];
      // Miracle. 2014/06/14. Sometimes host will return wrong object data. check it is duplicate to prevent error.
      if (ptrObjectTableElement->Type == last_type) {
        cnt++;
        if(cnt < 5)
          continue;
      }
      last_type = ptrObjectTableElement->Type;
      cnt = 0;

      pObjectTableElement = new ObjectTableElement;
      pObjectTableElement->Type  = ptrObjectTableElement->Type;
      pObjectTableElement->LSB   = ptrObjectTableElement->LSB;
      pObjectTableElement->MSB   = ptrObjectTableElement->MSB;
      pObjectTableElement->Size  = ptrObjectTableElement->Size;
      pObjectTableElement->Instances = ptrObjectTableElement->Instances;
      pObjectTableElement->NumberOfReportIDs = ptrObjectTableElement->NumberOfReportIDs;

      if (pObjectTableElement->NumberOfReportIDs) {
        numReportIDs = (pObjectTableElement->Instances+1)*pObjectTableElement->NumberOfReportIDs;
        for (k = 0; k < numReportIDs; ++k) {
          ReportIds[lastReportID + k] = pObjectTableElement->Type;
        }
        lastReportID += (UINT8)k;
      }
      AllObjects.push_back(pObjectTableElement);
      for (UINT index = 0;; ++index) {
        if (TouchSelfCapObjects[index] == 0) break;
        if (TouchSelfCapObjects[index] == pObjectTableElement->Type) {
          TouchSelfCapTotalInstances++;
          TouchSelfCapTotalInstances += pObjectTableElement->Instances;
          break;
        }
      }
      ++j;
      ++ptrObjectTableElement;
      // remaining objects
      --ulTotalObjects;
    }
    while(ulTotalObjects);

    // Read the info block checksum
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;
    writeBuffer[1] = 0x51;
    writeBuffer[2] = 0x02;
    writeBuffer[3] = 3;
    // Now set the address where to read
    USHORT address = (USHORT)(7 + j*6);
    writeBuffer[4] = address & 0x00FF;
    writeBuffer[5] = (address>>8) & 0x00FF;

    // Replaced by Miracle. Read failures seen in HP Cedar 1.1
  if(!ReadWriteDevice_InboxDriver(writeBuffer, &bytesWritten, readBuffer, &bytesRead, 0)) {
    return FALSE;
  }
    InfoBlockChecksum = readBuffer[3] | ((readBuffer[4] << 8) & 0xFF00) | ((readBuffer[5] << 16) & 0xFF0000);
    // done
  }

  IdentifyDevice();
  HandleDeviceRead = mXTDevice.HidDevice;

  return TRUE;
}

void IdentifyDevice()
{
  int iRetCode;
  UNREFERENCED_PARAMETER(iRetCode);
  string name;
  // Set device as unknown first...
  DeviceConnected = eUnrecognized;

#ifdef AUTO_UPDATER
    DeviceAppFromXML = FALSE;
#else
  iRetCode = GetDeviceAppName(DeviceMemoryMap.FamilyID, DeviceMemoryMap.VariantID, name);
  // check if success...
  if (!iRetCode) {
    _tcscpy_s(DeviceAppNameFromXML, CA2T(name.c_str()));
    DeviceAppFromXML = TRUE;  // This is a flag that tells that DeviceAppNameFromXML[] holds a valid name
  } else {
    DeviceAppFromXML = FALSE;
  }
#endif

  switch(DeviceMemoryMap.FamilyID) {
    case 0x32:
      switch(DeviceMemoryMap.VariantID) {
        case 0:
        case 1:
          DeviceConnected = eMXTS200;
          break;
        case 2:
          DeviceConnected = eMXTS220;
          break;
      }
      break;

    case 0x81:
      switch(DeviceMemoryMap.VariantID) {
        case 0:
        case 1:
        case 18:
        case 20:
        case 21:
        case 23:
        case 24:
        case 28:
          DeviceConnected = eMXT224E;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
          break;
        case 19:
        case 25:
        case 27:
          DeviceConnected = eMXT112E;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
          break;
      }
      break;

    case 0x82:
      switch(DeviceMemoryMap.VariantID) {
        case 19:
          DeviceConnected = eMXT112S;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
          break;
        case 1:
        case 24:
        case 26:
        case 29:
        case 31:
        case 34:
        case 36:
        case 37:
        case 43:
        case 46:
        case 50:
        case 55:
        case 56:
        case 60:
          DeviceConnected = eMXT224S;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
          break;
        case 23:
        case 25:
        case 27:
        case 30:
        case 32:
        case 33:
        case 35:
        case 38:
        case 40:
        case 41:
        case 42:
        case 47:
        case 48:
        case 49:
        case 53:
        case 54:
        case 58:
        case 59:
        case 61:
          DeviceConnected = eMXT336S;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
          break;
      case 28:
      case 44:
      case 45:
      case 52:
        DeviceConnected = eMXT540S;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
        break;
      case 51:
        DeviceConnected = eMXTP540S;
          UniqueIDPage = 1;
          UniqueIDStart = 8;
          UniqueIDSize = 18;
          UniqueIDRevIDPage = 0;
        break;
      }
      break;

    case 0xA0:
      switch(DeviceMemoryMap.VariantID) {
        case 0:
        case 8:
          DeviceConnected = eMXT1386;
          break;
        case 2:
        case 3:
        case 11:
        case 12:
        case 7:
        case 9:
          if (DeviceMemoryMap.MatrixSizeY == 42) {
            DeviceConnected = eMXT1386E;
          } else {
            DeviceConnected = eMXT1716E;
          }
          break;
        case 10:
        case 13:
        case 14:
          DeviceConnected = eMXT3432S;
          UniqueIDPage = 4;
          UniqueIDStart = 4;
          UniqueIDSize = 16;
          UniqueIDRevIDPage = 0;
          UniqueIDRevIDStart = 19;
          break;
      }
      break;

    case 0xA1:
      switch(DeviceMemoryMap.VariantID) {
        case 0x02:
          DeviceConnected = eMXT768E;
          break;
      }
      break;

    case 0xA2:
      switch(DeviceMemoryMap.VariantID) {
        case 1:
        case 4:
        case 6:
        case 9:
        case 15:
        case 17:
        case 19:
        case 22:
          DeviceConnected = eMXT1188S;
          UniqueIDPage = 2;
          UniqueIDStart = 4;
          UniqueIDSize = 16;
          UniqueIDRevIDPage = 0;
          UniqueIDRevIDStart = 19;
          break;
        default:
          DeviceConnected = eMXT1664S;
          UniqueIDPage = 2;
          UniqueIDStart = 4;
          UniqueIDSize = 16;
          UniqueIDRevIDPage = 0;
          UniqueIDRevIDStart = 19;
          break;
      }
      break;

    case 0xA3:
      DeviceConnected = eMXF0100i;
      break;

    case 0xA4:
      switch(DeviceMemoryMap.VariantID) {
        case 0:
        case 6:
          DeviceConnected = eMXT2952T;
          UniqueIDPage = 6;
          break;
        case 1:
        case 10:
          DeviceConnected = eMXT1664T;
          UniqueIDPage = 6;
          break;
        case 2:
          DeviceConnected = eMXT640T;
          UniqueIDPage = 6;
          break;
        case 3:
          DeviceConnected = eMXT2952T2;
          UniqueIDPage = 6;
          break;
        case 4:
        case 19:
          DeviceConnected = eMXT1664T2;
          UniqueIDPage = 6;
          break;
        case 5:
        case 21:
          DeviceConnected = eMXT336T;
          UniqueIDPage = 6;
          break;
        case 7:
          DeviceConnected = eMXT641T;
          UniqueIDPage = 6;
          break;
        case 8:
          DeviceConnected = eMXT2405T;
          UniqueIDPage = 6;
          break;
        case 9:
        case 22:
          DeviceConnected = eMXT224T;
          UniqueIDPage = 6;
          break;
        case 11:
        case 12:
          DeviceConnected = eMXT1066T2;
          UniqueIDPage = 6;
          break;
        case 13:
          DeviceConnected = eMXT2954T2;
          UniqueIDPage = 6;
          break;
        case 14:
          DeviceConnected = eMXT2405T2;
          UniqueIDPage = 6;
          break;
        case 15:
          DeviceConnected = eMXT2407T2;
          UniqueIDPage = 6;
          break;
        case 16:
        case 20:
          DeviceConnected = eMXT1666T2;
          UniqueIDPage = 6;
          break;
        case 17:
          DeviceConnected = eMXT337T;
          UniqueIDPage = 6;
          break;
        case 18:
          DeviceConnected = eMXT225T;
          UniqueIDPage = 6;
          break;
        case 23:
          DeviceConnected = eMXT336TO;
          UniqueIDPage = 6;
          break;
        case 24:
          DeviceConnected = eMXT641T_AT;
          UniqueIDPage = 6;
          break;
        case 25:
          DeviceConnected = eMXT449T_AT;
          UniqueIDPage = 6;
          break;
        case 26:
          DeviceConnected = eMXT641TP_AT;
          UniqueIDPage = 6;
          break;
        case 27:
          DeviceConnected = eMXT449TP_AT;
          UniqueIDPage = 6;
          break;
        case 28:
          DeviceConnected = eMXT1068T2;
          UniqueIDPage = 6;
          break;
        case 29:
          DeviceConnected = eMXT1667T2;
          UniqueIDPage = 6;
          break;
        case 30:
          DeviceConnected = eMXT1668T2;
          UniqueIDPage = 6;
          break;
        case 31:
          DeviceConnected = emXT1664T3;
          UniqueIDPage = 6;
          break;
        case 32:
          DeviceConnected = emXT1668T2C01;
          UniqueIDPage = 6;
          break;
        case 33:
          DeviceConnected = emXT225T_AT;
          UniqueIDPage = 6;
          break;
        case 34:
          DeviceConnected = emXT643T;
          UniqueIDPage = 6;
          break;
        case 35:
          DeviceConnected = emXT2952T2C01;
          UniqueIDPage = 6;
          break;
        case 36:
          DeviceConnected = emXT2954TO;
          UniqueIDPage = 6;
          break;
        case 37:
          DeviceConnected = emXT1665TAT;
          UniqueIDPage = 6;
          break;
        case 38:
          DeviceConnected = emXT1664T2C2UC01;
          UniqueIDPage = 6;
          break;
		case 55:
		  DeviceConnected = emXT2952TM;
		  UniqueIDPage = 6;
		  break;
      }
      break;
    case 0xA5:
      DeviceConnected = eMXC576T;
      UniqueIDPage = 6;
      break;
    case 0xA6:
      DeviceConnected = eMXT874U;
      UniqueIDPage = 6;
      break;
    }
  DeviceAlreadyIdentified = TRUE;
}

BOOL ZeroConfigMemory()
{
  UINT16 address;
  UINT8 size;
  UINT8 buffer[256];
  DWORD bytesToWrite;
  UINT i, j;

  ZeroMemory(buffer, 256);

  if (!ReadMemoryMap()) return FALSE;
  for (i = 0; i < NumberOfObjects; i++) {
    if (AllObjects[i]->Type == 0) {
      // No more objects
      break;
    }
    if (!IsObjectConfigurable(i) || AllObjects[i]->Type == DEBUG_DIAGNOSTIC_T37) {
      continue;
    }

  // 2014/11/07. Miracle. Fix bug in zero configure. Original code will write to the same address for differnece instance.
  address = 0;
  address = AllObjects[i]->LSB;
  address |= (AllObjects[i]->MSB << 8) & 0xFF00;
  size = AllObjects[i]->Size + 1;
  bytesToWrite = size;
  for (j = 0; j <= AllObjects[i]->Instances; ++j) {
    // Added by Miracle.
    if (ConnectFWType == I2CBRIDGE && Protocol_HID == TRUE) {
      if(! I2CBridge_HIDI2C_Write(address, size, buffer)) {
        DBGPRINT(_T("ZeroConfigMemory: Call I2CBridge_Write_HIDI2C failure\n"));
        return FALSE;
      }
      address += size;
    }
    else if (ConnectFWType == HIDUSB || ConnectFWType == HIDI2C || (ConnectFWType == I2CBRIDGE && Protocol_HID == FALSE)) {
      if (!WriteToDevice(address, buffer, &bytesToWrite)) {
        DBGPRINT(_T("Error clearing config memory on device, code = %d\n"), GetLastError());
        return FALSE;
      }
      address += (UINT16)bytesToWrite;
    }
    else {
      if (!WriteWithRetryToMxt(hI2CDriver, address, (UINT16)size, buffer)) {
        DBGPRINT(_T("Error clearing config memory on device, code = %d\n"), GetLastError());
        return FALSE;
      }
      address += size;
    }
    }
  }
  return TRUE;
}

BOOL GetObjectAddressAndSize(UINT8 Type, UINT16 *Address, UINT8 *Size)
{
  if (!ReadMemoryMap()) return FALSE;

  for (UINT i = 0; i < NumberOfObjects; i++) {
    if (AllObjects[i]->Type == Type) {
      *Address = 0;
      *Address = AllObjects[i]->LSB;
      *Address |= (AllObjects[i]->MSB<<8) &0xFF00;
      if (Size) *Size = AllObjects[i]->Size + 1;
      return TRUE;
    }
  }
  return FALSE;
}

BOOL GetObjectIndex(UINT8 Type, UINT8 *Index)
{
  if (!ReadMemoryMap()) return FALSE;

  for (UINT i = 0; i < NumberOfObjects; i++) {
    if (AllObjects[i]->Type == Type) {
      *Index = (UINT8)i;
      return TRUE;
    }
  }
  return FALSE;
}

UINT8 GetObjectSize(UINT8 Type)
{
  if (!ReadMemoryMap()) return FALSE;

  for (UINT i = 0; i < NumberOfObjects; i++) {
    if (AllObjects[i]->Type == Type) {
      return(AllObjects[i]->Size + 1);
    }
  }
  return 0;
}

void CleanSavedDeviceInfo()
{
  // Make sure that we clean object vector...
  if (AllObjects.size()) {
    vector<ObjectTableElement*>::const_iterator current = AllObjects.begin();
    while (current != AllObjects.end()) {
      delete (*current);
      ++current;
    }
    AllObjects.clear();
  }
  if (ObjectsInFW.size()) {
    vector<OPObject*>::const_iterator current = ObjectsInFW.begin();
    vector<OPRegister*>::const_iterator regcurrent;
    while (current != ObjectsInFW.end()) {
      regcurrent = (*current)->registers.begin();
      while (regcurrent != (*current)->registers.end()) {
        delete (*regcurrent);
        ++regcurrent;
      }
      delete (*current);
      ++current;
    }
    ObjectsInFW.clear();
  }
  if (ObjectsInFile.size()) {
    vector<OPObject*>::const_iterator current = ObjectsInFile.begin();
    vector<OPRegister*>::const_iterator regcurrent;
    while (current != ObjectsInFile.end()) {
      regcurrent = (*current)->registers.begin();
      while (regcurrent != (*current)->registers.end()) {
        delete (*regcurrent);
        ++regcurrent;
      }
      delete (*current);
      ++current;
    }
    ObjectsInFile.clear();
  }

  if (hI2CDriver != INVALID_HANDLE_VALUE) {
    CloseHandle(hI2CDriver);
    hI2CDriver = INVALID_HANDLE_VALUE;
  }
  if (hI2CDriverBootloader != INVALID_HANDLE_VALUE) {
    CloseHandle(hI2CDriverBootloader);
    hI2CDriverBootloader = INVALID_HANDLE_VALUE;
  }
  InitI2CNormalDone = FALSE;
  InitI2CBootloaderDone = FALSE;

  if (HidDevice[FIRST_DISCOVERY].size()) {
    FreeMemHidDevices();
  }
  if (HidDevice[SECOND_DISCOVERY].size()) {
    FreeMemHidDevices(SECOND_DISCOVERY);
  }

  if (MxtCtrlDevice[FIRST_DISCOVERY].size()) {
    MxtCtrlDevice[FIRST_DISCOVERY].clear();
  }

  if (MxtCtrlDevice[SECOND_DISCOVERY].size()) {
    MxtCtrlDevice[SECOND_DISCOVERY].clear();
  }

  DeviceAlreadyIdentified = FALSE;
}



BOOL HIDI2C_ReadWrite(UINT8 wlen, UINT8 *wbuf, UINT8 rlen, UINT8 *rbuf)
{
	// BOOL ret;
	UINT8 bufw[65];
	//UINT8 bufr[65];

	if (ConnectFWType == I2CBRIDGE) {
		if (!I2CBridge_HIDI2C_ReadWrite(wlen, wbuf, rlen, rbuf))
			return FALSE;
		// first two bytes input register ignore because in Inbox I2C usecase, the two bytes cannot be read.
		for (int i = 0; i < mXTDevice.InputReportByteLength - 2 /*rlen*/; i++)
			rbuf[i] = rbuf[i + 2];
	}
	else if (ConnectFWType == HIDUSB || ConnectFWType == HIDI2C) {
		// ZeroMemory(bufw, 65);
		for (int i = 0; i < 65; i++)
			bufw[i] = 0;

		bufw[0] = AtmelReportId;
		bufw[1] = 0x51;
		bufw[2] = wlen;
		bufw[3] = rlen;
		for (int i = 4; i < mXTDevice.OutputReportByteLength; i++) {
			bufw[i] = wbuf[i - 4];
		}

		DWORD bytesWritten = mXTDevice.OutputReportByteLength;
		DWORD bytesRead = 1;
		if (!ReadWriteDevice_InboxDriver_DuplicateCheck(bufw, &bytesWritten, rbuf, &bytesRead, rlen)) {
			return FALSE;
		}
	}
	else {
		printf("Not correct ConnectFWType=%d in HIDI2C_ReadWrite. Exit...\n", ConnectFWType);
		exit(0);
	}



	return TRUE;
}


// Miracle. 2016/02/23. Overwrite readNibble in BootWorker.cpp
UINT8 readNibble(ifstream *file)
{
	char temp;

	if (!file->is_open()) {
		return 0;
	}

	if (!file->good()) {
		return 0;
	}

	temp = (char)file->get();

	if (temp >= '0' && temp <= '9')
	{
		return temp - '0';
	}
	else if (temp >= 'A' && temp <= 'F')
	{
		return temp - 'A' + 10;
	}
	else if (temp >= 'a' && temp <= 'f')
	{
		return temp - 'a' + 10;
	}
	else
		return 0;
}

BOOL Get_HIDBootState(UINT8 *bstate)
{
	UINT8 bufw[256];
	UINT8 bufr[256];

	for (int i = 0; i < mXTDevice.OutputReportByteLength; i++)
		bufw[i] = 0;

	if (!HIDI2C_ReadWrite(0, bufw, 1, bufr))
		return FALSE;

	*bstate = bufr[3];

	return TRUE;
}

// Return codes:
// -1: generic error
// if error codes required, negative code returned with error
// 0: success
int WriteFWToDevice_HID()
{
	int iRetCode = -1; // set as generic error
	BOOL bFirstTimeWHID = TRUE;
	UINT percentDone = 0;
	ifstream encFWFile;
	//DWORD WaitTime = 0;
	UINT32 totalFileSize;
	int fileSize = 0;
	int framecount = 0;
	UINT8 frame[1024];
	UINT8 wbuf[1024];
	UINT8 rbuf[1024];
	UINT8 bstate = false;
	UINT8 wlen = 0;
	int len_input;


#ifndef AUTO_UPDATER
	encFWFile.open(EncFile, ifstream::in);

	if (!encFWFile.is_open()) {
		_stprintf(PrintBuffer, _T("Error opening binary file: %s. Cannot proceed. \n"), EncFile);
		_TPRINTF(PrintBuffer);
		if (ErrorCodes) iRetCode = FW_FILE_OPEN_ERROR;
		return iRetCode;
	}
	//get total filesize
	encFWFile.seekg(0, std::ios::end);
	totalFileSize = (UINT32)encFWFile.tellg() / 2; //divide by two because of ASCII encoding of enc files
	encFWFile.seekg(0);

#else
	totalFileSize = FWFileSize / 2;  //divide by two because of ASCII encoding of enc files
#endif

	//start writing fw frames
	fileSize = 0;
	for (;;)	 {
		/* Read block of data. */
		UINT16 blockLength;

		blockLength = ((UINT16)readNibble(&encFWFile)) << (4 * 3);
		blockLength |= ((UINT16)readNibble(&encFWFile)) << (4 * 2);
		blockLength |= ((UINT16)readNibble(&encFWFile)) << (4 * 1);
		blockLength |= ((UINT16)readNibble(&encFWFile)) << (4 * 0);

		if (blockLength == 0) {
			_stprintf(PrintBuffer, _T("\rDownloading firmware completed\n"));
			_TPRINTF(PrintBuffer);
			_stprintf(PrintBuffer, _T("If the program gets stuck here longer than 10-15 sec. please hit CTRL+c.\nYou can perform \"MxtUpdater -reset\" and \"MxtUpdater -i\" to check status.\n"));
			_TPRINTF(PrintBuffer);
			_stprintf(PrintBuffer, _T("If commands get stuck, try disabling/enabling the driver from Device Manager.\nOccasionally it may be necessary to power-cycle.\n\n"));
			_TPRINTF(PrintBuffer);
			iRetCode = SUCCESS;
			break;
		}

		UINT16 i = 0;
		frame[i++] = (blockLength & 0xFF00) >> 8;
		frame[i++] = (blockLength & 0x00FF) >> 0;


		while (blockLength) {
			UINT8 byte = readNibble(&encFWFile) << (4 * 1);
			byte |= readNibble(&encFWFile) << (4 * 0);
			frame[i] = byte;
			++i;
			++fileSize;
			--blockLength;
		}

		if (ConnectFWType == I2CBRIDGE) {
			len_input = 256 * HIDHEADER[HIDDSC_InputLength_MSB] + HIDHEADER[HIDDSC_InputLength_LSB] - 8;
		}
		else if (ConnectFWType == HIDUSB || ConnectFWType == HIDI2C) {
			len_input = mXTDevice.OutputReportByteLength - 4;
		}
		else {
			printf("Not recognize ConnectFWType in WriteFWToDevice_HID!\n");
			exit(0);
		}
		for (int k = 0; k < i;)
		{
			for (wlen = 0;;)
			{
				wbuf[wlen] = frame[k];
				k++;
				wlen++;
				if (wlen >= len_input)
					break;
				if (k >= i)
					break;
			}

			if (wlen >= len_input) {
				if (!HIDI2C_ReadWrite(wlen, wbuf, 0, rbuf)) {	// 2016/03/28. Write ENC payload only without read bootstate. Change request by Janus.
					_stprintf(PrintBuffer, _T("WritePayloadwoRead\n\n"));
					_TPRINTF(PrintBuffer);
					return FALSE;
				}
			}
			else
			{
				if (!HIDI2C_ReadWrite(wlen, wbuf, 1, rbuf)){		// 2016/03/28. For last small trunk data should read bootstate.
					_stprintf(PrintBuffer, _T("ReadBuffLastData\n\n"));
					_TPRINTF(PrintBuffer);
					
					return FALSE;
				}

				bstate = rbuf[3];								// first boot state to check, MCG would not get run, ??, Not needed?
			}

			framecount = framecount + wlen;			//Keep track of the frame count
																																																																																																																																																																																																																																																																								
			if (k >= i)
				break;
		}


		DBGPRINT(_T("Wrote frame of length %d\n"), i);
		DBGPRINT(_T("-- %dB / %dB, %d%% --\n\n"), fileSize, totalFileSize, (fileSize * 100) / totalFileSize);

		if (bFirstTimeWHID) {
			_tprintf(_T("Downloading firmware 00%c done"), '%');
			if (MaxTouchAnalyzer) _TPRINTF(_T("\n"));
			bFirstTimeWHID = false;
		}
		else {
			if (MaxTouchAnalyzer) {
				_stprintf(PrintBuffer, _T("%2d/100 done\r\n"), ((fileSize * 100) / totalFileSize));
				_TPRINTF(PrintBuffer);
			}
			else {
				if ((fileSize * 100) / totalFileSize != percentDone) {
					_tprintf(_T("\b\b\b\b\b\b\b\b%2d%c done"), ((fileSize * 100) / totalFileSize), '%');
				}
			}
			percentDone = (fileSize * 100) / totalFileSize;
		}

		for (int k = 0; k <= 10; k++) {		//Check to get HIDBootState loop of 10
			if (k == 10) {
				//printf("Error! Timeout to wait for PASS CRC check.\n");
				_stprintf(PrintBuffer, _T("\nError! Timeout to wait for PASS CRC check.\n"));
				_TPRINTF(PrintBuffer);
				return FALSE;
			}
			if ((bstate & 0x03) == 0x03)
			{
				//printf("Error: CRC check failure!\n");
				_stprintf(PrintBuffer, _T("\nError: CRC check failure!\n"));
				_TPRINTF(PrintBuffer);
				return FALSE;
			}
			if (bstate == 0x04)
			{	// Pass CRC check
				break;
			}
			if (bstate == 0x84)
				break;		// This special state means currently CRC pass and wait for frame data.
			if (!Get_HIDBootState(&bstate)) {
				_stprintf(PrintBuffer, _T("\nError: Stage 1 Get Boot State Failed.\n"));
				_TPRINTF(PrintBuffer);
				return FALSE;
			}
		}

		if (bstate == 0x84)
			continue;		// This special state means CRC pass and wait for frame data. It helps to save one HIDI2C write/read transaction.

		//Track the total frames flashed
		if (framecount == totalFileSize) {

			_stprintf(PrintBuffer, _T("\nFlashing completed successfully.\n"));
			_TPRINTF(PrintBuffer);
			break;

		}

		for (int k = 0; k <= 10; k++) {
			if (k == 10) {
				_stprintf(PrintBuffer, _T("\nError: Timeout to wait for frame ready.\n"));
				_TPRINTF(PrintBuffer);
				return FALSE;
			}
			if (!Get_HIDBootState(&bstate)) {
				_stprintf(PrintBuffer, _T("\nError: Stage 2 Get Boot State Failed.\n"));
				_TPRINTF(PrintBuffer);
				return FALSE;
			}
			if ((bstate & 0xC0) == 0x80)
				break;
		}


	}

#ifndef AUTO_UPDATER
	//close .enc file handle
	encFWFile.close();
#endif

	return(iRetCode);
}


BOOL FWUpgrade_HIDBootloader()
{
	UINT8 bufw[65];
	UINT8 bufr[65];


	// Unlock command
	ZeroMemory(bufw, 65);
	bufw[0] = 0xDC;
	bufw[1] = 0xAA;
	if (!HIDI2C_ReadWrite(2, bufw, 1, bufr)) {
		printf("Unlock command failed.\n");
		return FALSE;
	}

	// Write frame data.
	WriteFWToDevice_HID();

	return TRUE;
}


BOOL SetmXTDeviceInBootloadMode()
{
  BOOL bReturncode;
  UINT8 addr = 0;
  bReturncode = SendCmdToDevice(eReset, BOOTLOADER);
  // If successful wait a little bit
  if (bReturncode) {
    if (MaxTouchAnalyzer)
    Sleep(1000);
    else
    Sleep(2000);
    // Added by Miracle.
    if (DeviceDiscovered == USB_NORMAL && ConnectFWType == I2CBRIDGE) {
      addr = I2CBridge_FindAddr();
      if (addr > 0x40) {
        return FALSE;   // Still in application address.
      }
    }
    return (bReturncode);
  }
  return FALSE;
}

BOOL BackupAndResetDevice()
{  
  if (SendCmdToDeviceWithAck(eBackupNV, BACKUPNVM)) {
    if (MaxTouchAnalyzer)
    Sleep(1000);
    else
    Sleep(2000);
    
  return (ResetDevice());
  }
  return FALSE;
}

BOOL BackupDevice(UINT8 Code)
{
  if (SendCmdToDeviceWithAck(eBackupNV, Code)) {
    Sleep(1000);
    return TRUE;
  }
  return FALSE;
}

BOOL ResetDevice()
{
  BOOL bRetCode = TRUE;
#ifndef NO_RESET
  bRetCode = SendCmdToDevice(eReset, RESET);
#endif
  return bRetCode;
}

BOOL CalibrateDevice()
{
  return SendCmdToDeviceWithAck(eCalibrate, RESET);
}

BOOL SendCmdToDevice(UINT8 CmdType, UINT8 Code)
{
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];   // Added by Miracle.
  DWORD bytesWritten;
  BOOL bReturncode = TRUE;
  UINT16 address;

  // get Command Processor address...
  if (!GetObjectAddressAndSize(GEN_COMMANDPROCESSOR_T6, &address)) {
    // Cannot get Command Processor address...
    return FALSE;
  }
  // Added by Miracle.
  DBGPRINT(_T("\nSendCmdToDevice: Address: %d\n"),address);

  if (ConnectFWType == I2CBRIDGE) {
    writeBuffer[0] = (address & 0x00ff) + CmdType;
    writeBuffer[1] = (address >> 8) & 0x00ff;
    writeBuffer[2] = Code;
    bReturncode = I2CBridge_ReadWrite(3, writeBuffer, 0, readBuffer);
  }
  else if (ConnectFWType == HIDUSB || ConnectFWType == HIDI2C) {
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;
    writeBuffer[1] = 0x51;
    writeBuffer[2] = 0x03;
    writeBuffer[3] = 00;
    writeBuffer[4] = (address & 0x00ff) + CmdType;
    writeBuffer[5] = (address >> 8) & 0x00ff;
    writeBuffer[6] = Code;

    bReturncode = WriteFile (mXTDevice.HidDevice,
      writeBuffer,
      mXTDevice.OutputReportByteLength,
      &bytesWritten,
      NULL) && (bytesWritten == mXTDevice.OutputReportByteLength);
  } else {
    address += CmdType; // get the right register
    bytesWritten = WriteWithRetryToMxt(hI2CDriver, address, 1, &Code);
    if (bytesWritten != 1) {
      return FALSE;
    }
  }
  return bReturncode;
}

BOOL SendCmdToDeviceWithAck(UINT8 CmdType, UINT8 Code)
{
  UINT8 writeBuffer[65], readBuffer[65];
  DWORD bytesWritten = 0, bytesRead = 0;
  BOOL bReturncode = TRUE;
  UINT16 address;
  int idx;

  if (!ReadMemoryMap()) {
    _stprintf(PrintBuffer, _T("Cannot read memory map\n"));
//    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  // get Command Processor address...
  if (!GetObjectAddressAndSize(GEN_COMMANDPROCESSOR_T6, &address)) {
    // Cannot get Command Processor address...
    _stprintf(PrintBuffer, _T("GetObjectAddressAndSize error!\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  // Added by Miracle.
  if (ConnectFWType == I2CBRIDGE && Protocol_HID) {
    idx = 0;
    writeBuffer[idx++] = (address & 0x00ff) + CmdType;
    writeBuffer[idx++] = (address >> 8) & 0x00ff;
    writeBuffer[idx++] = Code;
    bReturncode = I2CBridge_HIDI2C_ReadWrite((UINT8)idx, writeBuffer, 0, NULL);

    ReadMessage(readBuffer);

    idx = 0;
    writeBuffer[idx++] = HIDHEADER[HIDDSC_OutputRegister_LSB];
    writeBuffer[idx++] = HIDHEADER[HIDDSC_OutputRegister_MSB];
    writeBuffer[idx++] = 4;   // HID layer. Output length
    writeBuffer[idx++] = 0;
    writeBuffer[idx++] = 0x06;    // HID layer. Output length
    writeBuffer[idx++] = 0x88;    // OBP layer. Output register.
    bReturncode = I2CBridge_I2C_ReadWrite((UINT8)idx, writeBuffer, 0, NULL);

    ReadMessage(readBuffer);

  }
  else if (ConnectFWType == HIDUSB || ConnectFWType == HIDI2C) {
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;
    writeBuffer[1] = 0x51;
    writeBuffer[2] = 0x03;
    writeBuffer[3] = 00;
    writeBuffer[4] = (address & 0x00ff) + CmdType;
    writeBuffer[5] = (address >> 8) & 0x00ff;
    writeBuffer[6] = Code;

    if(!ReadWriteDevice_InboxDriver(writeBuffer, &bytesWritten, readBuffer, &bytesRead, 0x04)) {
      return FALSE;
    }
  } else {
    address += CmdType; // get the right register
    bytesWritten = WriteWithRetryToMxt(hI2CDriver, address, 1, &Code);
    if (bytesWritten != 1) {
      return FALSE;
    }
    bytesRead = ReadWithRetryFromMxt(hI2CDriver, address, 1, readBuffer);
    if (bytesRead != 1) {
      return FALSE;
    }
  }
  return bReturncode;
}

void FindBootloader()
{
  //start search for bootloader chip
  bootloader_state = BOOTLOADER_STATE_NOT_FOUND_SEARCH_ADDR;
  i2c_address_index = 0u;
  app_crc_fail = false;

  //setup address 1
  if(ok_to_use_400khz)
  {
    WriteData(0x80, 0x30, i2c_addresses[i2c_address_index], 0x00, 0x00, 0x00, 0x00, 0x00);
  }
  else
  {
    WriteData(0x80, 0x00, i2c_addresses[i2c_address_index], 0x00, 0x00, 0x00, 0x00, 0x00);
  }
}

BOOL IsObjectConfigurable(UINT Index)
{
  UINT j = 0;
  while (NonConfigurableObjects[j]) {
    if (AllObjects[Index]->Type == NonConfigurableObjects[j]) {
      return FALSE;
    }
    ++j;
  }
  return TRUE;
}

BOOL ReadConfigFromDevice()
{
  UINT i, j;
  UINT8 bufr[65];

  if (!ReadMemoryMap()) return FALSE;
  for (i = 0; i < NumberOfObjects; i++) {
    if (AllObjects[i]->Type == 0) {
      // No more objects
      break;
    }
    if (!IsObjectConfigurable(i)) {
      continue;
    }

  ReadMessage(bufr);

    for (j = 0; j <= AllObjects[i]->Instances; ++j) {
      if(!FillValueInObject(i, j)) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOL FillValueInObject(UINT Index, UINT Instance)
{
  UINT8 buffer[1024];
  UINT16 address;
  UINT value;
  OPObject *pCurrentObject;
  DWORD bytesRead;

  address = AllObjects[Index]->LSB;
  address |= (AllObjects[Index]->MSB << 8) &0xFF00;
  address += (UINT16)((AllObjects[Index]->Size + 1) * Instance); // get the right address
  bytesRead =  AllObjects[Index]->Size + 1;

  if (!ReadFromDevice(address, buffer, &bytesRead)) return FALSE;
  pCurrentObject = CreateAndInitObject(AllObjects[Index]->Type, Instance, address, AllObjects[Index]->Size + 1);
  if (!pCurrentObject) {
    _stprintf(PrintBuffer, _T("Found an unknown object type %d in the device.\n"), AllObjects[Index]->Type);
    _TPRINTF(PrintBuffer);
    _stprintf(PrintBuffer, _T("This version of the tool cannot save the configuration of this device.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  ObjectsInFW.push_back(pCurrentObject);
  vector<OPRegister*>::iterator regcurrent = pCurrentObject->registers.begin();
  while (regcurrent != pCurrentObject->registers.end()) {
    if ((*regcurrent)->size == 4) {
      // Make sure we are in boundaries
      if ((*regcurrent)->offset < bytesRead - 3) {
        value = buffer[(*regcurrent)->offset] & 0x00ff;
        value += ((buffer[(*regcurrent)->offset+1] << 8) & 0xff00);
        value += ((buffer[(*regcurrent)->offset+2] << 16) & 0xff0000);
        value += ((buffer[(*regcurrent)->offset+3] << 24) & 0xff000000);
        (*regcurrent)->value = value;
        if ((*regcurrent)->isSigned) {
          if ((*regcurrent)->value & 0x80000000) {
            // add sign...
            (*regcurrent)->value |= 0x80000000;
          } else {
            // remove sign...
            (*regcurrent)->value &= 0x7FFFFFFF;
          }
        }
      }
    } else if ((*regcurrent)->size == 2) {
      // Make sure we are in boundaries
      if ((*regcurrent)->offset < bytesRead - 1) {
        value = buffer[(*regcurrent)->offset] & 0x00ff;
        value += ((buffer[(*regcurrent)->offset+1] << 8) & 0xff00);
        (*regcurrent)->value = value;
        if ((*regcurrent)->isSigned) {
          if ((*regcurrent)->value & 0x8000) {
            // add sign...
            (*regcurrent)->value |= 0xFFFF0000;
          } else {
            // remove sign...
            (*regcurrent)->value &= 0x0000FFFF;
          }
        }
      }
    } else {
      if ((*regcurrent)->offset < bytesRead) {
        (*regcurrent)->value = buffer[(*regcurrent)->offset];
        if ((*regcurrent)->isSigned) {
          if ((*regcurrent)->value & 0x80) {
            // add sign...
            (*regcurrent)->value |= 0xFFFFFF00;
          } else {
            // remove sign...
            (*regcurrent)->value &= 0x000000FF;
          }
        }
      }
    }
    ++regcurrent;
  }
  return TRUE;
}

BOOL WriteConfigFile(TCHAR *FilePath)
{
  wofstream outFile;
  outFile.open(FilePath, wofstream::out);
  if (!outFile.is_open()) {
    // We already tested it - it should never happen
    _stprintf(PrintBuffer, _T("Cannot open file for writing.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  if (ObjectsInFW.size()) {
    vector<OPObject*>::const_iterator current = ObjectsInFW.begin();
    while (current != ObjectsInFW.end()) {
      if ((*current)->start_address == MXT_INVALID_ADDRESS) {
        ++current;
        continue;
      }
      outFile << "[" <<  (*current)->name << " INSTANCE " << (*current)->instance << "]\n";
      vector<OPRegister*>::const_iterator regcurrent = (*current)->registers.begin();
      while (regcurrent != (*current)->registers.end()) {
        if ((*regcurrent)->isSigned) {
          outFile <<  (*regcurrent)->name << "=" << (INT)(*regcurrent)->value <<"\n";
        } else {
          outFile <<  (*regcurrent)->name << "=" << (*regcurrent)->value <<"\n";
        }
        ++regcurrent;
      }
      ++current;
    }
  }
  outFile.close();
  return TRUE;
}

BOOL WriteXConfigFile(TCHAR *FilePath)
{
  wofstream outFile;
  TCHAR buffer[256];
  SYSTEMTIME datetime;
  DWORD dwSize = sizeof(buffer);
  UINT32 checksum;
  UINT address;

  outFile.open(FilePath, wofstream::out);
  if (!outFile.is_open()) {
    // We already tested it - it should never happen
    _stprintf(PrintBuffer, _T("Cannot open file %s for writing.\n"), FilePath);
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  if (!ReadNVMChecksum(&checksum)) {
    // If it fails, write -1
    checksum = 0xffffffff;
  }
  // Mimic the same header as QTServer
  outFile << "[COMMENTS]\n";
  GetUserName(buffer, &dwSize);
  outFile << "Saved by: " <<  buffer << "\n";
  GetLocalTime(&datetime);
  _stprintf(buffer, _T("Date and time: %s %s %02d %02d:%02d:%02d %d\n\n"), ShortDays[datetime.wDayOfWeek], ShortMonths[datetime.wMonth], datetime.wDay, datetime.wHour, datetime.wMinute, datetime.wSecond, datetime.wYear);
  outFile << buffer;

  outFile << "[VERSION_INFO_HEADER]\n";

  //i = (DeviceMemoryMap.FamilyID & 0xF0) | (DeviceMemoryMap.FamilyID & 0x0F);
  outFile << "FAMILY_ID=" << DeviceMemoryMap.FamilyID << "\n";
  outFile << "VARIANT=" << DeviceMemoryMap.VariantID << "\n";
  outFile << "VERSION=" << DeviceMemoryMap.Version << "\n";
  outFile << "BUILD=" << DeviceMemoryMap.Build << "\n";

  _stprintf(buffer, _T("VENDOR_ID=0x3EB\n"));
  outFile << buffer;
  _stprintf(buffer, _T("PRODUCT_ID=0x%04X\n"), AllPids[DeviceIndex-1]);
  outFile << buffer;
  _stprintf(buffer, _T("CHECKSUM=0x%06X\n"), checksum);
  outFile << uppercase << buffer;
  _stprintf(buffer, _T("INFO_BLOCK_CHECKSUM=0x%06X\n"), InfoBlockChecksum);
  outFile << uppercase << buffer;

  outFile << "[APPLICATION_INFO_HEADER]\n";
  outFile << "NAME=" << _T("MxtUpdater") << "\n";
  outFile << "VERSION=" << MXTUPDATER_VERSION << "\n";

  if (ObjectsInFW.size()) {
    vector<OPObject*>::const_iterator current = ObjectsInFW.begin();
    while (current != ObjectsInFW.end()) {
      if ((*current)->start_address == MXT_INVALID_ADDRESS) {
        ++current;
        continue;
      }
      if ((*current)->type != SPT_PROTOTYPE_T35) {
        outFile << "[" <<  (*current)->name << " INSTANCE " << (*current)->instance << "]\n";
      } else {
        address = (*current)->start_address;
        outFile << "[" <<  (*current)->name << " INSTANCE " << (*current)->instance << " ADDRESS " <<  address << " SIZE " << (*current)->objectSize << "]\n";
      }

      _stprintf(buffer, _T("OBJECT_ADDRESS=%d\n"), (*current)->start_address);
      outFile << buffer;
      _stprintf(buffer, _T("OBJECT_SIZE=%d\n"), (*current)->objectSize);
      outFile << buffer;

      vector<OPRegister*>::const_iterator regcurrent = (*current)->registers.begin();
      while (regcurrent != (*current)->registers.end()) {
        if ((*regcurrent)->isSigned) {
          outFile << (*regcurrent)->offset << " " << (*regcurrent)->size << " " <<  (*regcurrent)->name << "=" << (INT)(*regcurrent)->value <<"\n";
        } else {
          outFile << (*regcurrent)->offset << " " << (*regcurrent)->size << " " <<  (*regcurrent)->name << "=" << (*regcurrent)->value <<"\n";
        }
        ++regcurrent;
      }
      ++current;
    }
  }
  outFile.close();
  return TRUE;
}

BOOL WriteRawConfigFile(TCHAR *FilePath, BOOL IsFromDevice)
{
  vector<OPObject*> *pObjects;
  wofstream outFile;
  TCHAR buffer[256];
  UINT32 checksum;
  UINT value;

  // This function can be used either from data read from device or
  // from file (converting to raw). 
  pObjects = &ObjectsInFile;
  if (IsFromDevice) {
    pObjects = &ObjectsInFW;
  }
  outFile.open(FilePath, wofstream::out);
  if (!outFile.is_open()) {
    // We already tested it - it should never happen
    _stprintf(PrintBuffer, _T("Cannot open file %s for writing.\n"), FilePath);
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  if (IsFromDevice) {
    if (!ReadNVMChecksum(&checksum)) {
      // If it fails, write -1
      checksum = 0xffffffff;
    }
  } else {
    // get it from .xcfg file
    checksum = ConfigChecksum;
    // put a fake value else Object Server won't use this raw file - only if InfoBlockChecksum == 0 
    // Object Server supports InfoBlockChecksum starting from 4.10.211.0
    if (!InfoBlockChecksum) {
      InfoBlockChecksum = ConfigChecksum;
    }
  }
  // Mimic the same header as QTServer
  outFile << "OBP_RAW V1\n";

  //i = (DeviceMemoryMap.FamilyID & 0xF0) | (DeviceMemoryMap.FamilyID & 0x0F);
  _stprintf(buffer, _T("%02X %02X %02X %02X %02X %02X %02X"), DeviceMemoryMap.FamilyID, DeviceMemoryMap.VariantID, DeviceMemoryMap.Version, DeviceMemoryMap.Build, DeviceMemoryMap.MatrixSizeX, DeviceMemoryMap.MatrixSizeY, DeviceMemoryMap.NumberOfObjects);
  outFile << buffer << "\n";
  outFile << hex << uppercase << InfoBlockChecksum << "\n";
  _stprintf(buffer, _T("%06X"), checksum);
  outFile << buffer << "\n";

//  outFile << hex << uppercase << checksum << "\n";

  if (pObjects->size()) {
    vector<OPObject*>::const_iterator current = pObjects->begin();
    while (current != pObjects->end()) {
      if ((*current)->start_address == MXT_INVALID_ADDRESS) {
        ++current;
        continue;
      }
      if ((*current)->type == DEBUG_DIAGNOSTIC_T37) {
        // skip this
        ++current;
        continue;
      }
      _stprintf(buffer, _T("%04X %04X %04X"), (*current)->type, (*current)->instance, (*current)->objectSize);
      outFile << buffer;

      vector<OPRegister*>::const_iterator regcurrent = (*current)->registers.begin();
      while (regcurrent != (*current)->registers.end()) {
        outFile << " ";
        if ((*regcurrent)->size == 4) {
          value = (*regcurrent)->value;
          _stprintf(buffer, _T("%02X %02X %02X %02X"), (value & 0x00FF), ((value & 0xFF00) >> 8), ((value & 0xFF0000) >> 16), ((value & 0xFF000000) >> 24)); 
          outFile << buffer;
          ++regcurrent;
          continue;
        }
        if ((*regcurrent)->size == 2) {
          value = (*regcurrent)->value;
          _stprintf(buffer, _T("%02X %02X"), (value & 0x00FF), ((value & 0xFF00) >> 8)); 
          outFile << buffer;
          ++regcurrent;
          continue;
        }
        _stprintf(buffer, _T("%02X"), (*regcurrent)->value & 0x00FF);
        outFile << buffer;
        ++regcurrent;
      }
      outFile << "\n";
      ++current;
    }
  }

  outFile.close();
  return TRUE;
}


// The following routine writes the configuration to a device
// The input file is parsed with: "ParseReadConfigFile(FilePath)"
// This routine is a bit tricky to follow
// Pls. refer to the way we write to the device
// Also notice that configuration parameters have different offsets may not be contiguous
// and this complicates a bit all the address calculations below
// Two routines are used, depending on using Inbox driver or debug driver.

BOOL WriteConfigToDevice(TCHAR *FilePath)
{
#ifdef AUTO_UPDATER
  // Check if regular or extended config
  TCHAR line[LINE_MAX_SIZE];
  UINT size = LINE_MAX_SIZE-1;
  GetLine(line, &size);
  if (!_tcsnicmp(line, _T("[COMMENTS]"), 10)) {
    // Extended config: set the flag
    bIsXCfg = TRUE;
  }
#endif
  // Check if we perform default config clear - -wxl leaves values there
  if (ClearConfig) {
    ZeroConfigMemory();
  }
  // Added by Miracle.
  if (ConnectFWType == HIDUSB || ConnectFWType == HIDI2C || ConnectFWType == I2CBRIDGE) {
    return (WriteConfigToDeviceUSB(FilePath));
  }
  return(WriteConfigToDeviceI2C(FilePath));
}

BOOL WriteConfigToDeviceUSB(TCHAR *FilePath)
{
  UINT8 writeBuffer[65];
  UINT8 readBuffer[65];
  DWORD bytesWritten, bytesRead;
  BOOL bReturncode;
  UINT16 address;
  UINT i, j, k, totalBytesSent;

   if (bIsRaw) {
    bReturncode = ParseReadConfigRawFile(FilePath);
  } else if (bIsXCfg) {
    bReturncode = ParseReadConfigXFile(FilePath);
  } else {
    bReturncode = ParseReadConfigFile(FilePath);
  }
  if (!bReturncode) return FALSE;
  
  // Check if this config is for this FW version
  // If we read values from config: FamilyId must be always non-zero
  if (CfgFamilyId) {
    if ((CfgFamilyId != DeviceMemoryMap.FamilyID) || (CfgVariant != DeviceMemoryMap.VariantID) ||
      (CfgVersion != DeviceMemoryMap.Version) || (CfgBuild != DeviceMemoryMap.Build)) {
        _stprintf(PrintBuffer, _T("This configuration applies to FW version: \t0x%02X%02X\n"), CfgVersion, CfgBuild);
        _TPRINTF(PrintBuffer);
        if (DeviceAppFromXML) {
          _stprintf(PrintBuffer, _T("The version of FW running in %s is: \t0x%02X%02X\n"), DeviceAppNameFromXML, DeviceMemoryMap.Version, DeviceMemoryMap.Build);
        } else {
        _stprintf(PrintBuffer, _T("The version of FW running in %s is: \t0x%02X%02X\n"), DEVICE_NAME[DeviceConnected], DeviceMemoryMap.Version, DeviceMemoryMap.Build);
        }
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("\nMismatch between FW version in config file and FW version running in device.\nContinuing with potential errors...\n"));
        _TPRINTF(PrintBuffer);
    }
  }
//  CreateDynamicObjects();
//  if (!AddDynamicObjects()) {
//    VerifyConfigEnabled = FALSE;
//  }

  // Now we have the configuration values in the Objects vector...
  vector<OPObject*>::const_iterator current = ObjectsInFile.begin();

  while (current != ObjectsInFile.end()) {
    // Check if DEBUG_DIAGNOSTIC_T37: if so, skip it...
    // This object is read-only and it is saved by the Object Server for debugging purposes
    if ((*current)->type == DEBUG_DIAGNOSTIC_T37) {
      ++current;
      continue;
    }
    address = (*current)->start_address;
    j = 1;
    i = 0;
    totalBytesSent = 0;
    while (i < (*current)->registers.size()) {
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = AtmelReportId;
    writeBuffer[1] = 0x51;
    writeBuffer[3] = 0;
    writeBuffer[4] = address & 0x00FF;
    writeBuffer[5] = (address >> 8) & 0x00FF;

    // (mXTDevice.OutputReportByteLength - 6) is the maximum number of bytes that can be sent to the device
    // we do one less in case the last item has 2 bytes size
    while (i < (*current)->registers.size() && (*current)->registers[i]->offset < j*(mXTDevice.OutputReportByteLength-7)) {
      k = (*current)->registers[i]->offset + 6 - totalBytesSent;
      writeBuffer[k] = (*current)->registers[i]->value & 0x00ff;
      if ((*current)->registers[i]->size == 2) {
        writeBuffer[k+1] = ((*current)->registers[i]->value >> 8) & 0x00ff;
      }
      if ((*current)->registers[i]->size == 4) {
        writeBuffer[k+1] = ((*current)->registers[i]->value >> 8) & 0x00ff;
        writeBuffer[k+2] = ((*current)->registers[i]->value >> 16) & 0x00ff;
        writeBuffer[k+3] = ((*current)->registers[i]->value >> 24) & 0x00ff;
      }
      ++i;
    }
    writeBuffer[2] = (UINT8)(2 + (*current)->registers[i-1]->offset + (*current)->registers[i-1]->size - totalBytesSent);

    if (ConnectFWType  == I2CBRIDGE && Protocol_HID == TRUE) {
      bReturncode = I2CBridge_HIDI2C_Write(address, writeBuffer[2], &writeBuffer[6]);
    }
    else {
      if(!ReadWriteDevice_InboxDriver(writeBuffer, &bytesWritten, readBuffer, &bytesRead, 4)) {
        DBGPRINT(_T("WriteConfigToDevice: Error on WriteFile, code = %d\n"), GetLastError());
        return FALSE;
      }
    }
    ++j;

    totalBytesSent = (*current)->registers[i-1]->offset + (*current)->registers[i-1]->size;
    address = (UINT16)((*current)->start_address + totalBytesSent); // update address
    }
    current++;
  }
  return TRUE;
}

BOOL WriteConfigToDeviceI2C(TCHAR *FilePath)
{
  UINT8 writeBuffer[1024];
  DWORD bytesWritten;
  UINT16 address;
  UINT i, bytesToBeSent = 0;
  BOOL bReturncode;

  if (bIsRaw) {
    bReturncode = ParseReadConfigRawFile(FilePath);
  } else if (bIsXCfg) {
    bReturncode = ParseReadConfigXFile(FilePath);
  } else {
    bReturncode = ParseReadConfigFile(FilePath);
  }
  if (!bReturncode) return FALSE;

//  CreateDynamicObjects();
//  if (!AddDynamicObjects()) {
//    VerifyConfigEnabled = FALSE;
//  }

  // Now we have the configuration values in the Objects vector...
  vector<OPObject*>::const_iterator current = ObjectsInFile.begin();
  while (current != ObjectsInFile.end()) {
    // Check if DEBUG_DIAGNOSTIC_T37: if so, skip it...
    if ((*current)->type == DEBUG_DIAGNOSTIC_T37) {
      ++current;
      continue;
    }
    ZeroMemory(writeBuffer, 1024);
    address = (*current)->start_address;
    i = 0;
    while (i < (*current)->registers.size()) {
      writeBuffer[(*current)->registers[i]->offset] = (*current)->registers[i]->value & 0x00ff;
      // keep track of amount of data written
      bytesToBeSent = (*current)->registers[i]->offset;
      if ((*current)->registers[i]->size == 2) {
        writeBuffer[(*current)->registers[i]->offset + 1] = ((*current)->registers[i]->value >> 8) & 0x00ff;
        bytesToBeSent = (*current)->registers[i]->offset + 1;
      }
      if ((*current)->registers[i]->size == 4) {
        writeBuffer[(*current)->registers[i]->offset + 1] = ((*current)->registers[i]->value >> 8) & 0x00ff;
        writeBuffer[(*current)->registers[i]->offset + 2] = ((*current)->registers[i]->value >> 16) & 0x00ff;
        writeBuffer[(*current)->registers[i]->offset + 3] = ((*current)->registers[i]->value >> 24) & 0x00ff;
        bytesToBeSent = (*current)->registers[i]->offset + 3;
      }
      ++i;
    }
    ++bytesToBeSent;
    bytesWritten = WriteWithRetryToMxt(hI2CDriver, address, (UINT16) bytesToBeSent, writeBuffer);

    if (bytesWritten != bytesToBeSent) {
      _stprintf(PrintBuffer, _T("Error writing object configuration to mXT.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    ++current;
  }
  return TRUE;
}

BOOL ParseReadConfigFile(TCHAR *FilePath)
{
#ifdef AUTO_UPDATER
  UNREFERENCED_PARAMETER(FilePath);
  UINT size = LINE_MAX_SIZE-1;
#else
  wfstream cfgFile;
#endif
  TCHAR line[LINE_MAX_SIZE];
  TCHAR objectName[MAX_NAME_SIZE];
  TCHAR instance[MAX_NAME_SIZE];
  int objectType = -1;
  int instanceNumber;
  int registerValue;
  UINT registerIndex = 0;
  TCHAR registerName[MAX_NAME_SIZE];
  OPObject *pCurrentObject = NULL;

  if (!ReadMemoryMap()) return FALSE;

#ifndef AUTO_UPDATER
  cfgFile.open(FilePath, ifstream::in);
  if(!cfgFile.is_open()) {
    _stprintf(PrintBuffer, _T("Cannot open configuration file: %s.\n"), FilePath);
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  // read config file line by line
  // find object first
  while(cfgFile.getline(line, LINE_MAX_SIZE)){
#else
  while(GetLine(line, &size)){
#endif
    if(line[0] == '['){
      // It's a new object
      pCurrentObject = NULL;
      // it is the beginning of an object
      //remove first [
      line[0] = ' ';

      //remove the _T part
      TCHAR* lastUnderscore = _tcsrchr(line, '_');
      if (!lastUnderscore) continue; // not found in this line...
      lastUnderscore[0] = ' ';
      lastUnderscore[1] = ' ';

      //parse and find object name and id
      _stscanf(line, _T("%s %i %s %i"), objectName, &objectType, instance, &instanceNumber);
      if (objectType == DEBUG_DIAGNOSTIC_T37) { 
        //skip this...
        continue;
      }
      pCurrentObject = CreateAndInitObject(objectType, instanceNumber);
      if (!pCurrentObject) {
        _stprintf(PrintBuffer, _T("Found an unknown object type %d in the configuration file. Continuing...\n"), objectType);
        _TPRINTF(PrintBuffer);
        ConfigObjectNotFoundError = TRUE;
        continue;
      }

      ObjectsInFile.push_back(pCurrentObject);
      registerIndex = 0;
    } else if(line[0] == '#'){
      //comment, so do nothing
      continue;
    } else {
      if (!pCurrentObject) continue; // Object not yet created... skip - something wrong with the cfg file
      // it is a register
      //search for = sign, if not found, continue to next line
      TCHAR* pos = _tcschr(line, '=');
      if( pos != 0 ){
        //replace = with space so it can be parsed
        *pos = ' ';
        //parse line
        _stscanf(line, _T("%s %i"), registerName, &registerValue);
        // save register value for current object
        while (pCurrentObject && (registerIndex < pCurrentObject->registers.size())) {
          if (!_tcscmp(pCurrentObject->registers[registerIndex]->name, registerName)) {
            // All is well...
            pCurrentObject->registers[registerIndex]->value = registerValue;
            ++registerIndex;
            break;
          } else  {
            // it's a reserved value
            pCurrentObject->registers[registerIndex]->value = 0;
            ++registerIndex;
          }
        }
      }
    }
  }
#ifndef AUTO_UPDATER
  cfgFile.close();
#endif  
  //  CalculateConfigCRC();
  //  _stprintf(PrintBuffer, _T("\nChecksum extracted from the configuration file to be written is:   0x%06X\n"), ConfigChecksum);
  return TRUE;
}

BOOL ParseReadConfigRawFile(TCHAR *FilePath)
{
  wfstream cfgFile;
  TCHAR line[LINE_MAX_SIZE];
  int objectType = -1;
  int instanceNumber;
  int registerValue, registerValue2, registerValue3, registerValue4;
  int objectSize;
  OPObject *pCurrentObject = NULL;
  BOOL bStarted = FALSE;
  UINT i = 0;
  TCHAR *pLine;
  UINT cfgT35Obj = 0, fwT35Obj = 0;
  UINT address = 0, size = 0;

  if (!ReadMemoryMap()) return FALSE;

  cfgFile.open(FilePath, ifstream::in);
  if(!cfgFile.is_open()) {
    _stprintf(PrintBuffer, _T("Cannot open configuration file: %s.\n"), FilePath);
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  // read config file line by line
  // find object first
  while(cfgFile.getline(line, LINE_MAX_SIZE)){
    if (!bStarted) {
      if (_tcsnicmp(line, _T("OBP_RAW V1"), 10)) {
        continue;
      }
      if(!cfgFile.getline(line, LINE_MAX_SIZE)) {
        // error reading file
        return FALSE;
      }

      _stscanf(line, _T("%x %x %x %x"), &CfgFamilyId, &CfgVariant, &CfgVersion, &CfgBuild);

      for (i = 0; i < 2; ++i) {
        if(!cfgFile.getline(line, LINE_MAX_SIZE)) {
          // error reading file
          return FALSE;
        }
      }
      bStarted = TRUE;
      continue;
    }
    //parse and find object name and id
    _stscanf(line, _T("%x %x %x"), &objectType, &instanceNumber, &objectSize);
    if (objectType == SPT_PROTOTYPE_T35) {
      ++cfgT35Obj;
      for (i = 0; i < NumberOfObjects; ++i) {
        if (AllObjects[i]->Type == objectType) {
          ++fwT35Obj;
          if (cfgT35Obj != fwT35Obj) continue;
          address = AllObjects[i]->LSB | (AllObjects[i]->MSB << 8);
          if (instanceNumber && AllObjects[i]->Instances) {
            address += (UINT16)((AllObjects[i]->Size + 1)*instanceNumber);
          }
          size = (AllObjects[i]->Size + 1);
          fwT35Obj = 0;
          break;
        }
      }
      pCurrentObject = CreateAndInitObject(objectType, instanceNumber, (UINT16)address, (UINT8)size);
    } else {
      pCurrentObject = CreateAndInitObject(objectType, instanceNumber);
    }
    pLine = line + 15;
    for (i = 0; i < (UINT)pCurrentObject->registers.size(); ++i) {
      _stscanf(pLine, _T("%x"), &registerValue);
      pLine += 3;
      if (pCurrentObject->registers[i]->size == 2) {
        _stscanf(pLine, _T("%x"), &registerValue2);
        pLine += 3;
        registerValue |= ((registerValue2 << 8) &0xFF00);
      }
      if (pCurrentObject->registers[i]->size == 4) {
        _stscanf(pLine, _T("%x"), &registerValue2);
        pLine += 3;
        _stscanf(pLine, _T("%x"), &registerValue3);
        pLine += 3;
        _stscanf(pLine, _T("%x"), &registerValue4);
        pLine += 3;
        registerValue |= ((registerValue2 << 8) &0xFF00);
        registerValue |= ((registerValue3 << 16) &0xFF0000);
        registerValue |= ((registerValue4 << 24) &0xFF000000);
      }
      if (pCurrentObject->registers[i]->isSigned) {
        pCurrentObject->registers[i]->value = registerValue;
      } else {
        pCurrentObject->registers[i]->value = registerValue;
      }
    }
    // Save the object
    ObjectsInFile.push_back(pCurrentObject);
  }
  cfgFile.close();
  return TRUE;
}

BOOL ParseReadConfigXFile(TCHAR *FilePath)
{
#ifdef AUTO_UPDATER
  UNREFERENCED_PARAMETER(FilePath);
  UINT size = LINE_MAX_SIZE-1;
#else
  wfstream cfgFile;
#endif
  TCHAR line[LINE_MAX_SIZE];
  TCHAR objectName[MAX_NAME_SIZE];
  TCHAR objectName2[MAX_NAME_SIZE];
  TCHAR instance[MAX_NAME_SIZE];
  TCHAR objectT35AddressName[MAX_NAME_SIZE];
  TCHAR objectT35SizeName[MAX_NAME_SIZE];
  int objectType = -1;
  int instanceNumber;
  int registerValue;
  int objectSize;
  int actualCfgObjSize = 0;
  int regOffset, regSize; // used for extended config file
  UINT registerIndex = 0;
  TCHAR registerName[MAX_NAME_SIZE];
  OPObject *pCurrentObject = NULL;
  BOOL bStarted = FALSE;
  BOOL bCfgObjSizeMismatch = FALSE;
  UINT i = 0;
  UINT addressT35 = 0, address;
  UINT sizeT35 = 0;
  UINT cfgT35Obj = 0, fwT35Obj = 0;
  UINT objSize = 0;

  if (!ReadMemoryMap()) return FALSE;

#ifndef AUTO_UPDATER
  cfgFile.open(FilePath, ifstream::in);
  if(!cfgFile.is_open()) {
    _stprintf(PrintBuffer, _T("Cannot open configuration file: %s.\n"), FilePath);
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
#endif
  // read config file line by line
  // find object first
  do {
    if (!bStarted) {
      // Skip the first part of an xcfg file
#ifndef AUTO_UPDATER
      while(cfgFile.getline(line, LINE_MAX_SIZE)){
#else
      while(GetLine(line, &size)){
#endif
        if (!_tcsnicmp(line, _T("[VERSION_INFO_HEADER]"), 21)) {
          break;
        }
      }
      // Find the first object...
#ifndef AUTO_UPDATER
      while(cfgFile.getline(line, LINE_MAX_SIZE)){
#else
      while(GetLine(line, &size)){
#endif
        if (!_tcsnicmp(line, _T("FAMILY_ID"), 9) || !_tcsnicmp(line, _T("FAMILY"), 6)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %d"), registerName, &CfgFamilyId);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("VARIANT"), 7)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %d"), registerName, &CfgVariant);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("VERSION"), 7)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %d"), registerName, &CfgVersion);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("BUILD"), 5)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %d"), registerName, &CfgBuild);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("CHECKSUM"), 8)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %x"), registerName, &ConfigChecksum);
          }
          continue;
        }
        if(line[0] == '['){
          break;
        }
      }
      bStarted = TRUE;
    }
    if(line[0] == '['){
      // It's a new object
      pCurrentObject = NULL;
      bCfgObjSizeMismatch = FALSE;
      // it is the beginning of an object
      //remove first [
      line[0] = ' ';

      //remove the _T part
      TCHAR* lastUnderscore = _tcsrchr(line, '_');
      if (!lastUnderscore) continue; // not found in this line...
      // save object name
      _stscanf(line, _T("%s %s %i"), objectName, instance, &instanceNumber);

      lastUnderscore[0] = ' ';
      lastUnderscore[1] = ' ';

      //parse and find object name and id
      _stscanf(line, _T("%s %i %s %i"), objectName2, &objectType, instance, &instanceNumber);
      if ((objectType == DEBUG_DIAGNOSTIC_T37) || (objectType == INVALID_OBJECT_TYPE)) { 
        //skip this...
        continue;
      }
      if (objectType == SPT_PROTOTYPE_T35) {
        _stscanf(line, _T("%s %i %s %i %s %i %s %i"), objectName2, &objectType, instance, &instanceNumber, objectT35AddressName, &addressT35, objectT35SizeName, &sizeT35);
        ++cfgT35Obj;
      }

      pCurrentObject = new OPObject(objectName, objectType, instanceNumber);
      registerIndex = 0;
      actualCfgObjSize = 0;
      // Save object address
      for (i = 0; i < NumberOfObjects; ++i) {
        if (AllObjects[i]->Type == objectType) {
          address = AllObjects[i]->LSB | (AllObjects[i]->MSB << 8);
          if (instanceNumber && AllObjects[i]->Instances) {
            address += (UINT16)((AllObjects[i]->Size + 1)*instanceNumber);
          }
          objSize = (AllObjects[i]->Size + 1);
          if (objectType != SPT_PROTOTYPE_T35) {
            pCurrentObject->start_address = (UINT16)address;
            pCurrentObject->objectSize = objSize;
            break;
          } else {
            ++fwT35Obj;
            if ((address == addressT35 && objSize == sizeT35) || ((fwT35Obj == cfgT35Obj) && (objSize == sizeT35))) { 
              pCurrentObject->start_address = (UINT16)address;
              pCurrentObject->objectSize = objSize;
              fwT35Obj = 0;
              break;
            }
            continue;
          }
        }
      }
      if (i == NumberOfObjects) {
        // we had an error: config file has an object not in the FW
        // possibly we have a wrong config file
        delete pCurrentObject;
        pCurrentObject = NULL;
        _stprintf(PrintBuffer, _T("\nObject type %i not found in firmware.\nPlease verify that the config file is meant for this device.\n\n"), objectType);
        _TPRINTF(PrintBuffer);
        continue;
      }
      // Save the object
      ObjectsInFile.push_back(pCurrentObject);
    } else if(line[0] == '#'){
      //comment, so do nothing
      continue;
    } else if (bCfgObjSizeMismatch) {
      continue;
    } else {
      if (!pCurrentObject) continue; // Object not yet created... skip
      if (!_tcsnicmp(line, _T("OBJECT_ADDRESS"), 14)) {
        // skip this
        continue;
      } else if (!_tcsnicmp(line, _T("OBJECT_SIZE"), 11)) {
        //search for = sign, if not found, continue to next line
        TCHAR* pos = _tcschr(line, '=');
        if( pos != 0 ){
          //replace = with space so it can be parsed
          *pos = ' ';
          //parse line
          objectSize = 0;
          _stscanf(line, _T("%s %i"), registerName, &objectSize);
// We don't care of config reported object size, but the actual params
//          if (objectSize != AllObjects[i]->Size + 1) {
//            _stprintf(PrintBuffer, _T("Size mismatch on T%d between config file and FW\n. Please check config file. Continuing.\n"));
//            _TPRINTF(PrintBuffer);
//          }
        } else  {
          continue;
        }
      } else {
        // it is a register
        //search for = sign, if not found, continue to next line
        TCHAR* pos = _tcschr(line, '=');
        if( pos != 0 ){
          //replace = with space so it can be parsed
          *pos = ' ';
          //parse line
          _stscanf(line, _T("%i %i %s %i"), &regOffset, &regSize, registerName, &registerValue);
          actualCfgObjSize += regSize;
          // check if we are exceeding the FW reported object size          
          if (actualCfgObjSize > AllObjects[i]->Size + 1) {
            _stprintf(PrintBuffer, _T("Size mismatch on T%d between config file and FW. \nPlease check config file. Continuing.\n"), AllObjects[i]->Type);
            _TPRINTF(PrintBuffer);
            bCfgObjSizeMismatch = TRUE;
            continue;
          }
          // save register value for current object
          if (pCurrentObject) {
            pCurrentObject->registers.push_back(new OPRegister(registerName, regOffset, regSize));
            pCurrentObject->registers[registerIndex]->value = registerValue;
            ++registerIndex;
          }
        }
      }
    }
#ifndef AUTO_UPDATER
  } while(cfgFile.getline(line, LINE_MAX_SIZE));
  cfgFile.close();
#else  
  } while(GetLine(line, &size));
#endif
  return TRUE;
}

BOOL ParseReadConfigXFileOnly(TCHAR *FilePath)
{
  wfstream cfgFile;
  TCHAR line[LINE_MAX_SIZE];
  TCHAR objectName[MAX_NAME_SIZE];
  TCHAR objectName2[MAX_NAME_SIZE];
  TCHAR instance[MAX_NAME_SIZE];
  int objectType = -1;
  int instanceNumber;
  int registerValue;
  int objectSize;
  UINT address;
  int regOffset, regSize; // used for extended config file
  UINT registerIndex = 0;
  TCHAR registerName[MAX_NAME_SIZE];
  OPObject *pCurrentObject = NULL;
  BOOL bStarted = FALSE;

  cfgFile.open(FilePath, ifstream::in);
  if(!cfgFile.is_open()) {
    _stprintf(PrintBuffer, _T("Cannot open configuration file: %s.\n"), FilePath);
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  // read config file line by line
  // find object first
  do {
    if (!bStarted) {
      // Skip the first part of an xcfg file
      while(cfgFile.getline(line, LINE_MAX_SIZE)){
        if (!_tcsnicmp(line, _T("[VERSION_INFO_HEADER]"), 21)) {
          break;
        }
      }
      // Find the first object...
      while(cfgFile.getline(line, LINE_MAX_SIZE)){
        if (!_tcsnicmp(line, _T("FAMILY_ID"), 9)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %hhi"), registerName, &DeviceMemoryMap.FamilyID);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("VARIANT"), 7)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %hhi"), registerName, &DeviceMemoryMap.VariantID);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("VERSION"), 7)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %hhi"), registerName, &DeviceMemoryMap.Version);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("BUILD"), 5)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %hhi"), registerName, &DeviceMemoryMap.Build);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("CHECKSUM"), 8)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %x"), registerName, &ConfigChecksum);
          }
          continue;
        }
        if (!_tcsnicmp(line, _T("INFO_BLOCK_CHECKSUM"), 19)) {
          //search for = sign, if not found, continue to next line
          TCHAR* pos = _tcschr(line, '=');
          if( pos != 0 ){
            //replace = with space so it can be parsed
            *pos = ' ';
            //parse line
            _stscanf(line, _T("%s %x"), registerName, &InfoBlockChecksum);
          }
          continue;
        }
        if(line[0] == '['){
          break;
        }
      }
      bStarted = TRUE;
    }
    if(line[0] == '['){
      // It's a new object
      pCurrentObject = NULL;
      // it is the beginning of an object
      //remove first [
      line[0] = ' ';

      //remove the _T part
      TCHAR* lastUnderscore = _tcsrchr(line, '_');
      if (!lastUnderscore) continue; // not found in this line...
      // save object name
      _stscanf(line, _T("%s %s %i"), objectName, instance, &instanceNumber);

      lastUnderscore[0] = ' ';
      lastUnderscore[1] = ' ';

      //parse and find object name and id
      _stscanf(line, _T("%s %i %s %i"), objectName2, &objectType, instance, &instanceNumber);
      if ((objectType == DEBUG_DIAGNOSTIC_T37) || (objectType == INVALID_OBJECT_TYPE)) { 
        //skip this...
        continue;
      }
      pCurrentObject = new OPObject(objectName, objectType, instanceNumber);
      registerIndex = 0;
      // Save the object
      ObjectsInFile.push_back(pCurrentObject);
    } else if(line[0] == '#'){
      //comment, so do nothing
      continue;
    } else {
      if (!pCurrentObject) continue; // Object not yet created... skip
      if (!_tcsnicmp(line, _T("OBJECT_ADDRESS"), 14)) {
        //search for = sign, if not found, continue to next line
        TCHAR* pos = _tcschr(line, '=');
        if( pos != 0 ){
          //replace = with space so it can be parsed
          *pos = ' ';
          //parse line
          objectSize = 0;
          _stscanf(line, _T("%s %i"), registerName, &address);
          pCurrentObject->start_address = (UINT16)address;
          continue;
        }
      } else if (!_tcsnicmp(line, _T("OBJECT_SIZE"), 11)) {
        //search for = sign, if not found, continue to next line
        TCHAR* pos = _tcschr(line, '=');
        if( pos != 0 ){
          //replace = with space so it can be parsed
          *pos = ' ';
          //parse line
          objectSize = 0;
          _stscanf(line, _T("%s %i"), registerName, &objectSize);
          pCurrentObject->objectSize = objectSize;
          continue;
        }
      } else {
        // it is a register
        //search for = sign, if not found, continue to next line
        TCHAR* pos = _tcschr(line, '=');
        if( pos != 0 ){
          //replace = with space so it can be parsed
          *pos = ' ';
          //parse line
          _stscanf(line, _T("%i %i %s %i"), &regOffset, &regSize, registerName, &registerValue);
          // save register value for current object
          if (pCurrentObject) {
            pCurrentObject->registers.push_back(new OPRegister(registerName, regOffset, regSize));
            pCurrentObject->registers[registerIndex]->value = registerValue;
            ++registerIndex;
          }
        }
      }
    }
  } while(cfgFile.getline(line, LINE_MAX_SIZE));
  cfgFile.close();

  return TRUE;
}

#if 0
BOOL VerifyConfigWritten()
{
  BOOL bReturnCode;
  UINT i, j, k;

  bReturnCode = ReadConfigFromDevice();
  if (bReturnCode) {
    for (i = 0; i < ObjectsInFile.size(); ++i) {
      for (j = 0; j < ObjectsInFW.size(); ++j) {
        if ((ObjectsInFW[j]->type == ObjectsInFile[i]->type) && (ObjectsInFW[j]->instance == ObjectsInFile[i]->instance)) {
          break;
        }
      }
      for (k = 0; k < ObjectsInFile[i]->registers.size(); ++k) {
        if (ObjectsInFW[j]->registers[k]->size == 1) {
          if ((ObjectsInFW[j]->registers[k]->value & 0x00FF) != (ObjectsInFile[i]->registers[k]->value & 0x00FF)) {
            return FALSE;
          }
        } else {
          if ((ObjectsInFW[j]->registers[k]->value & 0x00FFFF) != (ObjectsInFile[i]->registers[k]->value & 0x00FFFF)) {
            return FALSE;
          }
        }
      }
    }
  }
  return bReturnCode;
}

BOOL VerifyConfigA()
{
  BOOL bReturnCode;
  BOOL bObjExcluded = FALSE;
  UINT i, j, k, n;

  bReturnCode = ReadConfigFromDevice();
// Find the objects to be excluded (Dynamic config) (To be done - now static)
//  for (j = 0; j < ObjectsInFW.size(); ++j) {
//    ObjectsInFW[j]->type == 
  if (bReturnCode) {
    for (i = 0; i < ObjectsInFile.size(); ++i) {
      // skip excluded objects
      n = 0;
      for (;;) {
        if (CfgExcludedObj[n] == 0) break;
        if (CfgExcludedObj[n++] == ObjectsInFile[i]->type) {
          bObjExcluded = TRUE;
          break;
        }
      }
      if (bObjExcluded) {
        bObjExcluded = FALSE;
        continue;
      }

      // If 0 it is a dynamic object and it matches
      // If 1 it is NOT a dynamic object: go on checking
      // If -1 it is a dynamic object and it does not match
//      if (!CompareDynamicObjects(i)) continue;
//      if (CompareDynamicObjects(i) == -1) return FALSE;
        
      for (j = 0; j < ObjectsInFW.size(); ++j) {
        if ((ObjectsInFW[j]->type == ObjectsInFile[i]->type) && (ObjectsInFW[j]->instance == ObjectsInFile[i]->instance)) {
          break;
        }
      }
      // Make sure that they have the same size else config is different
      // and if we don't check we can get a violation when the number of register in FW is smaller...
      if (ObjectsInFile[i]->registers.size() != ObjectsInFW[j]->registers.size()) {
        _tprintf(_T("Objet T%d differs\n"), ObjectsInFW[j]->type);
        return FALSE;
      }
      for (k = 0; k < ObjectsInFile[i]->registers.size(); ++k) {
        if (ObjectsInFW[j]->registers[k]->size == 1) {
          if ((ObjectsInFW[j]->registers[k]->value & 0x00FF) != (ObjectsInFile[i]->registers[k]->value & 0x00FF)) {
            _tprintf(_T("Objet T%d differs\n"), ObjectsInFW[j]->type); 
            return FALSE;
          }
        } else {
          if ((ObjectsInFW[j]->registers[k]->value & 0x00FFFF) != (ObjectsInFile[i]->registers[k]->value & 0x00FFFF)) {
            _tprintf(_T("Objet T%d differs\n"), ObjectsInFW[j]->type); 
            return FALSE;
          }
        }
      }
    }
  }
  return bReturnCode;
}

// This version simply identify dynamic objects to be excluded from compairison
// Next version, it will compare all the possible variations of dynamic objects
BOOL AddDynamicObjects()
{
  UINT i;
  vector<OPObject*>::const_iterator object70;
  vector<OPRegister*>::const_iterator obj70Registers;

  // Sanity check
  if (!ObjectsInFile.size()) return FALSE;
  object70 = ObjectsInFile.begin();

  while (object70 != ObjectsInFile.end()) {
    if ((*object70)->type == SPT_DYNAMICCONFIGURATIONCONTROLLER_T70) {
      obj70Registers = (*object70)->registers.begin();
      // check if enabled
      if (!((*obj70Registers)[eCtrl].value & 1)) {
        // not enabled
        ++object70;
        continue;
      }
      obj70Registers += eObjtype;
      for (i = 0; i < 32; ++i) {
        if (CfgExcludedObj[i] == 0) break;
        if (CfgExcludedObj[i] == (*obj70Registers)[0].value) break;
      }
      if (i == 32) return FALSE;
      // if it is not 0, we already have this object
      if (CfgExcludedObj[i] == 0) {
        CfgExcludedObj[i++] = (*obj70Registers)[0].value;
        CfgExcludedObj[i] = 0;
      }
    }
    ++object70;
  }
  return TRUE;
}


int CompareDynamicObjects(UINT Index)
{
  UNREFERENCED_PARAMETER(Index);
 
  // it is NOT a dynamic object
  return 1;  
}

BOOL CreateDynamicObjects()
{
  BOOL bObj71Found = FALSE;
  BOOL bObjInFileFound = FALSE;
  vector<OPRegister*>::const_iterator obj70Registers;
  vector<OPObject*>::const_iterator object70;
  vector<OPObject*>::const_iterator object71;  
  vector<OPObject*>::const_iterator objectInFile;

  // Sanity check
  if (!ObjectsInFile.size()) return FALSE;

  object70 = ObjectsInFile.begin();
  object71 = ObjectsInFile.begin();
  objectInFile = ObjectsInFile.begin();

  // Find object 71, the dynamic data container
  while (object71 != ObjectsInFile.end()) {
    if ((*object71)->type == SPT_DYNAMICCONFIGURATIONCONTAINER_T71) {
      bobj71Found = TRUE;
      break;
    }
    ++object71;
  }
  // we did not find it
  if (!bObj71Found) return FALSE;

  while (object70 != ObjectsInFile.end()) {
    if ((*object70)->type == SPT_DYNAMICCONFIGURATIONCONTROLLER_T70) {
      obj70Registers = (*object70)->registers.begin();
      // check if active
      if (!((*obj70Registers)[eCtrl].value & 1)) {
        ++object70;
        continue;
      }
      while (objectInFile != ObjectsInFile.end()) {
        if ((*objectInFile)->type == (*obj70Registers)[eObjtype].value) {
          bObjInFileFound = TRUE;
          break;
        }
        ++objectInFile;
      }
      if (bObjInFileFound) {


    while (regcurrent != (*current)->registers.end()) {
      delete (*regcurrent);
      ++regcurrent;
    }
    delete (*current);
    ++current;
  }


  return TRUE;
}
#endif

#ifdef AUTO_UPDATER
BOOL GetLine(TCHAR *Line, UINT *Size)
{
  static int s_counter;
  for (UINT i = 0; i < *Size, s_counter < (int)CfgFileSize; ++i) {
    Line[i] = *CfgFileRes++;
    s_counter++;
    if (Line[i] == 0x0d) {
      Line[i] = 0;
      CfgFileRes++;
      s_counter++;
      *Size = i;
      return TRUE;
    }
  }
  return FALSE;
}
#endif
//---------------------------------------------------------------------------
int WriteData(int B0, int B1, int B2, int B3, int B4, int B5, int B6, int B7)
{
  int i;
  DWORD bytesWritten;
  BOOL bReturncode;

  if(bridge_in_HS_mode)
  {
    // empty the array
    for(i = 0; i < 64; i++)
    {
      usbData_64[i] = 0;
    }

    // in HS mode the RW commands look different - trap this and modify
    if((B0 & 0x80) == 0x00 )
    { // trying to do a R/W
      // should only be talking to I2C device 0 so only need to decode num TX and Rx
      usbData_64[0] = 0;
      usbData_64[1] = 0x51;
      usbData_64[2] = (B0 & 0x07);              //num tx
      usbData_64[3] = ((B0&0x78) >> 3) - 1;     //num rx
      usbData_64[4] = (UINT8)B1;
      usbData_64[5] = (UINT8)B2;
      usbData_64[6] = (UINT8)B3;
      usbData_64[7] = (UINT8)B4;
      usbData_64[8] = (UINT8)B5;
      usbData_64[9] = (UINT8)B6;
      usbData_64[10] = (UINT8)B7;

    }
    else
    {

      // fill with our new data
      usbData_64[0] = 0;
      usbData_64[1] = (UINT8)B0;
      usbData_64[2] = (UINT8)B1;
      usbData_64[3] = (UINT8)B2;
      usbData_64[4] = (UINT8)B3;
      usbData_64[5] = (UINT8)B4;
      usbData_64[6] = (UINT8)B5;
      usbData_64[7] = (UINT8)B6;
      usbData_64[8] = (UINT8)B7;
    }

    //send_usb_report_64(usbData_64);
    // write to device
    bReturncode = WriteFile(mXTDevice.HidDevice, usbData_64, (65), &bytesWritten, NULL);
    Sleep(10);
    if (hReadWriteEvent) SetEvent(hReadWriteEvent);
    return bytesWritten;
  }
  else
  {
    usbData[0] = 0;
    usbData[1] = (UINT8)B0;
    usbData[2] = (UINT8)B1;
    usbData[3] = (UINT8)B2;
    usbData[4] = (UINT8)B3;
    usbData[5] = (UINT8)B4;
    usbData[6] = (UINT8)B5;
    usbData[7] = (UINT8)B6;
    usbData[8] = (UINT8)B7;

    // write to device

    bReturncode = WriteFile(mXTDevice.HidDevice, usbData, 9, &bytesWritten, NULL);
    Sleep(10);
    if (hReadWriteEvent) SetEvent(hReadWriteEvent);
    return bytesWritten;
  }
}

int SendArray(int * DataArray, UINT8 NumberOfBytes)
{
  int i;
  DWORD bytesWritten;
  BOOL bReturncode;

  if(bridge_in_HS_mode)
  {
    // empty the array
    for(i = 0; i < 64; i++)
    {
      usbData_64[i] = 0;
    }

    // fill with our new data
    usbData_64[0] = 0;
    usbData_64[1] = 0x51;
    usbData_64[2] = (UINT8)NumberOfBytes;
    usbData_64[3] = 0; // read 0 bytes

    for(i = 0; i < NumberOfBytes; i++)
    {
      usbData_64[i+4] = (UINT8)DataArray[i];
    }

    //send_usb_report_64(usbData_64);
    // write to device
    bReturncode = WriteFile(mXTDevice.HidDevice, usbData_64, (65), &bytesWritten, NULL);
    Sleep(10);
    if (hReadWriteEvent) SetEvent(hReadWriteEvent);
    return bytesWritten;
  }
  else
  {
    usbData[0] = 0;
    usbData[1] = (UINT8)(0x08 | NumberOfBytes);
    usbData[2] = (UINT8)DataArray[0];
    usbData[3] = (UINT8)DataArray[1];
    usbData[4] = (UINT8)DataArray[2];
    usbData[5] = (UINT8)DataArray[3];
    usbData[6] = (UINT8)DataArray[4];
    usbData[7] = (UINT8)DataArray[5];
    usbData[8] = (UINT8)DataArray[6];

    // write to device
    bReturncode = WriteFile(mXTDevice.HidDevice, usbData, (9),&bytesWritten, NULL);
    Sleep(10);
    if (hReadWriteEvent) SetEvent(hReadWriteEvent);
    return bytesWritten;
  }
}


BOOL ProgramChip()
{
  int CharCountOffset;
  int i;
  int app_id_index = 0;
  int app_id_code;
  int app_build_code;
  TCHAR app_info_string[512];
  TCHAR app_info_string1[512];
  char IdString[ID_STRING_LENGTH];
  bool app_and_bootloader_compatible = true;

#ifndef AUTO_UPDATER
  FILE *EncFirm;

  EncFirm = _tfsopen(EncFile, _T("r"), _SH_DENYWR);

  if( EncFirm == NULL )
  {
    _stprintf(PrintBuffer, _T("Please select a valid program data file\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
#endif

  _stprintf(PrintBuffer, _T("Scanning file...\n"));
  _TPRINTFD(PrintBuffer);
  Sleep(100);

  CharCounter = 0;

  do {//read as characters
#ifndef AUTO_UPDATER
    temp = (UINT8)fgetc(EncFirm);
#else
    temp = (UINT8)*FWFileRes++;
#endif
    if(CharCounter < MAX_CHARS)
    {
      // only fill the legal array region 
      EncData[CharCounter] = integerise(temp);
    }
    else
    {
      // file is too big - exit, close the file and create warning message 

      // force the test below to fail 
      CharCounter = MAX_CHARS + 2u;
      break;
    }

    if (CharCounter < ID_STRING_LENGTH){
      IdString[CharCounter] = (char)temp;
    }
    CharCounter++;

#ifndef AUTO_UPDATER    
  } while(temp!=EOF);

  TotalCharCount =  CharCounter-1;
  fclose(EncFirm);
#else
  } while(CharCounter < (int)FWFileSize);

  temp = EOF;
  EncData[CharCounter] = integerise(temp);
  CharCounter++;

  TotalCharCount =  CharCounter-1;
#endif

  _stprintf(PrintBuffer, _T("Scanning file done!\n"));
  _TPRINTFD(PrintBuffer);

  if(TotalCharCount > MAX_CHARS)
  {
    // there is an error in the file - don't allow download 
    _stprintf(PrintBuffer, _T("File Error: File too large, is the correct file selected?\n"));
    _TPRINTFD(PrintBuffer);
    return FALSE;
  }

  if (strncmp(IdString, "ATML_VIH", ID_STRING_LENGTH) == 0)
  {

    // extract the ID from the header 
    app_id_code = (EncData[ID_STRING_LENGTH] * 16 + EncData[ID_STRING_LENGTH + 1])*256 + EncData[ID_STRING_LENGTH + 2] * 16 + EncData[ID_STRING_LENGTH + 3];

    // Search the list to find this chip's info string 
    for(i = 0u; i < NUM_APPLICATIONS; i++)
    {
      if(application_identifier[i] == app_id_code)
      {
        // found the application 
        app_id_index = i;
        break;
      }
    }

    if(i == NUM_APPLICATIONS)
    {
      // unknown application 
      app_id_index = 0u;
    }
    // got application info 

    // print up the header information and version code 
    _stprintf(app_info_string1, _T("%s V%d.%d"), application_information[app_id_index], EncData[ID_STRING_LENGTH + 4], EncData[ID_STRING_LENGTH + 5]);

    // released build or engineering build? 
    app_build_code = EncData[ID_STRING_LENGTH + 6] * 16 + EncData[ID_STRING_LENGTH + 7];
    if(app_build_code == 0xAAu)
    {
      _stprintf(app_info_string, _T("%s (Release Build)"), app_info_string1);
    }
    else
    {
      _stprintf(app_info_string, _T("%s (Engineering Build 0x%2x)"), app_info_string1, app_build_code);
    }

    _stprintf(PrintBuffer, _T("Application Firmware File: %s\n"), app_info_string);
    _TPRINTFD(PrintBuffer);

    // Subtract 22 bytes of version data
    TotalCharCount -= VERSION_INFO_BYTES;
    CharCountOffset = VERSION_INFO_BYTES;
  }
  else
  {
    CharCountOffset = 0;
  }

  for(CharCounter = 0;CharCounter < TotalCharCount;CharCounter+=2)
  {//turn into true bytes
    ByteData[CharCounter / 2] = ((EncData[CharCounter + CharCountOffset]) * 16) +
      (EncData[CharCounter + CharCountOffset + 1]);
  }

  //parse through and work out how many frames there are to send
  CharCounter = 0;
  total_frames = 0;
  largest_frame_size = 0;
  while(CharCounter < (TotalCharCount/2))
  {
    if(((ByteData[CharCounter])*256)+(ByteData[CharCounter+1]) > largest_frame_size)
    { //track the largest frame size
      largest_frame_size = ((ByteData[CharCounter])*256)+(ByteData[CharCounter+1]);
      if(largest_frame_size > max_frame_size)
      {
        break;
      }
    }
    CharCounter += ((ByteData[CharCounter])*256)+(ByteData[CharCounter+1])+2;

    total_frames++;

    if(total_frames > MAX_FRAMES)
    {// dump out if we have selected an invalid file 
      break;
    }

  }

  if(app_and_bootloader_compatible == false)
  {
    _stprintf(PrintBuffer, _T("File Error: Application firmware not compatible with detected bootloader\n"));
    _TPRINTFD(PrintBuffer);
    return FALSE;
  }
  else if(largest_frame_size > max_frame_size)
  {
    // there is an error in the file - don't allow download 
    _stprintf(PrintBuffer, _T("File Error: At least one frame is too large, is the correct file selected?\n"));
    _TPRINTFD(PrintBuffer);
    return FALSE;
  }
  else if(total_frames == 0)
  {
    // there is an error in the file - don't allow download 
    _stprintf(PrintBuffer, _T("File Error: No Frame data found, is the correct file selected?\n"));
    _TPRINTFD(PrintBuffer);
    return FALSE;
  }
  else if(total_frames > MAX_FRAMES)
  {
    // there is an error in the file - don't allow download 
    _stprintf(PrintBuffer, _T("File Error: Too many Frames found in file, is the correct file selected?\n"));
    _TPRINTFD(PrintBuffer);
    return FALSE;
  }
  else
  {
    // zero the indexes - allows us to bootload another board without quiting the SW
    current_frame = 0;
    current_byte_index = 0;
    current_frame_size = 0;
    current_frame_index = 0;

    DBGPRINT(_T("Dbg - BOOTLOADER_STATE_SENDING_FRAME_DATA: Read state\n"));

    //read the status code and this will kick us into the waiting state
    WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    bootloader_state = BOOTLOADER_STATE_SENDING_FRAME_DATA;
    return TRUE;
  }
}
//---------------------------------------------------------------------------
// This is for the 2952 patch, a mini state machine
#define PATCH_FLUSHING_DONE   1
#define PATCH_FIRST_CL_CHECK  2
#define PATCH_FLUSH_DONE 3

void HIDDeviceData(UINT8 ReportID, UINT8 *Data, UINT Size)
{
  UNREFERENCED_PARAMETER(ReportID);
  UNREFERENCED_PARAMETER(Size);
  if(!bStateOk) return;
  TCHAR printBuffer1[512];
  TCHAR printBuffer2[512];
  UINT8 *data = (UINT8 *) Data;
  UINT8 header = *data++;
  static UINT Flushing2952Patch = 0;
  INT iRetCode;
  string name;
  UNREFERENCED_PARAMETER(iRetCode);

  if(bridge_in_HS_mode)
  { // skip a UINT8 as the packet has an extra UINT8 before the Data
    *data++;
  }
  UINT8 return_data = *data++;
  UINT8 return_data_2 = *data++;
  UINT8 return_data_3 = *data++;

  switch(bootloader_state)
  {
  case BOOTLOADER_STATE_NOT_FOUND:
    {
    }
    break;


  case BOOTLOADER_STATE_NOT_FOUND_SEARCH_ADDR:
    {
      if(header & 0x80)
      { //IND response on setting address
        //do a read
        DBGPRINT(_T("Dbg - BOOTLOADER_STATE_NOT_FOUND_SEARCH_ADDR: Read state\n"));
        WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
      }
      else
      {
        // Data has come back
        //if((header & 0x78) ==  0x00) //chip ACK'ED
        if(((bridge_in_HS_mode == false) && ((header & 0x78) == 0x00)) ||
          ((bridge_in_HS_mode == true ) && ((header & 0x0F) == 0x00)))
        {
          //move state
          bootloader_state = BOOTLOADER_STATE_FOUND_CHECK_VER;
          bootloader_i2c_address = i2c_addresses[i2c_address_index];
          if ((return_data & 0xC0) == 0x40)
          {
            app_crc_fail = true;
          }

          // check if we need to read extended bytes 
          if(return_data & 0x20)
          {
            //read 3 bytes - to get extended ID info
            DBGPRINT(_T("Dbg - BOOTLOADER_STATE_FOUND_CHECK_VER#1: Read state\n"));
            WriteData(0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
          }
          else
          {
            //read 1 UINT8
            DBGPRINT(_T("Dbg - BOOTLOADER_STATE_FOUND_CHECK_VER#2: Read state\n"));
            WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
          }
        }
        else
        {
          // no response from this address - try the next one or stop if we have finished 
          i2c_address_index++;
//          if(i2c_address_index < NUM_I2C_ADDRESSES)
          if(i2c_address_index < sizeof(i2c_addresses))
          {
            //configure next address
            if(ok_to_use_400khz)
            {
              WriteData(0x80, 0x30, i2c_addresses[i2c_address_index], 0x00, 0x00, 0x00, 0x00, 0x00);
            }
            else
            {
              WriteData(0x80, 0x00, i2c_addresses[i2c_address_index], 0x00, 0x00, 0x00, 0x00, 0x00);
            }
          }
          else
          {
            //else - NOT FOUND, this is probably because the application is running
            bootloader_state = BOOTLOADER_STATE_NOT_FOUND;
            //interrogate application firmware to find out the version?
            _stprintf(PrintBuffer, _T("Bootloader not found, Application Firmware is probably running. Make sure Force Flash sequence was followed and search again.\n"));
            _TPRINTFD(PrintBuffer);

          }
        }
      }
    }
    break;

  case BOOTLOADER_STATE_FOUND_CHECK_VER:
    {
      //Data should have come back from the state read, we will have read 3 bytes
      if(header & 0x80)
      {
        // shouldn't be here!
        _stprintf(PrintBuffer, _T("Error\n"));
        _TPRINTFD(PrintBuffer);
      }
      else
      {
        //check state code
        if(return_data & 0xC0)
        {
          //we are in one of the states that stores the version number
          bootloader_id = return_data & 0x3F;

          // extract the reported code so we can put it on screen even if we can't id it 
          unsigned short reported_bootloader_id = bootloader_id;

          // assume ID two chars long unless we are in extended ID mode 
          UINT8 num_id_char = 2u;

          bootloader_version = 0u;
          if(bootloader_id & 0x20)
          {
            // this chip holds its ID in the extended code bytes 
            bootloader_id = return_data_2;
            bootloader_version = return_data_3;
            num_id_char = 4u;

            reported_bootloader_id = bootloader_id;
            realbootloader_id = bootloader_id;
            reported_bootloader_id <<= 8u;
            reported_bootloader_id |= bootloader_version;
            // start counting at 32 to index the arrays easily, ie extended mode id 0 == version 32 
            bootloader_id += 32u;
          }

#ifdef NEW_OLD_BLDR
          if (bootloader_id == 68) {
            FWFileRes = FWFileRes2;
            FWFileSize = FWFileSize2;
          }
#endif

          //update version number on screen
//          if(bootloader_id >= NUM_BOOTLOADER_IDS)
//          {
//            DBGPRINT(_T("Unknown Bootloader ID %d\n"), bootloader_id);
            // unknown bootloader 
//            bootloader_id = 0u;
//          }

          // work out the maximum frame size 
//          max_frame_size = version_decoder_max_frame_size[bootloader_id];

          if (bootloader_id) {
#ifdef AUTO_UPDATER
            DeviceBlrFromXML = FALSE;
#else
            iRetCode = GetDeviceBtlName(realbootloader_id, name);
            // check if success...
            if (!iRetCode) {     //if not successful, use internal hardcoded, outdated data... (very rare)
              _tcscpy_s(DeviceBtlNameFromXML, CA2T(name.c_str()));
              DeviceBlrFromXML = TRUE;  // it either from external XML file or from internal XML resource
            } else {
              DeviceBlrFromXML = FALSE;
            }
#endif
          }
//          max_frame_size = version_decoder_max_frame_size[bootloader_id];

          if (DeviceBlrFromXML) {
            // print up the chip info 
            _stprintf(printBuffer1, _T("Bootloader Type: %s - Bootloader ID:%d"), DeviceBtlNameFromXML, realbootloader_id);
          } else {
            bootloader_id = 0;
            // print up the chip info 
            _stprintf(printBuffer1, _T("Bootloader Type: %s"), version_decoder_name[bootloader_id]);
          }

          if(bootloader_version)
          {
            // add the version code for extended mode chips
            _stprintf(printBuffer2, _T("%s (Version:%d)"), printBuffer1, bootloader_version);
          }
          // print up the raw ID info and I2C address 
          _stprintf(printBuffer1, _T("%s\n(ID: 0x%X"), printBuffer2, reported_bootloader_id);

          if(bootloading_mode == BOOTLOADING_MODE__I2C)
          {
            _stprintf(printBuffer2, _T("%s, SLA: 0x%2x)"), printBuffer1, bootloader_i2c_address);
          }

          _stprintf(printBuffer2, _T("%s, APP: "), printBuffer1);

          if(app_crc_fail)
          {
            _stprintf(printBuffer1, _T("%sCRC FAIL)"), printBuffer2);
          }
          else
          {
            _stprintf(printBuffer1, _T("%sOK)"), printBuffer2);
          }


          _stprintf(PrintBuffer, _T("%s\n"), printBuffer1);
          _TPRINTFD(PrintBuffer);

          switch(return_data & 0xC0)
          {
          case 0x40:
            {
              //invalid APP CRC
              //do another read and we should kick into the waiting unlock state
              bootloader_state = BOOTLOADER_STATE_WAIT_UNLOCK_CMD;
              DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_UNLOCK_CMD - case 0x40: Read state\n"));
              WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            }
            break;
          case 0x80:
            {
              //Waiting frame Data - somehow the chip has bypassed the unlock command, or we have caught it midway through!
              _stprintf(PrintBuffer, _T("Error: Bootloader not in expected state - bootloading is already in progress.\r\nExpected state was 0xCn or 0x4n, Got State Code: 0x%2x.\r\nPlease retry or power-cycle the device and start again.\r\n"), return_data);
              _TPRINTFD(PrintBuffer);
              //send 0x00 0x00 to send a soft reset command
#ifdef AUTO_UPDATER
              _tprintf(_T("Error: Please power-cycle the device and try again.\r\n"));
#endif
              DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_UNLOCK_CMD - case 0x80: Read state\n"));
              WriteData(0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
              bStateOk = FALSE;
            }
            break;

          case 0xC0:
            {
              //we are waiting the unlock command - do another read so we sync up
              // with the APP CRC fail case
              bootloader_state = BOOTLOADER_STATE_WAIT_UNLOCK_CMD;
              DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_UNLOCK_CMD - case 0xC0: Read state\n"));
              WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            }
            break;
          }
        }
        else
        {
          //state code doesn't give us the version number - means we have caught it midway through loading! not good
          _stprintf(PrintBuffer, _T("Error: Bootloader in unexpected state, expected state was 0xCn, 0x8n or 0x4n Got State Code: 0x%2x.\nPlease retry or power-cycle the device and start again.\n"), return_data);
          _TPRINTFD(PrintBuffer);
          //send 0x00 0x00 to send a soft reset command
#ifdef AUTO_UPDATER
          _tprintf(_T("Error: Please power-cycle the device and try again.\n"));
#endif
          WriteData(0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
          bStateOk = FALSE;
        }
      }

    }
    break;
    //  case BOOTLOADER_STATE_APPCRC_FAIL:
    // {
    // }break;
  case BOOTLOADER_STATE_WAIT_UNLOCK_CMD:
    {
      //if(((header & 0x78) == 0x00) && ((return_data & 0xC0) == 0xC0))
      if(((((bridge_in_HS_mode == false) && ((header & 0x78) == 0x00)) ||
        ((bridge_in_HS_mode == true ) && ((header & 0x0F) == 0x00)))) && ((return_data & 0xC0) == 0xC0))
        //send the unlock command
      {
        bootloader_state = BOOTLOADER_STATE_READ_BEFORE_WAIT_FRAME;
        if (CheckBootloaderOnly) {
          if (!LeaveInBootloaderMode) {
            // Exit bootloader mode
            WriteData(0x0A, 0xDD, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00);
          }
          // signal main program that we are done...
          if (hCompletionEvent) {
            SetEvent(hCompletionEvent);
          }
        } else {
          DBGPRINT(_T("Dbg - BOOTLOADER_STATE_READ_BEFORE_WAIT_FRAME: Unlock bootloader\n"));
          WriteData(0x0A, 0xDC, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00);
        }
      }
      else
      {
        //we expected to be in the waiting unlock state but weren't!
        _stprintf(PrintBuffer, _T("Error: Bootloader in unexpected state, expected state was 0xCn, Got State Code: 0x%2x.\nPlease retry or power-cycle the device and start again.\n"), return_data);
        _TPRINTFD(PrintBuffer);
        //send 0x00 0x00 to send a soft reset command
#ifdef AUTO_UPDATER
        _tprintf(_T("Error: Please power-cycle the device and try again.\n"));
#endif
        DBGPRINT(_T("Dbg - Bootloader in unexpected state #1\n"));
        WriteData(0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        bStateOk = FALSE;
      }
    } break;

  case BOOTLOADER_STATE_READ_BEFORE_WAIT_FRAME:
    {
      if(chg_is_driven_low)
      {
        // float change line in case it was driven 
        DBGPRINT(_T("Dbg - BOOTLOADER_STATE_READ_BEFORE_WAIT_FRAME: WriteData 0x83\n"));
        WriteData(0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        if (!MaxTouchAnalyzer) Sleep(20);  // Reduced delay from 100 following Miracle's recommendation (undid it 10/02/14, due to flashing failures)
        chg_is_driven_low = false;
        // dump out and come in again 
        break;
      }

      if(ok_to_use_change_line)
      {
        DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_CHANGE_LINE: WriteData 0x82\n"));
        WriteData(0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        bootloader_next_state = BOOTLOADER_STATE_WAIT_FRAME;
        bootloader_state = BOOTLOADER_STATE_WAIT_CHANGE_LINE;
      }
      else
      {
        if (!MaxTouchAnalyzer) Sleep(10);  // Reduced delay from 50 following Miracle's recommendation (undid it 10/02/14, due to flashing failures)
        //we need to do a read to confirm we are in the right state
        WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        bootloader_state = BOOTLOADER_STATE_WAIT_FRAME;
      }
    }
    break;

  case BOOTLOADER_STATE_WAIT_FRAME:
    {
      if((return_data & 0xC0) == 0x80)
      {
        //we are in the right state
        //ready to send data!
      }
      else
      {
        //wrong state!
        _stprintf(PrintBuffer, _T("Error: Bootloader in unexpected state, expected state was 0x8n, Got State Code: 0x%2x.\nPlease retry or power-cycle the device and start again.\n"), return_data);
        _TPRINTFD(PrintBuffer);
        //send 0x00 0x00 to send a soft reset command
#ifdef AUTO_UPDATER
        _tprintf(_T("Error: Please power-cycle the device and try again.\n"));
#endif
        DBGPRINT(_T("Dbg - Bootloader in unexpected state #2\n"));
        WriteData(0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        bStateOk = FALSE;
      }
    }
    break;

  case BOOTLOADER_STATE_SENDING_FRAME_DATA:
    {//if we got here via a read check our state code is correct
      //we have just started or are midway through sending a frame
      //if((header & 0x18) ==  0x00) //sent ok
      if(((bridge_in_HS_mode == false) && ((header & 0x18) == 0x00)) ||
        ((bridge_in_HS_mode == true ) && ((header & 0x03) == 0x00)))
      {
        if(current_frame_index == 0)
        {
          if(bFirstTime) {
            if (!SilentMode) _tprintf(_T("Downloading firmware 00%c done"), '%');
            if (MaxTouchAnalyzer || MaxTouchStudio) _TPRINTF(_T("\n"));
            bFirstTime = false;
          }
          //we starting a new frame?
          if ((((current_frame+1)*100)/total_frames) == 100) {
            Flushing2952Patch = PATCH_FLUSHING_DONE;
          }
          if (MaxTouchAnalyzer || MaxTouchStudio) {
            _tprintf(_T("%2d%c done"), (((current_frame+1)*100)/total_frames), '%');
            _TPRINTFD(_T("\n"));
          } else {  
            if (!SilentMode) _tprintf(_T("\b\b\b\b\b\b\b\b%2d%c done"), (((current_frame+1)*100)/total_frames), '%');
          }
          //get the new frame size
          current_frame_size = ((ByteData[current_byte_index])*256)+(ByteData[current_byte_index+1])+2;
        }

        if((current_frame_size - current_frame_index) > number_of_bytes_per_write)
        {
          SendArray(&ByteData[current_byte_index], (UINT8)number_of_bytes_per_write);
          current_frame_index += number_of_bytes_per_write;
          current_byte_index += number_of_bytes_per_write;

        }
        else if(current_frame_size == current_frame_index)
        {
          //the whole frame has been sent - read the state code
          //when the 400khz bridge works correctly we can go on the change line - at the moment it won't read back the state os PD4 correctly
          if(ok_to_use_change_line)
          {
            DBGPRINT(_T("Dbg - BOOTLOADER_STATE_SENDING_FRAME_DATA: WriteData 0x82\n"));
            WriteData(0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            bootloader_next_state = BOOTLOADER_STATE_WAIT_CRC_CHECK;
            bootloader_state = BOOTLOADER_STATE_WAIT_CHANGE_LINE;
          }
          else
          {
            if (!MaxTouchAnalyzer) Sleep(20);  // Reduced delay from 100 following Miracle's recommendation (undid it 10/02/14, due to flashing failures)
            WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            bootloader_state = BOOTLOADER_STATE_WAIT_CRC_CHECK;
          }

          current_frame++;
          current_frame_index = 0;
        }
        else
        {
          SendArray(&ByteData[current_byte_index], (UINT8)(current_frame_size - current_frame_index));
          current_byte_index += (current_frame_size - current_frame_index);
          current_frame_index = current_frame_size; //all sent now
        }
      }
      else
      {
        //didn't transmit ok!
        _stprintf(PrintBuffer, _T("Error: Frame Transmission failed\n"));
        _TPRINTFD(PrintBuffer);
        bStateOk = FALSE;
      }
    }
    break;

  case BOOTLOADER_STATE_WAIT_CRC_CHECK:
    {
      if(return_data == 0x02)
      {
        // we have sent the whole frame - read again to make the CRC check happen 
        //when the 400khz bridge works correctly we can go on the change line - at the moment it won't read back the state os PD4 correctly
        if(ok_to_use_change_line)
        {
          DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_CRC_CHECK: WriteData 0x82\n"));
          WriteData(0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
          bootloader_next_state = BOOTLOADER_STATE_WAIT_CRC_RESULT;
          bootloader_state = BOOTLOADER_STATE_WAIT_CHANGE_LINE;
        }
        else
        {
          Sleep(20);  // Reduced delay from 100 following Miracle's recommendation (undid it 10/02/14, due to flashing failures)
          WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
          bootloader_state = BOOTLOADER_STATE_WAIT_CRC_RESULT;
        }
      }
      else
      {
        // something has gone wrong - we are not in sync 
        _stprintf(PrintBuffer, _T("Error: Bootloader in unexpected state, expected state was 0x02, Got State Code: 0x%2x.\nPlease retry or power-cycle the device and start again.\n"), return_data);
        _TPRINTFD(PrintBuffer);
        //send 0x00 0x00 to send a soft reset command
#ifdef AUTO_UPDATER
        _tprintf(_T("Error: Please power-cycle the device and try again.\n"));
#endif
        DBGPRINT(_T("Dbg - Bootloader in unexpected state #3\n"));
        WriteData(0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        bStateOk = FALSE;
      }
    }
    break;

  case BOOTLOADER_STATE_WAIT_CRC_RESULT:
    {
      if(return_data == 0x03)
      {

        crc_fail_counter++;
        if(crc_fail_counter > 5)
        {
          _stprintf(PrintBuffer, _T("Error: Frame CRC Check failed 5 times in a row - .enc file is corrupt or invalid. \nFailed on frame number: %d.\n.Suggest checking file, Power-cycling device and starting again"), (current_frame - 1));
          _TPRINTFD(PrintBuffer);
          bStateOk = FALSE;
        }
        else
        {
          //re-try sending the data to the chip as the CRC check failed
          current_frame--;
          current_byte_index -= current_frame_size;

          //wait for change line to activate as the program step may take a while esp for eeprom
          //when the 400khz bridge works correctly we can go on the change line - at the moment it won't read back the state os PD4 correctly
          if(ok_to_use_change_line)
          {
            DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_CRC_RESULT: WriteData 0x82\n"));
            WriteData(0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            bootloader_next_state = BOOTLOADER_STATE_SENDING_FRAME_DATA;
            bootloader_state = BOOTLOADER_STATE_WAIT_CHANGE_LINE;
          }
          else
          {
            if (!MaxTouchAnalyzer) {
              Sleep(200);  // Reduced delay from 800 following Miracle's recommendation (undid it 10/02/14, due to flashing failures)
            } else {
              Sleep(50);
            }
            WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            bootloader_state = BOOTLOADER_STATE_SENDING_FRAME_DATA;
          }
        }
      }
      else if(return_data == 0x04)
      {
        crc_fail_counter = 0; //zero the counter
        // have we finished sending all of the frames?
        if(current_frame >= total_frames)
        {

//#if 0
          // chip will now reset - download complete
          // check if patch on 2952T
          if (DeviceConnected != eMXT2952T || Flushing2952Patch != PATCH_FLUSH_DONE) {
            _stprintf(PrintBuffer, _T("\nDownloading firmware update... DONE!\n"));
            _TPRINTF(PrintBuffer);
            Sleep(1000);
            // signal main program that we are done...
            if (hCompletionEvent) {
              SetEvent(hCompletionEvent);
            } else {
              _stprintf(PrintBuffer, _T("\nReceived  CRC result\n"));
              _TPRINTFD(PrintBuffer);
            }
          }
//#endif 0
          // do we want to check that the application is now up and running?? 
        }
        else
        {
          //wait for change line to activate as the program step may take a while esp for eeprom

          //when the 400khz bridge works correctly we can go on the change line - at the moment it won't read back the state os PD4 correctly
          if(ok_to_use_change_line)
          {
            DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_CRC_RESULT: WriteData 0x82 -ret: 0x04\n"));
            WriteData(0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            bootloader_next_state = BOOTLOADER_STATE_SENDING_FRAME_DATA;
            bootloader_state = BOOTLOADER_STATE_WAIT_CHANGE_LINE;
          }
          else
          {
            if (MaxTouchAnalyzer) {
              Sleep(50);
            } else {
              Sleep(200);  // Reduced delay from 800 following Miracle's recommendation (undid it 10/02/14, due to flashing failures)
            }
            WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
            bootloader_state = BOOTLOADER_STATE_SENDING_FRAME_DATA;
          }
        }
      }
      else
      {
        _stprintf(PrintBuffer, _T("Error: Bootloader in unexpected state, expected state was 0x03 or 0x04, Got State Code: 0x%2x, Failed on frame number: %d.\nPlease retry or power-cycle the device and start again.\n"), return_data, (current_frame - 1));
        _TPRINTFD(PrintBuffer);
        //send 0x00 0x00 to send a soft reset command
        DBGPRINT(_T("Dbg - Bootloader in unexpected state #4\n"));
        WriteData(0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
        bStateOk = FALSE;
      }
    }
    break;

    //when the 400khz bridge works correctly we can go on the change line - at the moment it won't read back the state os PD4 correctly
  case BOOTLOADER_STATE_WAIT_CHANGE_LINE:
    {
      //is the pin low??
      //if((header&0xFF) == 0x82)
      {
        if(((bridge_in_HS_mode == false) && ((return_data_2 & 0x04) == 0x00)) ||
          ((bridge_in_HS_mode == true ) && ((return_data & 0x04) == 0x00)))
        {
          change_line_high_counter = 0;//reset timeout counter
          // pin is low so clock on 
          bootloader_state = bootloader_next_state;
          // read the state code 
          DBGPRINT(_T("Dbg - BOOTLOADER_STATE_WAIT_CHANGE_LINE\n"));
          WriteData(0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
//#if 0
          // Patch for 2952T:
          // We never get the reply to CRC_RESULT and it hangs indefinitely
 
          if (DeviceConnected == eMXT2952T) {
            if (Flushing2952Patch == PATCH_FLUSHING_DONE) {
              Flushing2952Patch = PATCH_FIRST_CL_CHECK;
            } else if (Flushing2952Patch == PATCH_FIRST_CL_CHECK) {
               Flushing2952Patch = PATCH_FLUSH_DONE;
              // chip will now reset - download complete
              _stprintf(PrintBuffer, _T("\nDownloading firmware update... DONE!\n"));
              _TPRINTFD(PrintBuffer);
              Sleep(1000);
              // signal main program that we are done...
              if (hCompletionEvent) {
                SetEvent(hCompletionEvent);
              }
            }
          }
//#endif
        }
        else
        {
          change_line_high_counter++;
          if(change_line_high_counter > 1000)
          {
            _stprintf(PrintBuffer, _T("Error: CHANGE line did not go low.\nSuggest checking board connections and starting again\n"));
            _TPRINTFD(PrintBuffer);
            bStateOk = FALSE;
          }
          else
          {
            Sleep(10);
            //check line again
            DBGPRINT(_T("Dbg - check line again\n"));
            WriteData(0x82, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
          }
        }
      }
      //else
      //{
      //unexpected response
      //  assert(0);
      //}
    }
    break;

  default:
    {
      //shouldn't get here
      _stprintf(PrintBuffer, _T("Error: Software Got into unexpected state - restart required\nProgram will close\n"));
      _TPRINTFD(PrintBuffer);
#ifdef AUTO_UPDATER
      _tprintf(_T("Error: Please power-cycle the device and try again.\n"));
#endif
      return;
    }
    break;
  }
  if (!bStateOk) {
    // We need to terminate here...
    // signal main program that we are done...
    if (hCompletionEvent) {
      SetEvent(hCompletionEvent);
    }
  }
}

//---------------------------------------------------------------------------
void DisplayEnumeratedDevices()
{
  TCHAR buffer[256];
  BOOL bReturncode = TRUE;
  int i = 1;
  if (HidDevice[FIRST_DISCOVERY].size() == 0) {
    bReturncode = FindHidDevices();
  }
  // Display all the HID devices
  if (bReturncode) {
    _stprintf(PrintBuffer, _T("Enumeration returned a total of %d Atmel maXTouch devices/interfaces.\n\n"), (int)HidDevice[FIRST_DISCOVERY].size());
    _TPRINTF(PrintBuffer);
    vector<HID_DEVICE *>::iterator devCurrent = HidDevice[FIRST_DISCOVERY].begin();
    while (devCurrent != HidDevice[FIRST_DISCOVERY].end()) {
      _stprintf(PrintBuffer, _T("Device #%d:\n"), i);
      _TPRINTF(PrintBuffer);
      _stprintf(PrintBuffer, _T("Device Path: %s\n"), (*devCurrent)->DevicePath); 
      _TPRINTF(PrintBuffer);
      _stprintf(PrintBuffer, _T("Device Vendor ID: %x\n"), (*devCurrent)->Attributes.VendorID);
      _TPRINTF(PrintBuffer);
      _stprintf(PrintBuffer, _T("Device Product ID: %x\n"), (*devCurrent)->Attributes.ProductID);
      _TPRINTF(PrintBuffer);
      bReturncode = HidD_GetProductString((*devCurrent)->HidDevice, buffer, 256);
      if (bReturncode) {
        _stprintf(PrintBuffer, _T("Device String: %s\n"), buffer);
        _TPRINTF(PrintBuffer);
      }
      ++i;
      ++devCurrent;
    }
  }
  // Clean-up
  if (HidDevice[FIRST_DISCOVERY].size()) {
    FreeMemHidDevices(FIRST_DISCOVERY);
  }
  if (HidDevice[SECOND_DISCOVERY].size()) {
    FreeMemHidDevices(SECOND_DISCOVERY);
  }
}

//---------------------------------------------------------------------------

void NormalizeCustomString(char *InputString)
{
  UINT length;
  
  length = (UINT)strlen(InputString);
  length = min(length, 20);
  // Add extra termination just in case...
  InputString[length] = '\0';

  for (UINT i = 19; i > 0; --i) {
    if (InputString[i] != ' ' && InputString[i] != '\0') {
      InputString[i+1] = '\0';
      break;
    }
  }
}

int GetIndexFromCustomString(char *StringToFind)
{
  char customString[128];
  int index = 0;
  int foundDeviceIndex = 0;
  BOOL bFound = FALSE;
  BOOL bReturncode = TRUE;

  if (HidDevice[FIRST_DISCOVERY].size() == 0) {
    bReturncode = FindHidDevices();
  }
  // List the control interface of MaxTouch devices
  if (bReturncode) {
    vector<MAXTOUCHDEVICES *>::iterator mxtDevCurrent = MxtCtrlDevice[FIRST_DISCOVERY].begin();
    while (mxtDevCurrent != MxtCtrlDevice[FIRST_DISCOVERY].end()) {
      ++index;
      DeviceIndex = index;
      if(GetT60String((*mxtDevCurrent), customString)) {
        if (!strcmp(StringToFind, customString)) {
          // Found!
          if (bFound) {
            // there is already a device with the same serial #
            index = -1;
            return(index);
          }
          foundDeviceIndex = index;
          bFound = TRUE;
        }
      }
      ++mxtDevCurrent;
    }
    if (!bFound) {
      index = 0;
    }
  }
  return(foundDeviceIndex);
}

void DisplayMaxTouchControlDevices()
{
  TCHAR buffer[256];
  char customString[128];

  BOOL bReturncode = TRUE;

  if (HidDevice[FIRST_DISCOVERY].size() == 0) {
    bReturncode = FindHidDevices();
  }
  // List the control interface of MaxTouch devices
  if (bReturncode) {
    _stprintf(PrintBuffer, _T("List of connected Atmel maXTouch devices:\n\n"));
    _TPRINTF(PrintBuffer);
    vector<MAXTOUCHDEVICES *>::iterator mxtDevCurrent = MxtCtrlDevice[FIRST_DISCOVERY].begin();
    while (mxtDevCurrent != MxtCtrlDevice[FIRST_DISCOVERY].end()) {
      _stprintf(PrintBuffer, _T("Device #%d:\n"), (*mxtDevCurrent)->DeviceIndex);
      _TPRINTF(PrintBuffer);
      _stprintf(PrintBuffer, _T("Device Path: %s\n"), ((*mxtDevCurrent)->DeviceData)->DevicePath); 
      _TPRINTF(PrintBuffer);
      _stprintf(PrintBuffer, _T("Device Product ID: %x\n"), ((*mxtDevCurrent)->DeviceData)->Attributes.ProductID);
      _TPRINTF(PrintBuffer);
      bReturncode = HidD_GetProductString(((*mxtDevCurrent)->DeviceData)->HidDevice, buffer, 256);
      if (bReturncode) {
        _stprintf(PrintBuffer, _T("Device String: %s\n"), buffer);
        _TPRINTF(PrintBuffer);
      }
      if(GetT60String((*mxtDevCurrent), customString)) {
        printf("Custom String: \"%s\"\n", customString);
      }
      ++mxtDevCurrent;
    }
  }
  // Clean-up
  if (HidDevice[FIRST_DISCOVERY].size()) {
    FreeMemHidDevices(FIRST_DISCOVERY);
  }
  if (HidDevice[SECOND_DISCOVERY].size()) {
    FreeMemHidDevices(SECOND_DISCOVERY);
  }
}

void DisplayMaXTouchPIDs()
{
  UINT maxTouchDevices;
  maxTouchDevices = (UINT)MxtCtrlDevice[FIRST_DISCOVERY].size();

  if (maxTouchDevices > 1) {
    _stprintf(PrintBuffer, _T("There are %d MaxTouch devices connected.\nThese are the PIDs:\n"), maxTouchDevices);
    _TPRINTF(PrintBuffer);
    for (UINT j = 0; j < maxTouchDevices; ++j) {
      _stprintf(PrintBuffer, _T("Device #%d: PID: %4X\n"), j+1, AllPids[j]);
      _TPRINTF(PrintBuffer);
    }
  } else {
    _stprintf(PrintBuffer, _T("MaxTouch device PID: 0x%04X\n"), AllPids[0]);
    _TPRINTF(PrintBuffer);
  }
}

BOOL GetT60String(MAXTOUCHDEVICES *MxtDevice, char *CustomStringT60)
{
  UINT16 address;
  DWORD byteRead = 0;
  BOOL retCode;
  
  AtmelReportId = (UINT8)MxtDevice->ReportId;
  ConnectFWType = MxtDevice->ConnectFWType;

  mXTDevice.InputReportByteLength = (MxtDevice->DeviceData)->Caps.InputReportByteLength;
  mXTDevice.OutputReportByteLength = (MxtDevice->DeviceData)->Caps.OutputReportByteLength;
  mXTDevice.HidDevice = CreateFile ((MxtDevice->DeviceData)->DevicePath,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,          // no SECURITY_ATTRIBUTES structure
    OPEN_EXISTING, // No special create flags
    0,             // Open device as non-overlapped so we can get data
    NULL);         // No template file

  if (mXTDevice.HidDevice == INVALID_HANDLE_VALUE) {
    return FALSE;
  }

  if (!ReadMemoryMap()) return FALSE;
  if (!GetObjectAddressAndSize(SPT_PRODUCTIONCUSTOM_T60, &address, (UINT8 *)&byteRead)) return FALSE;
  address += 2;
  byteRead -= 2;
  retCode = ReadFromDevice(address, (UINT8 *)CustomStringT60, &byteRead);
  if (!retCode) return FALSE;
  CustomStringT60[byteRead] = '\0';
  NormalizeCustomString(CustomStringT60);
  return TRUE;
}


//---------------------------------------------------------------------------
BOOL FindHIDI2CDeviceByDevPath(const TCHAR *AtmelString)
{
  BOOL bReturncode = TRUE, bFound = FALSE;
  int length;

  AtmelReportId = ATMEL_REPORT_ID_I2C;
  ConnectFWType = HIDI2C;

  if (HidDevice[FIRST_DISCOVERY].size() == 0) {
    bReturncode = FindHidDevices();
  }

  vector<HID_DEVICE *>::iterator devCurrent = HidDevice[FIRST_DISCOVERY].begin();
  // Find mXT1386/768E among the HID devices
  if (bReturncode) {
    while (devCurrent != HidDevice[FIRST_DISCOVERY].end()) {
      length = (int)_tcslen(AtmelString);
      if (!_tcsnicmp((*devCurrent)->DevicePath, AtmelString, length)) {
        bFound = TRUE; // save our device
        break;
      }
      ++devCurrent;
    }
  }
  if (!bFound) {
    return FALSE;
  }
  // Close handle if previously open...
  if (mXTDevice.HidDevice && (mXTDevice.HidDevice != INVALID_HANDLE_VALUE)) {
    CloseHandle(mXTDevice.HidDevice);
  }
  mXTDevice.InputReportByteLength = (*devCurrent)->Caps.InputReportByteLength;
  mXTDevice.OutputReportByteLength = (*devCurrent)->Caps.OutputReportByteLength;
  mXTDevice.HidDevice = CreateFile ((*devCurrent)->DevicePath,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,          // no SECURITY_ATTRIBUTES structure
    OPEN_EXISTING, // No special create flags
    0,             // Open device as non-overlapped so we can get data
    NULL);         // No template file


  if (mXTDevice.HidDevice == INVALID_HANDLE_VALUE) {
    _stprintf(PrintBuffer, _T("Cannot open for read/write device - error: %d.\n"), GetLastError());
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  return TRUE;
}

//
// Tries to find a device in Normal or Bootloader mode, USB or I2C regularly discoverable
// Open handle for read/write if successful
// Sets AtmelReportId, ConnectFWType, and returns one of:
// USB_NORMAL, USB_BOOTLOADER, I2C_NORMAL, I2C_BOOTLOADER or -1 on error
//
int FindDiscoverableDevice(int Order)
{
  //  TCHAR buffer[256];
  HID_DEVICE *pSelectedDevice = NULL;
  int iRetCode = NOT_FOUND;
  BOOL bReturncode = TRUE;
  BOOL bFoundSelected = FALSE;
  vector<MAXTOUCHDEVICES *>::iterator mxtDevCurrent, mxtDevCurrent2;

  if (HidDevice[Order].size() == 0) {
    bReturncode = FindHidDevices(Order);
  }

  // Close handle if previously open...
  if (mXTDevice.HidDevice && (mXTDevice.HidDevice != INVALID_HANDLE_VALUE)) {
    CloseHandle(mXTDevice.HidDevice);
    mXTDevice.HidDevice = 0;
  }
  if (Order) {
    //do 2nd discovery stuff
    mxtDevCurrent = MxtCtrlDevice[FIRST_DISCOVERY].begin();
    mxtDevCurrent2 = MxtCtrlDevice[SECOND_DISCOVERY].begin();
    // Try this to solve some corner cases
    if (DeviceInBootloader[0] == 0 && DeviceInBootloader[1] == 1) {
      // There is only one device in Bootloader mode and it is our device
      while (mxtDevCurrent2 != MxtCtrlDevice[SECOND_DISCOVERY].end()) {
        pSelectedDevice = (*mxtDevCurrent2)->DeviceData;
        if (pSelectedDevice->Attributes.ProductID == 0x211D) {
          bFoundSelected = TRUE;
          break;
        }
        ++mxtDevCurrent2;
      }
    }
    if (!bFoundSelected) {
      // we are not sure on the device (more than one in BL mode)
      // go standard way
      mxtDevCurrent2 = MxtCtrlDevice[SECOND_DISCOVERY].begin();
      while (mxtDevCurrent2 != MxtCtrlDevice[SECOND_DISCOVERY].end()) {
        while (mxtDevCurrent != MxtCtrlDevice[FIRST_DISCOVERY].end()) {
          pSelectedDevice = (*mxtDevCurrent2)->DeviceData;
          if (!_tcsicmp(pSelectedDevice->DevicePath, ((*mxtDevCurrent)->DeviceData)->DevicePath)) {
            break;
          }
          ++mxtDevCurrent;
        }
        if (mxtDevCurrent == MxtCtrlDevice[FIRST_DISCOVERY].end()) {
			bFoundSelected = TRUE;
          break;
        }
        ++mxtDevCurrent2;
      }
    }
    if (bFoundSelected) {
      iRetCode = (*mxtDevCurrent2)->DeviceType;
      AtmelReportId = (UINT8)(*mxtDevCurrent2)->ReportId;
      ConnectFWType = (*mxtDevCurrent2)->ConnectFWType;
      mXTDevice.InputReportByteLength = ((*mxtDevCurrent2)->DeviceData)->Caps.InputReportByteLength;
      mXTDevice.OutputReportByteLength = ((*mxtDevCurrent2)->DeviceData)->Caps.OutputReportByteLength;
    } else {
 //     _stprintf(PrintBuffer, _T("Invalid maXTouch device index provided.\n"));
 //     _TPRINTF(PrintBuffer);
      return iRetCode;
    }
  } else {
    mxtDevCurrent = MxtCtrlDevice[Order].begin();

    while (mxtDevCurrent != MxtCtrlDevice[Order].end()) {
      if ((*mxtDevCurrent)->DeviceIndex == DeviceIndex) {
        break;
      }
      ++mxtDevCurrent;
    }
    if (mxtDevCurrent == MxtCtrlDevice[Order].end()) {
//      _stprintf(PrintBuffer, _T("Invalid maXTouch device index provided.\n"));
//      _TPRINTF(PrintBuffer);
      return iRetCode;
    }

    iRetCode = (*mxtDevCurrent)->DeviceType;
    AtmelReportId = (UINT8)(*mxtDevCurrent)->ReportId;
    ConnectFWType = (*mxtDevCurrent)->ConnectFWType;
	DevicePID = (*mxtDevCurrent)->DeviceData->Attributes.ProductID;		// 2016/06/06. Miracle. For HIDI2C BL.

    mXTDevice.InputReportByteLength = ((*mxtDevCurrent)->DeviceData)->Caps.InputReportByteLength;
    mXTDevice.OutputReportByteLength = ((*mxtDevCurrent)->DeviceData)->Caps.OutputReportByteLength;
    pSelectedDevice = (*mxtDevCurrent)->DeviceData;
  }
  mXTDevice.HidDevice = CreateFile (pSelectedDevice->DevicePath,
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    NULL,          // no SECURITY_ATTRIBUTES structure
    OPEN_EXISTING, // No special create flags
    0,             // Open device as non-overlapped so we can get data
    NULL);         // No template file

  if (mXTDevice.HidDevice == INVALID_HANDLE_VALUE) {
    _stprintf(PrintBuffer, _T("Cannot open for read/write device - error: %d.\n"), GetLastError());
    _TPRINTF(PrintBuffer);
    return -1;
  }

  // Added by Miracle.
  if (ConnectFWType == I2CBRIDGE) {
    if (DeviceDiscovered == I2C_BOOTLOADER) {
      iRetCode = I2C_BOOTLOADER;
    }
    else {
      iRetCode = USB_NORMAL;
    }
  }
  return iRetCode;
}

void __cdecl ReadDevice (void *dummy)
{
  UNREFERENCED_PARAMETER(dummy);
  BOOL bReturncode = FALSE;
  UINT8 readBuffer[65];
  DWORD bytesRead;
  BOOL bLoop = TRUE;
  static UINT USBDelay = 10;

  // Loop forever reading device
  while(bLoop) {
    // Before issuing a read, wait for a wite to performed
    WaitForSingleObject(hReadWriteEvent, INFINITE);
    // Add this delay for the "scanning.... Done" occasional hanging
    // no need to delay the whole FW flashing process
    if (USBDelay) {
      USBDelay--;
      Sleep(20);
    }
    // Patch for Compal - extended to general release with parameter (Increases FW flashing time ~40%)
    // from ~135 sec to 190 sec
    if (USBReadDelay) Sleep(10);
    for (int i = 0; i < 5; i++) {

      bReturncode = ReadFile(mXTDevice.HidDevice,
        readBuffer,
        mXTDevice.InputReportByteLength,
        &bytesRead,
        NULL);

      if (bReturncode)
        break;

      if (i == 4) {
        if (!FindHidDevices(TRUE))
          break;

        if (current_frame >= total_frames) {
          _stprintf(PrintBuffer, _T("\nDownloading firmware update... DONE!\n"));
          _TPRINTF(PrintBuffer);
          Sleep(1000);          SetEvent(hCompletionEvent);
          break;                   // Miracle. 2015/06/23.
        }
      }
      Sleep(1000);
    }

    if (bReturncode) {
      HIDDeviceData(readBuffer[0], &readBuffer[1], bytesRead-1);
    }
  }
}

OPObject* CreateAndInitObject(int Type, int Instance, UINT16 Address, UINT8 Size)
{
  OPObject* pObject;
  int errorOnObject = 0;
  TCHAR itemName[32];
  UINT i = 0, k = 0, j = 0;
  UINT partialSum = 0;
  UINT8 objectSize;

  // This is to double check if we are up-to-date on object description
  objectSize = Size;
  if (!Size) {
    objectSize =  GetObjectSize((UINT8)Type);
  }

  switch (Type) {
  case SPT_USERDATA_T38:
    pObject = new OPObject(_T("SPT_USERDATA_T38"), SPT_USERDATA_T38, Instance);
    for (k = 0; k < objectSize; ++k) {
      _stprintf(itemName, _T("DATA[%d]"), k);
      pObject->registers.push_back(new OPRegister(itemName, k));
    }
    break;

  case DEBUG_DIAGNOSTIC_T37:
    k = 0;
    pObject = new OPObject(_T("DEBUG_DIAGNOSTIC_T37"), DEBUG_DIAGNOSTIC_T37, Instance);
    pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("PAGE"), k++));
    for (i = 0; i < (UINT)(objectSize-2); ++i) {
      _stprintf(itemName, _T("DATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    break;

  case GEN_POWERCONFIG_T7:
    k = 0;
    pObject = new OPObject(_T("GEN_POWERCONFIG_T7"), GEN_POWERCONFIG_T7, Instance);
    pObject->registers.push_back(new OPRegister(_T("IDLEACQINT"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVACQINT"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTV2IDLETO"), k++));
    if (objectSize >= 4) {
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }
    if (objectSize >= 5) {
      pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case GEN_ACQUISITIONCONFIG_T8:
    k = 0;
    pObject = new OPObject(_T("GEN_ACQUISITIONCONFIG_T8"), GEN_ACQUISITIONCONFIG_T8, Instance);
    pObject->registers.push_back(new OPRegister(_T("CHRGTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("ATCHDRIFT"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDRIFT"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFTST"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHAUTOCAL"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNC"), k++));
    pObject->registers.push_back(new OPRegister(_T("ATCHCALST"), k++));
    pObject->registers.push_back(new OPRegister(_T("ATCHCALSTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("ATCHFRCCALTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("ATCHFRCCALRATIO"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("MEASALLOW"), k++));
    pObject->registers.push_back(new OPRegister(_T("MEASIDLEDEF"), k++));
    pObject->registers.push_back(new OPRegister(_T("MEASACTVDEF"), k++));
    if (objectSize >= 14) {
      pObject->registers.push_back(new OPRegister(_T("REFMODE"), k++));
    }
    if (objectSize >= 15) {
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }

    if (objectSize != k) errorOnObject = Type;
    break;

  case TOUCH_MULTITOUCHSCREEN_T9:
    k = 0;
    pObject = new OPObject(_T("TOUCH_MULTITOUCHSCREEN_T9"), TOUCH_MULTITOUCHSCREEN_T9, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("AKSCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("BLEN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("ORIENT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGTIMEOUT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTI"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTN"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVFILTER"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMTOUCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("AMPHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XRANGE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YRANGE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("XLOCLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XHICLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YLOCLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YHICLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XEDGECTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGECTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("JUMPLIMIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XPITCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("YPITCH"), k++));
    if (objectSize > 34) {
      pObject->registers.push_back(new OPRegister(_T("NEXTTCHDI"), k++));
    }
    if (objectSize > 35) {
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }
    if (objectSize == 37) {
      pObject->registers.push_back(new OPRegister(_T("MOVFILTER2"), k++));
    }
    if (objectSize > 37) {
      pObject->registers.push_back(new OPRegister(_T("MOVFILTER2"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVSMOOTH"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVPRED"), k++));
    }
    if (objectSize > 39) {
      pObject->registers.push_back(new OPRegister(_T("TRACKTHRSF"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOISETHRSF"), k++));
    }
    if (objectSize == 42) {
      pObject->registers.push_back(new OPRegister(_T("DXSETTINGS"), k++));
    }
    if (objectSize == 43) {
      pObject->registers.push_back(new OPRegister(_T("DXSETTINGS"), k++));
      pObject->registers.push_back(new OPRegister(_T("MRGTHRADJSTR"), k++));
    }
    if (objectSize == 44) {
      pObject->registers.push_back(new OPRegister(_T("DXSETTINGS"), k++));
      pObject->registers.push_back(new OPRegister(_T("MRGTHRADJSTR"), k++));
      pObject->registers.push_back(new OPRegister(_T("CUTOFFTHR"), k++));
    }
    if (objectSize > 43) {
      pObject->registers.push_back(new OPRegister(_T("DXGAIN"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXTCHTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGEGRAD"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGEDIST"), k++));
    }
    if (objectSize > 45) {
      pObject->registers.push_back(new OPRegister(_T("MRGTHRADJSTR"), k++));
    }
    if (objectSize > 46) {
      pObject->registers.push_back(new OPRegister(_T("CUTOFFTHR"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case TOUCH_KEYARRAY_T15:
    k = 0;
    pObject = new OPObject(_T("TOUCH_KEYARRAY_T15"), TOUCH_KEYARRAY_T15, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("AKSCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("BLEN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_COMMSCONFIG_T18:
    k = 0;
    pObject = new OPObject(_T("SPT_COMMSCONFIG_T18"), SPT_COMMSCONFIG_T18, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMAND"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_GPIOPWM_T19:
    k = 0;
    pObject = new OPObject(_T("SPT_GPIOPWM_T19"), SPT_GPIOPWM_T19, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("REPORTMASK"), k++));
    pObject->registers.push_back(new OPRegister(_T("DIR"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTPULLUP"), k++));
    pObject->registers.push_back(new OPRegister(_T("OUT"), k++));
    pObject->registers.push_back(new OPRegister(_T("WAKE"), k++));
    if (objectSize > 6) {
      pObject->registers.push_back(new OPRegister(_T("PWM"), k++));
      pObject->registers.push_back(new OPRegister(_T("PERIOD"), k++));
      pObject->registers.push_back(new OPRegister(_T("DUTY[0]"), k++));
      pObject->registers.push_back(new OPRegister(_T("DUTY[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("DUTY[2]"), k++));
      pObject->registers.push_back(new OPRegister(_T("DUTY[3]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TRIGGER[0]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TRIGGER[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TRIGGER[2]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TRIGGER[3]"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_NOISESUPPRESSION_T22:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPPRESSION_T22"), PROCG_NOISESUPPRESSION_T22, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("VIRTREFRNKG"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCAFUL"), k++, 2, TRUE));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GCAFLL"), k++, 2, TRUE));
    k++;
    pObject->registers.push_back(new OPRegister(_T("ACTVGCAFVALID"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISETHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQHOPSCALE"),k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[3]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[4]"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLEGCAFVALID"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case TOUCH_PROXIMITY_T23:
    k = 0;
    pObject = new OPObject(_T("TOUCH_PROXIMITY_T23"), TOUCH_PROXIMITY_T23, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("BLEN"), k++));
    pObject->registers.push_back(new OPRegister(_T("FXDDTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("FXDDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("AVERAGE"), k++));
    pObject->registers.push_back(new OPRegister(_T("MVNULLRATE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MVDTHR"), k++, 2));
    k++;
    if (objectSize > 15) {
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_ONETOUCHGESTUREPROCESSOR_T24:
    k = 0;
    pObject = new OPObject(_T("PROCI_ONETOUCHGESTUREPROCESSOR_T24"), PROCI_ONETOUCHGESTUREPROCESSOR_T24, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMGEST"), k++));
    pObject->registers.push_back(new OPRegister(_T("GESTEN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("PROCESS"), k++));
    pObject->registers.push_back(new OPRegister(_T("TAPTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("FLICKTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRAGTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("SPRESSTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("LPRESSTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("REPPRESSTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("FLICKTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("DRAGTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TAPTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("THROWTHR"), k++, 2));
    k++;
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SELFTEST_T25:
    k = 0;
    pObject = new OPObject(_T("SPT_SELFTEST_T25"), SPT_SELFTEST_T25, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    for (i = 0; i < TouchSelfCapTotalInstances; ++i) {
      _stprintf(itemName, _T("SIGLIM[%d].UPSIGLIM"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++, 2));
      k++;
      _stprintf(itemName, _T("SIGLIM[%d].LOSIGLIM"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++, 2));
      k++;
    }
    partialSum = 2 + 4*TouchSelfCapTotalInstances;
    pObject->registers.push_back(new OPRegister(_T("PINDWELLUS"), k++));
    ++partialSum;
    if (objectSize > partialSum) {
      for (i = 0; i < TouchSelfCapTotalInstances; ++i) {
        _stprintf(itemName, _T("SIGRANGELIM[%d]"), i);
        pObject->registers.push_back(new OPRegister(itemName, k++, 2));
        k++;
      }
    }
    if (objectSize > k) {
      pObject->registers.push_back(new OPRegister(_T("PINTHR"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_TWOTOUCHGESTUREPROCESSOR_T27:
    k = 0;
    pObject = new OPObject(_T("PROCI_TWOTOUCHGESTUREPROCESSOR_T27"), PROCI_TWOTOUCHGESTUREPROCESSOR_T27, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMGEST"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("GESTEN"), k++));
    pObject->registers.push_back(new OPRegister(_T("ROTATETHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("ZOOMTHR"), k++, 2));
    k++;
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_CTECONFIG_T28:
    k = 0;
    pObject = new OPObject(_T("SPT_CTECONFIG_T28"), SPT_CTECONFIG_T28, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLEGCAFDEPTH"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVGCAFDEPTH"), k++));
    pObject->registers.push_back(new OPRegister(_T("VOLTAGE"), k++, 1, TRUE));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_PROTOTYPE_T35:
    pObject = new OPObject(_T("SPT_PROTOTYPE_T35"), SPT_PROTOTYPE_T35, Instance);
    for (k = 0; k < objectSize; ++k) {
      _stprintf(itemName, _T("PROTOTYPE_DATA[%d]"), k);
      pObject->registers.push_back(new OPRegister(itemName, k));
    }
    pObject->start_address = Address;
    pObject->objectSize = objectSize;
    break;

  case GEN_REMOTEACQUISITION_T39:
    k = 0;
    pObject = new OPObject(_T("GEN_REMOTEACQUISITION_T39"), GEN_REMOTEACQUISITION_T39, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTGAPTIME"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("ACTIVEGCAFDEPTH"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("IDLEGCAFDEPTH"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("RESETTIME"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("ACQSTALLTIME"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GCAFHILIM"), k++, 2, TRUE));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GCAFLOLIM"), k++, 2, TRUE));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GCAFVALID"), k++, 2));
    k++;
    for (i = 0; i < 16; ++i) {
      _stprintf(itemName, _T("THRESHOLDS[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    pObject->registers.push_back(new OPRegister(_T("CHARGETIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSTART"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YMASK"), k++, 2));
    k++;
    for (i = 0; i < 8; ++i) {
      _stprintf(itemName, _T("GAIN[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    for (i = 0; i < 64; ++i) {
      _stprintf(itemName, _T("CHANNELENABLED[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++, 2));
      k++;
    }
    pObject->registers.push_back(new OPRegister(_T("NUMNOISELINES"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMCHIPS"), k++));
    pObject->registers.push_back(new OPRegister(_T("CHIPNUM"), k++));
    pObject->registers.push_back(new OPRegister(_T("VOLTAGE"), k++));
    pObject->registers.push_back(new OPRegister(_T("SELFTEST_MODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("SELFTEST_PORT"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_GRIPSUPPRESSION_T40:
    k = 0;
    pObject = new OPObject(_T("PROCI_GRIPSUPPRESSION_T40"), PROCI_GRIPSUPPRESSION_T40, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XLOGRIP"), k++));
    pObject->registers.push_back(new OPRegister(_T("XHIGRIP"), k++));
    pObject->registers.push_back(new OPRegister(_T("YLOGRIP"), k++));
    pObject->registers.push_back(new OPRegister(_T("YHIGRIP"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_PALMSUPPRESSION_T41:
    k = 0;
    pObject = new OPObject(_T("PROCI_PALMSUPPRESSION_T41"), PROCI_PALMSUPPRESSION_T41, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("LARGEOBJTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("DISTANCETHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPEXTTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("STRENGTH"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_TOUCHSUPPRESSION_T42:
    k = 0;
    pObject = new OPObject(_T("PROCI_TOUCHSUPPRESSION_T42"), PROCI_TOUCHSUPPRESSION_T42, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("APPRTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXAPPRAREA"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXTCHAREA"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPSTRENGTH"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPEXTTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXNUMTCHS"), k++));
    pObject->registers.push_back(new OPRegister(_T("SHAPESTRENGTH"), k++));
    if (objectSize > 8) {
      pObject->registers.push_back(new OPRegister(_T("SUPDIST"), k++));
      pObject->registers.push_back(new OPRegister(_T("DISTHYST"), k++));
    }
    if (objectSize > 10) {
      pObject->registers.push_back(new OPRegister(_T("MAXSCRNAREA"), k++));
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }
    if (objectSize > 12) {
      pObject->registers.push_back(new OPRegister(_T("MAXFNGRSAREA"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_DIGITIZER_T43:
    k = 0;
    pObject = new OPObject(_T("SPT_DIGITIZER_T43"), SPT_DIGITIZER_T43, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("HIDIDLERATE"), k++));
    pObject->registers.push_back(new OPRegister(_T("XLENGTH"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YLENGTH"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("RWKRATE"), k++));
    pObject->registers.push_back(new OPRegister(_T("HEIGHTSCALE"), k++));
    pObject->registers.push_back(new OPRegister(_T("HEIGHTOFFSET"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("WIDTHSCALE"), k++));
    pObject->registers.push_back(new OPRegister(_T("WIDTHOFFSET"), k++, 1, TRUE));
    if (objectSize > 11) {
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }
    if (objectSize > 12) {
      pObject->registers.push_back(new OPRegister(_T("HEIGHTSF"), k++));
      pObject->registers.push_back(new OPRegister(_T("WIDTHSF"), k++));
    }
    if (objectSize > 14) {
      pObject->registers.push_back(new OPRegister(_T("EDGESNPRATIO"), k++));
    }
    if (objectSize > 15) {
      pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
    }
    if (objectSize > 16) {
      pObject->registers.push_back(new OPRegister(_T("BUTTONDI"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_CTECONFIG_T46:
    k = 0;
    pObject = new OPObject(_T("SPT_CTECONFIG_T46"), SPT_CTECONFIG_T46, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLESYNCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVSYNCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("ADCSPERSYNC"), k++));
    pObject->registers.push_back(new OPRegister(_T("PULSESPERADC"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSLEW"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCDELAY"), k++, 2));
    k++;
    if (objectSize > 9) {
      if (objectSize == 10) {
        pObject->registers.push_back(new OPRegister(_T("XVOLTAGE"), k++));
      } else {
        pObject->registers.push_back(new OPRegister(_T("XVOLTAGE"), k++));
        pObject->registers.push_back(new OPRegister(_T("ADCCTRL"), k++));
      }
    }
    if (objectSize > k) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_STYLUS_T47:
    k = 0;
    pObject = new OPObject(_T("PROCI_STYLUS_T47"), PROCI_STYLUS_T47, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CONTMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("CONTMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("STABILITY"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXTCHAREA"), k++));
    pObject->registers.push_back(new OPRegister(_T("AMPLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("STYSHAPE"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOVERSUP"), k++));
    pObject->registers.push_back(new OPRegister(_T("CONFTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCSPERX"), k++));
    if (objectSize > 10) {
      pObject->registers.push_back(new OPRegister(_T("XPOSADJ"), k++, 1, TRUE));
      pObject->registers.push_back(new OPRegister(_T("YPOSADJ"), k++, 1, TRUE));
      pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    }
    if (objectSize > 13) {
      pObject->registers.push_back(new OPRegister(_T("ACTIVELINETHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("ACTIVEPWR"), k++));
      pObject->registers.push_back(new OPRegister(_T("ACTIVERATIO"), k++));
      pObject->registers.push_back(new OPRegister(_T("XPOSPARAM"), k++));
      pObject->registers.push_back(new OPRegister(_T("YPOSPARAM"), k++));
      pObject->registers.push_back(new OPRegister(_T("COMMSFILTERDEPTH"), k++));
      pObject->registers.push_back(new OPRegister(_T("SUPPTIMER"), k++));
    }
    if (objectSize > 20) {
      pObject->registers.push_back(new OPRegister(_T("SUPSTYTO"), k++));
      pObject->registers.push_back(new OPRegister(_T("MAXNUMSTY"), k++));
    }
    if (objectSize > 22) {
      pObject->registers.push_back(new OPRegister(_T("XEDGECTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDIST"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDIST"), k++));
    }
    if (objectSize > 26) {
      pObject->registers.push_back(new OPRegister(_T("SUPTO"), k++));
      pObject->registers.push_back(new OPRegister(_T("SUPCLASSMODE"), k++));
    }
    if (objectSize > 28) {
      pObject->registers.push_back(new OPRegister(_T("DXXEDGECTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGEDIST"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGECTRLHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDISTHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGECTRLHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGEDISTHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECTRLHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDISTHI"), k++));
  }
    if (objectSize > 36) {
      pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVFILTER"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVSMOOTH"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVPRED"), k++));
  }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_NOISESUPPRESSION_T48:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPPRESSION_T48"), PROCG_NOISESUPPRESSION_T48, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("BASEFREQ"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBSOLETE_FREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBSOLETE_FREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBSOLETE_FREQ[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBSOLETE_FREQ[3]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MFFREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MFFREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBSOLETE_GCLIMIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCACTVINVLDADCS"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCIDLEINVLDADCS"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBSOLETE_GCINVALIDTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GCMAXADCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCLIMITMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCLIMITMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCCOUNTMINTGT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MFINVLDDIFFTHR"), k++));
    if (DeviceConnected >= eMXT1386E) {
      pObject->registers.push_back(new OPRegister(_T("MFINCADCSPXTHR"), k++, 2));
      k++;
    }
    pObject->registers.push_back(new OPRegister(_T("MFERRORTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SELFREQMAX"), k++));
    if (DeviceConnected >= eMXT1386E) {
      pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
      pObject->registers.push_back(new OPRegister(_T("INCNLTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("ADCSPERXTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("NLTHRMARGIN"), k++));
      pObject->registers.push_back(new OPRegister(_T("SELFREQSTEP"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    }
    pObject->registers.push_back(new OPRegister(_T("BLEN[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTHR[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDI[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTI[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTN[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVFILTER[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMTOUCH[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGHYST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGTHR[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("XLOCLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XHICLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YLOCLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YHICLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XEDGECTRL[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGEDIST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGECTRL[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGEDIST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("JUMPLIMIT[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHHYST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("NEXTTCHDI[0]"), k++));

    if (objectSize > 54) {
      pObject->registers.push_back(new OPRegister(_T("BLEN[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHTHR[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHDI[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVHYSTI[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVHYSTN[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVFILTER[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("NUMTOUCH[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MRGHYST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MRGTHR[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("XLOCLIP[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("XHICLIP[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("YLOCLIP[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("YHICLIP[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGECTRL[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDIST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECTRL[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDIST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("JUMPLIMIT[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHHYST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("NEXTTCHDI[1]"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case TOUCH_PROXKEY_T52:
    k = 0;
    pObject = new OPObject(_T("TOUCH_PROXKEY_T52"), TOUCH_PROXKEY_T52, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("AKSCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FXDDTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("FXDDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("AVERAGE"), k++));
    pObject->registers.push_back(new OPRegister(_T("MVNULLRATE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MVDTHR"), k++, 2));
    k++;
    if (objectSize != k) errorOnObject = Type;
    break;

  case GEN_DATASOURCE_T53:
    k = 0;
    pObject = new OPObject(_T("GEN_DATASOURCE_T53"), GEN_DATASOURCE_T53, Instance);
    pObject->registers.push_back(new OPRegister(_T("DSTYPE"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_NOISESUPPRESSION_T54:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPPRESSION_T54"), PROCG_NOISESUPPRESSION_T54, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("CALCFG"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("BASEFREQ"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[3]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MFFREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MFFREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("SELFREQMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("ADCSPERXMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLTHR"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_ADAPTIVETHRESHOLD_T55:
    k = 0;
    pObject = new OPObject(_T("PROCI_ADAPTIVETHRESHOLD_T55"), PROCI_ADAPTIVETHRESHOLD_T55, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("TARGETTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("THRADJLIM"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESETSTEPTIME"), k++));
    if (objectSize > 4) {
      pObject->registers.push_back(new OPRegister(_T("FORCECHGDIST"), k++));
      pObject->registers.push_back(new OPRegister(_T("FORCECHGTIME"), k++));
    }
    if (objectSize > 6) {
      pObject->registers.push_back(new OPRegister(_T("LOWESTTHR"), k++, 1, TRUE));
    }
    if (objectSize != k) errorOnObject = Type;
    break;
/*
  case PROCI_SHIELDLESS_T56:
    k = 0;
    pObject = new OPObject(_T("PROCI_SHIELDLESS_T56"), PROCI_SHIELDLESS_T56, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMAND"), k++));
    pObject->registers.push_back(new OPRegister(_T("OPTINT"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
    for (i = 0; i < DeviceMemoryMap.MatrixSizeX; ++i) {
      _stprintf(itemName, _T("INTDELAY[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    if (DeviceMemoryMap.MatrixSizeX == 33) {
      pObject->registers.push_back(new OPRegister(_T("INTDELAY[32]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MULTICUTGC"), k++));
      pObject->registers.push_back(new OPRegister(_T("GCLIMIT"), k++));
      if (objectSize > 39) {
        pObject->registers.push_back(new OPRegister(_T("NCNCL"), k++));
        pObject->registers.push_back(new OPRegister(_T("TOUCHBIAS"), k++));
        pObject->registers.push_back(new OPRegister(_T("BASESCALE"), k++));
        pObject->registers.push_back(new OPRegister(_T("SHIFTLIMIT"), k++));
        pObject->registers.push_back(new OPRegister(_T("YLONOISEMUL"), k++, 2));
        k++;
        pObject->registers.push_back(new OPRegister(_T("YLONOISEDIV"), k++, 2));
        k++;
        pObject->registers.push_back(new OPRegister(_T("YHINOISEMUL"), k++, 2));
        k++;
        pObject->registers.push_back(new OPRegister(_T("YHINOISEDIV"), k++, 2));
        k++;
      }
      if (objectSize > 51) {
        pObject->registers.push_back(new OPRegister(_T("NCNCLMANIDX"), k++));
      }
    } else if (DeviceMemoryMap.MatrixSizeX == 41) {
      for (i = 32; i < 41; ++i) {
        _stprintf(itemName, _T("INTDELAY[%d]"), i);
        pObject->registers.push_back(new OPRegister(itemName, k++));
      }
      pObject->registers.push_back(new OPRegister(_T("MULTICUTGC"), k++));
      pObject->registers.push_back(new OPRegister(_T("GCLIMIT"), k++));
    } else if (DeviceMemoryMap.MatrixSizeX == 44) {
      for (i = 32; i < 44; ++i) {
        _stprintf(itemName, _T("INTDELAY[%d]"), i);
        pObject->registers.push_back(new OPRegister(itemName, k++));
      }
      pObject->registers.push_back(new OPRegister(_T("MULTICUTGC"), k++));
      pObject->registers.push_back(new OPRegister(_T("GCLIMIT"), k++));
      pObject->registers.push_back(new OPRegister(_T("NCNCL"), k++));
      pObject->registers.push_back(new OPRegister(_T("TOUCHBIAS"), k++));
      pObject->registers.push_back(new OPRegister(_T("BASESCALE"), k++));
      pObject->registers.push_back(new OPRegister(_T("SHIFTLIMIT"), k++));
      pObject->registers.push_back(new OPRegister(_T("YLONOISEMUL"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("YLONOISEDIV"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("YHINOISEMUL"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("YHINOISEDIV"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("NCNCLMANIDX"), k++));
    } else {
      pObject->registers.push_back(new OPRegister(_T("MULTICUTGC"), k++));
      pObject->registers.push_back(new OPRegister(_T("GCLIMIT"), k++));
      if (objectSize > 38) {
        pObject->registers.push_back(new OPRegister(_T("NCNCL"), k++));
        pObject->registers.push_back(new OPRegister(_T("TOUCHBIAS"), k++));
        pObject->registers.push_back(new OPRegister(_T("BASESCALE"), k++));
        pObject->registers.push_back(new OPRegister(_T("SHIFTLIMIT"), k++));
        pObject->registers.push_back(new OPRegister(_T("YLONOISEMUL"), k++, 2));
        k++;
        pObject->registers.push_back(new OPRegister(_T("YLONOISEDIV"), k++, 2));
        k++;
        pObject->registers.push_back(new OPRegister(_T("YHINOISEMUL"), k++, 2));
        k++;
        pObject->registers.push_back(new OPRegister(_T("YHINOISEDIV"), k++, 2));
        k++;
      }
      if (objectSize > 50) {
        pObject->registers.push_back(new OPRegister(_T("NCNCLMANIDX"), k++));
      }
    }
    if (objectSize != k) errorOnObject = Type;
    break;
*/
  case PROCI_SHIELDLESS_T56:
    k = 0;
    pObject = new OPObject(_T("PROCI_SHIELDLESS_T56"), PROCI_SHIELDLESS_T56, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMAND"), k++));
    pObject->registers.push_back(new OPRegister(_T("OPTINT"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
    for (i = 0; i < DeviceMemoryMap.MatrixSizeX; ++i) {
      _stprintf(itemName, _T("INTDELAY[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    pObject->registers.push_back(new OPRegister(_T("MULTICUTGC"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCLIMIT"), k++));

    if (objectSize > k) {
      pObject->registers.push_back(new OPRegister(_T("NCNCL"), k++));
      pObject->registers.push_back(new OPRegister(_T("TOUCHBIAS"), k++));
      pObject->registers.push_back(new OPRegister(_T("BASESCALE"), k++));
      pObject->registers.push_back(new OPRegister(_T("SHIFTLIMIT"), k++));
      pObject->registers.push_back(new OPRegister(_T("YLONOISEMUL"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("YLONOISEDIV"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("YHINOISEMUL"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("YHINOISEDIV"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("NCNCLMANIDX"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_EXTRATOUCHSCREENDATA_T57:
    k = 0;
    pObject = new OPObject(_T("PROCI_EXTRATOUCHSCREENDATA_T57"), PROCI_EXTRATOUCHSCREENDATA_T57, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("AREATHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("AREAHYST"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_PRODUCTIONCUSTOM_T60:
    k = 0;
    pObject = new OPObject(_T("SPT_PRODUCTIONCUSTOM_T60"), SPT_PRODUCTIONCUSTOM_T60, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    for (i = 0; i < (UINT)objectSize-1; ++i) {
      _stprintf(itemName, _T("DATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    break;

  case SPT_TIMER_T61:
    k = 0;
    pObject = new OPObject(_T("SPT_TIMER_T61"), SPT_TIMER_T61, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("PERIOD"), k++, 2));
    k++;
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_NOISESUPPRESSION_T62:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPPRESSION_T62"), PROCG_NOISESUPPRESSION_T62, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("FALLNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINTHRADJ"), k++));
    pObject->registers.push_back(new OPRegister(_T("BASEFREQ"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXSELFREQ"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[3]"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQ[4]"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPCNT"), k++));
    pObject->registers.push_back(new OPRegister(_T("ALTMAXSELFREQ"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPCNTPER"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPEVALTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPST"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("INCNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("ADCSPERXTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLTHRMARGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXADCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVADCSVLDNOD"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLEADCSVLDNOD"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINGCLIMIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXGCLIMIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("ALTFREQ[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("ALTFREQ[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("ALTFREQ[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("ALTFREQ[3]"), k++));
    pObject->registers.push_back(new OPRegister(_T("ALTFREQ[4]"), k++));
    pObject->registers.push_back(new OPRegister(_T("BLEN[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTHR[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDI[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTI[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTN[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVFILTER[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMTOUCH[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGHYST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGTHR[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("XLOCLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XHICLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YLOCLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YHICLIP[0]"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XEDGECTRL[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGEDIST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGECTRL[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGEDIST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("JUMPLIMIT[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHHYST[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("NEXTTCHDI[0]"), k++));
    if (objectSize > 54) {
      pObject->registers.push_back(new OPRegister(_T("BLEN[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHTHR[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHDI[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVHYSTI[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVHYSTN[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVFILTER[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("NUMTOUCH[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MRGHYST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("MRGTHR[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("XLOCLIP[1]"), k++, 1, TRUE));
      pObject->registers.push_back(new OPRegister(_T("XHICLIP[1]"), k++, 1, TRUE));
      pObject->registers.push_back(new OPRegister(_T("YLOCLIP[1]"), k++, 1, TRUE));
      pObject->registers.push_back(new OPRegister(_T("YHICLIP[1]"), k++, 1, TRUE));
      pObject->registers.push_back(new OPRegister(_T("XEDGECTRL[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDIST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECTRL[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDIST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("JUMPLIMIT[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHHYST[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("NEXTTCHDI[1]"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_ACTIVESTYLUS_T63:
    k = 0;
    pObject = new OPObject(_T("PROCI_ACTIVESTYLUS_T63"), PROCI_ACTIVESTYLUS_T63, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXTCHAREA"), k++));
    pObject->registers.push_back(new OPRegister(_T("SIGPWR"), k++));
    pObject->registers.push_back(new OPRegister(_T("SIGRATIO"), k++));
    pObject->registers.push_back(new OPRegister(_T("SIGCNTMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("XADJUST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YADJUST"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMSFILTER"), k++));
    pObject->registers.push_back(new OPRegister(_T("DETADCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPDISTHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPTO"), k++));
    if (objectSize > 12) {
      pObject->registers.push_back(new OPRegister(_T("XEDGECTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDIST"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDIST"), k++));
      pObject->registers.push_back(new OPRegister(_T("PRESSUREFILTER"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVFILTER2"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVSMOOTH"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOVPRED"), k++));
    }
    if (objectSize > 20) {
      pObject->registers.push_back(new OPRegister(_T("MOVHYSTI"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("MOVHYSTN"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("SUPCLASSMODE"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_LENSBENDING_T65:
    k = 0;
    pObject = new OPObject(_T("PROCI_LENSBENDING_T65"), PROCI_LENSBENDING_T65, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("GRADTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YLONOISEMUL"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YLONOISEDIV"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YHINOISEMUL"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YHINOISEDIV"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LPFILTCOEF"), k++));
    if (objectSize > 11) {
      pObject->registers.push_back(new OPRegister(_T("FORCESCALE"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("FORCETHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("FORCETHRHYST"), k++));
      pObject->registers.push_back(new OPRegister(_T("FORCEDI"), k++));
      pObject->registers.push_back(new OPRegister(_T("FORCEHYST"), k++));
    }
    if (objectSize > 17) {
      pObject->registers.push_back(new OPRegister(_T("ATCHRATIO"), k++));
      pObject->registers.push_back(new OPRegister(_T("TOTVARLIM"), k++, 2));
      k++;
    }
    if (objectSize > 20) {
      pObject->registers.push_back(new OPRegister(_T("EXFRCTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("EXFRCTHRHYST"), k++));
      pObject->registers.push_back(new OPRegister(_T("EXFRCTO"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_GOLDENREFERENCES_T66:
    k = 0;
    pObject = new OPObject(_T("SPT_GOLDENREFERENCES_T66"), SPT_GOLDENREFERENCES_T66, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("FCALFAILTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("FCALDRIFTCNT"), k++));
    if (objectSize > 3) {
      pObject->registers.push_back(new OPRegister(_T("FCALDRIFTCOEF"), k++));
      pObject->registers.push_back(new OPRegister(_T("FCALDRIFTLIM"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_CUSTOMGESTUREPROCESSOR_T67:
    k = 0;
    pObject = new OPObject(_T("PROCI_CUSTOMGESTUREPROCESSOR_T67"), PROCI_CUSTOMGESTUREPROCESSOR_T67, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("GRABNUMTOUCHES"), k++));
    pObject->registers.push_back(new OPRegister(_T("GRABTO"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("POSHYST"), k++, 2));
    k++;
    if (objectSize > 6) { 
      pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
      pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SERIALDATACOMMAND_T68:
    k = 0;
    pObject = new OPObject(_T("SPT_SERIALDATACOMMAND_T68"), SPT_SERIALDATACOMMAND_T68, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("DATATYPE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LENGTH"), k++));
    for (i = 0; i < 64; ++i) {
      _stprintf(itemName, _T("DATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[3]"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_PALMGESTUREPROCESSOR_T69:
    k = 0;
    pObject = new OPObject(_T("PROCI_PALMGESTUREPROCESSOR_T69"), PROCI_PALMGESTUREPROCESSOR_T69, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("LONGDIMTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("SHORTDIMTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("LONGDIMHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("SHORTDIMHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVTHRTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("AREATHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("AREATHRTO"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_DYNAMICCONFIGURATIONCONTROLLER_T70:
    k = 0;
    pObject = new OPObject(_T("SPT_DYNAMICCONFIGURATIONCONTROLLER_T70"), SPT_DYNAMICCONFIGURATIONCONTROLLER_T70, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("EVENT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("OBJTYPE"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("OBJINST"), k++));
    pObject->registers.push_back(new OPRegister(_T("DSTOFFSET"), k++));
    pObject->registers.push_back(new OPRegister(_T("SRCOFFSET"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LENGTH"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_DYNAMICCONFIGURATIONCONTAINER_T71:
    pObject = new OPObject(_T("SPT_DYNAMICCONFIGURATIONCONTAINER_T71"), SPT_DYNAMICCONFIGURATIONCONTAINER_T71, Instance);
    for (k = 0; k < objectSize; ++k) {
      _stprintf(itemName, _T("DATA[%d]"), k);
      pObject->registers.push_back(new OPRegister(itemName, k));
    }
    break;

  case PROCG_NOISESUPPRESSION_T72:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPPRESSION_T72"), PROCG_NOISESUPPRESSION_T72, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("DEBUGCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPCNT"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPCNTPER"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPEVALTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPST"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAINDUALX"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("INCNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("FALLNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLTHRMARGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINTHRADJ"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLTHRLIMIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("BGSCAN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAINSINGX"), k++));
    pObject->registers.push_back(new OPRegister(_T("BLKNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("STABCTRL"), k++));
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("STABFREQ[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("STABTCHAPX[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("STABNOTCHAPX[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    pObject->registers.push_back(new OPRegister(_T("STABPC"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("STABHIGHNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("STABCNT"), k++));

    pObject->registers.push_back(new OPRegister(_T("NOISCTRL"), k++));
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("NOISFREQ[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("NOISTCHAPX[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("NOISNOTCHAPX[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    pObject->registers.push_back(new OPRegister(_T("NOISPC"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISLOWNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISHIGHNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISCNT"), k++));

    pObject->registers.push_back(new OPRegister(_T("VNOICTRL"), k++));
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("VNOIFREQ[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("VNOITCHAPX[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    for (j = 0; j < 5; ++j) {
      _stprintf(itemName, _T("VNOINOTCHAPX[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    pObject->registers.push_back(new OPRegister(_T("VNOIPC"), k++));
    pObject->registers.push_back(new OPRegister(_T("VNOILOWNLTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("VNOICNT"), k++));
    if (objectSize > 80) {
      pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOTCHMINDIFF"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHMINDIFF"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOTCHMINHOP"), k++));
      pObject->registers.push_back(new OPRegister(_T("TCHMINHOP"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_ZONEINDICATION_T73:
    k = 0;
    pObject = new OPObject(_T("PROCI_ZONEINDICATION_T73"), PROCI_ZONEINDICATION_T73, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("INDPERIOD"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case MOTION_MOTIONSENSING_T75:
    k = 0;
    pObject = new OPObject(_T("MOTION_MOTIONSENSING_T75"), MOTION_MOTIONSENSING_T75, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("SENSORSETID"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACCLI2CADDR"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACCLINTRPT"), k++));
    pObject->registers.push_back(new OPRegister(_T("GYROI2CADDR"), k++));
    pObject->registers.push_back(new OPRegister(_T("GYROINTRPT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAGI2CADDR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAGINTRPT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXRPTIDLE"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACCLRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("ACCLRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LINACRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LINACRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GYRORPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("GYRORPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MAGRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MAGRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TILTRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TILTRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("ROTVCRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("ROTVCRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TEMPRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TEMPRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOTWAKETHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("STABLETIME"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("STABLETHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("LOWMOTEXIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("LOWMOTACT"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALINTINIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALINTNORM"), k++));
    pObject->registers.push_back(new OPRegister(_T("FUSIONINT"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_MOTIONGESTURES_T76:
    k = 0;
    pObject = new OPObject(_T("PROCI_MOTIONGESTURES_T76"), PROCI_MOTIONGESTURES_T76, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("SHAKECTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("SHAKEINTMIN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SHAKEINTMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SHAKETHR"), k++, 2));
    k++;
    if (objectSize > 8) {
      pObject->registers.push_back(new OPRegister(_T("STEPINTERVAL"), k++));
      pObject->registers.push_back(new OPRegister(_T("STEPDELAY"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("STEPTHR"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("TAPJERKTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("TAPSHOCKTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("TAPQUIETTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("TAPDOUBLETIME"), k++, 2));
      k++;
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_CTESCANCONFIG_T77:
    k = 0;
    pObject = new OPObject(_T("SPT_CTESCANCONFIG_T77"), SPT_CTESCANCONFIG_T77, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    if (objectSize == 2 || objectSize == 4) { 
      pObject->registers.push_back(new OPRegister(_T("ACTVPRCSSCNEXT"), k++));
    }
    if (objectSize > 2) {
      pObject->registers.push_back(new OPRegister(_T("XZOOMGAIN"), k++));
      pObject->registers.push_back(new OPRegister(_T("XZOOMTCHTHR"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_GLOVEDETECTION_T78:
    k = 0;
    pObject = new OPObject(_T("PROCI_GLOVEDETECTION_T78"), PROCI_GLOVEDETECTION_T78, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINAREA"), k++));
    pObject->registers.push_back(new OPRegister(_T("CONFTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("GLOVEMODETO"), k++));
    pObject->registers.push_back(new OPRegister(_T("SUPTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCSPERX"), k++));
    if (objectSize > 7) {
      pObject->registers.push_back(new OPRegister(_T("HITHRMRGN"), k++));
    }
    if (objectSize > 8) {
      pObject->registers.push_back(new OPRegister(_T("DISCRIMTHR"), k++));
    }
    if (objectSize > 9) {
      pObject->registers.push_back(new OPRegister(_T("SCTGLOVETHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("SCTGLOVEHYST"), k++));
      pObject->registers.push_back(new OPRegister(_T("SCTMINAREA"), k++));
    }      
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_TOUCHEVENTTRIGGER_T79:
    k = 0;
    pObject = new OPObject(_T("SPT_TOUCHEVENTTRIGGER_T79"), SPT_TOUCHEVENTTRIGGER_T79, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("TOUCHCLASS"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHRELTO"), k++));
    if (objectSize > 3) {
      pObject->registers.push_back(new OPRegister(_T("NUMTCHTHR"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_RETRANSMISSIONCOMPENSATION_T80:
    k = 0;
    pObject = new OPObject(_T("PROCI_RETRANSMISSIONCOMPENSATION_T80"), PROCI_RETRANSMISSIONCOMPENSATION_T80, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMPGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TARGETDELTA"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMPTHR"), k++));
    if (objectSize > 4) {
      pObject->registers.push_back(new OPRegister(_T("ATCHTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOISTCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOISTDTO"), k++));
    }
    if (objectSize > 7) {
      pObject->registers.push_back(new OPRegister(_T("MOISTTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("MOISTINVATCHTHR"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_UNLOCKGESTURE_T81:
    k = 0;
    pObject = new OPObject(_T("PROCI_UNLOCKGESTURE_T81"), PROCI_UNLOCKGESTURE_T81, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINDIST"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("STARTXMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("STARTYMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("STARTXSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("STARTYSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("ENDXMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("ENDYMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("ENDXSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("ENDYSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVLIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVLIMMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXAREA"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXNUMTCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("ANGLE"), k++));
    if (objectSize > 17) {
      pObject->registers.push_back(new OPRegister(_T("TCHTIMTHR"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_NOISESUPEXTENSION_T82:
    k = 0;
    pObject = new OPObject(_T("SPT_NOISESUPEXTENSION_T82"), SPT_NOISESUPEXTENSION_T82, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("NSLVLHI"), k++));
    pObject->registers.push_back(new OPRegister(_T("NSLVLLO"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case ENVIRO_LIGHTSENSING_T83:
    k = 0;
    pObject = new OPObject(_T("ENVIRO_LIGHTSENSING_T83"), ENVIRO_LIGHTSENSING_T83, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTRPT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MAXRPTIDLE"), k++));
    pObject->registers.push_back(new OPRegister(_T("AMBRPTINT"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("AMBRPTHYST"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LORPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("HIRPTTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("HILOPERSISTCNT"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_GESTUREPROCESSOR_T84:
    k = 0;
    pObject = new OPObject(_T("PROCI_GESTUREPROCESSOR_T84"), PROCI_GESTUREPROCESSOR_T84, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("ZONETHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("DIREL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTO"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

 case PEN_ACTIVESTYLUSPOWER_T85:
    k = 0;
    pObject = new OPObject(_T("PEN_ACTIVESTYLUSPOWER_T85"), PEN_ACTIVESTYLUSPOWER_T85, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CLKCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("PWRCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("PWRCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("DEEPSTART"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("CYCLECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESYNCSCAN"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESYNCLINE"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESYNCREMAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESYNCMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESYNCMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCSCAN"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCBETWEEN"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCSEARCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("PERSLEEPCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("PERSLEEPCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("PERSLEEPCFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("DEEPSLEEPCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("DEEPSLEEPCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("DEEPSLEEPCFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("SDEEPSTART"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SDEEPSNOOZE"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_NOISESUPACTIVESTYLUS_T86:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPACTIVESTYLUS_T86"), PROCG_NOISESUPACTIVESTYLUS_T86, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    for (i = 0; i < 6; ++i) {
      _stprintf(itemName, _T("FREQ[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    for (i = 0; i < 6; ++i) {
      _stprintf(itemName, _T("RESERVED[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    pObject->registers.push_back(new OPRegister(_T("HOPST"), k++));
    pObject->registers.push_back(new OPRegister(_T("IIRCOEFF"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[6]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[7]"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTDHOP"), k++));
    pObject->registers.push_back(new OPRegister(_T("GCLIMNL"), k++));
    if (objectSize > 20) {
      pObject->registers.push_back(new OPRegister(_T("NLGAIN"), k++));
    }
    if (objectSize > 21) {
      pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED"), k++));
      pObject->registers.push_back(new OPRegister(_T("STABHIGHNLTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOISCTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOISLOWNLTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOISHIGHNLTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("NOISCNT"), k++));
      pObject->registers.push_back(new OPRegister(_T("VNOICTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("VNOILOWNLTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED"), k++));
      pObject->registers.push_back(new OPRegister(_T("VNOICNT"), k++));
/*
      for (j = 0; j < 3; ++j) {
        _stprintf(itemName, _T("CTRL[%d]"), j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
        _stprintf(itemName, _T("LOWNLTHR[%d]"), j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
        _stprintf(itemName, _T("HIGHNLTHR[%d]"), j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
        _stprintf(itemName, _T("CNT[%d]"), j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
      }
*/
      pObject->registers.push_back(new OPRegister(_T("MINNLTDDIFF"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PEN_ACTIVESTYLUSDATA_T87:
    k = 0;
    pObject = new OPObject(_T("PEN_ACTIVESTYLUSDATA_T87"), PEN_ACTIVESTYLUSDATA_T87, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MSGMODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("MSGSIZE"), k++));
    for (i = 0; i < 8; ++i) {
      _stprintf(itemName, _T("DBGMSGDATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    pObject->registers.push_back(new OPRegister(_T("ADCCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("BATCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("PCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("PCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("PDRIFTTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("PDRIFTTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("PFIXED"), k++));
    j = (objectSize - (k+2))/2;
    for (i = 0; i < j; ++i) {
      _stprintf(itemName, _T("PSCALE[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++, 2));
      k++;
    }
    pObject->registers.push_back(new OPRegister(_T("PFACTCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("PFACTCMD"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PEN_ACTIVESTYLUSRECEIVE_T88:
    k = 0;
    pObject = new OPObject(_T("PEN_ACTIVESTYLUSRECEIVE_T88"), PEN_ACTIVESTYLUSRECEIVE_T88, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("RXCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("RXCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("HEADER"), k++));
    pObject->registers.push_back(new OPRegister(_T("CLAMPDELAY"), k++));
    pObject->registers.push_back(new OPRegister(_T("DISABLEDELAY"), k++));
    pObject->registers.push_back(new OPRegister(_T("AMPGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("EDGE"), k++));
    pObject->registers.push_back(new OPRegister(_T("DBGCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("DBGCFG2"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PEN_ACTIVESTYLUSTRANSMIT_T89:
    k = 0;
    pObject = new OPObject(_T("PEN_ACTIVESTYLUSTRANSMIT_T89"), PEN_ACTIVESTYLUSTRANSMIT_T89, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("TXCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("TXCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("TXCFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("BOOSTCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("BOOSTSETTLE"), k++));
    pObject->registers.push_back(new OPRegister(_T("BOOSTVOLTS"), k++));
    pObject->registers.push_back(new OPRegister(_T("BOOSTREFRESH"), k++));
    pObject->registers.push_back(new OPRegister(_T("BOOSTHI"), k++));
    pObject->registers.push_back(new OPRegister(_T("BOOSTLO"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PEN_ACTIVESTYLUSWINDOW_T90:
    k = 0;
    pObject = new OPObject(_T("PEN_ACTIVESTYLUSWINDOW_T90"), PEN_ACTIVESTYLUSWINDOW_T90, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPCYCLES"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPLINEMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPLINEMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("UNCERTAINTY"), k++));
    pObject->registers.push_back(new OPRegister(_T("MARK0"), k++));
    pObject->registers.push_back(new OPRegister(_T("SPACE0"), k++));
    pObject->registers.push_back(new OPRegister(_T("MIN0"), k++));
    pObject->registers.push_back(new OPRegister(_T("HALF0"), k++));
    pObject->registers.push_back(new OPRegister(_T("MARK1"), k++));
    pObject->registers.push_back(new OPRegister(_T("SPACE1"), k++));
    pObject->registers.push_back(new OPRegister(_T("MIN1"), k++));
    pObject->registers.push_back(new OPRegister(_T("HALF1"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case DEBUG_CUSTOMDATACONFIG_T91:
    k = 0;
    pObject = new OPObject(_T("DEBUG_CUSTOMDATACONFIG_T91"), DEBUG_CUSTOMDATACONFIG_T91, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DELTADIVIDE"), k++));
    pObject->registers.push_back(new OPRegister(_T("DELTATRIGTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTERBYTEEXTDLY"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_SYMBOLGESTUREPROCESSOR_T92:
    k = 0;
    pObject = new OPObject(_T("PROCI_SYMBOLGESTUREPROCESSOR_T92"), PROCI_SYMBOLGESTUREPROCESSOR_T92, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOVY"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SYMTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("EXTSTRCNTMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("LSTRCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("LSTRCFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    j = k;
    for (i = 0; i < objectSize - j; ++i) {
      _stprintf(itemName, _T("SYMDATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_TOUCHSEQUENCELOGGER_T93:
    k = 0;
    pObject = new OPObject(_T("PROCI_TOUCHSEQUENCELOGGER_T93"), PROCI_TOUCHSEQUENCELOGGER_T93, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XMIN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("XMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YMIN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TCHMOVMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("NOTCHMOVMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("TCHTIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTIMMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOTCHTIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOTCHTIMMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHCNTTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTPRETIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTPOSTTIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTTCHMOVMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("DTNOTCHMOVMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("DTTCHTIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTTCHTIMMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTNOTCHTIMMIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("DTNOTCHTIMMAX"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_PTCCONFIG_T95:
    k = 0;
    pObject = new OPObject(_T("SPT_PTCCONFIG_T95"), SPT_PTCCONFIG_T95, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDRIFT"), k++));
    pObject->registers.push_back(new OPRegister(_T("ADCSEXP"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_PTCTUNINGPARAMS_T96:
    k = 0;
    pObject = new OPObject(_T("SPT_PTCTUNINGPARAMS_T96"), SPT_PTCTUNINGPARAMS_T96, Instance);
    for (j = 0; j < (UINT)objectSize/2; ++j) {
      _stprintf(itemName, _T("PARAMS[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k++, 2));
      k++;
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case TOUCH_PTCKEYS_T97:
    k = 0;
    pObject = new OPObject(_T("TOUCH_PTCKEYS_T97"), TOUCH_PTCKEYS_T97, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("AKSCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("GAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("KEYMASK"), k++, 4));
    k += 3;
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_PTCNOISESUPPRESSION_T98:
    k = 0;
    pObject = new OPObject(_T("PROCG_PTCNOISESUPPRESSION_T98"), PROCG_PTCNOISESUPPRESSION_T98, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    for (j = 0; j < 3; ++j) {
      _stprintf(itemName, _T("FREQ[%d]"), j);
      pObject->registers.push_back(new OPRegister(itemName, k));
      k++;
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case TOUCH_MULTITOUCHSCREEN_T100:
    k = 0;
    pObject = new OPObject(_T("TOUCH_MULTITOUCHSCREEN_T100"), TOUCH_MULTITOUCHSCREEN_T100, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("SCRAUX"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHAUX"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHEVENTCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("AKSCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("NUMTCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("XYCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("XORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("XPITCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("XLOCLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XHICLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XRANGE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("XEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("DXXEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("DXXEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YORIGIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YPITCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("YLOCLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YHICLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YRANGE"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("GAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("DXGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISESF"), k++));
    pObject->registers.push_back(new OPRegister(_T("CUTOFFTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGTHRADJSTR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MRGHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("DXTHRSF"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDIDOWN"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDIUP"), k++));
    pObject->registers.push_back(new OPRegister(_T("NEXTTCHDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("JUMPLIMIT"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVFILTER"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVSMOOTH"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVPRED"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTI"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("AMPLHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("SCRAREAHYST"), k++));
    if (objectSize > 53) {
      pObject->registers.push_back(new OPRegister(_T("INTTHRHYST"), k++));
    }
    if (objectSize > 54) {
      pObject->registers.push_back(new OPRegister(_T("XEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDISTHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("DXXEDGEDISTHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDISTHI"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_TOUCHSCREENHOVER_T101:
    k = 0;
    pObject = new OPObject(_T("SPT_TOUCHSCREENHOVER_T101"), SPT_TOUCHSCREENHOVER_T101, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XLOCLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XHICLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("XEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XHVRTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("XHVRHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YLOCLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YHICLIP"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YHVRTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YHVRHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("HVRDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("CONFTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVFILTER"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVSMOOTH"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVPRED"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTI"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("HVRAUX"), k++));
    if (objectSize > 25) {
      pObject->registers.push_back(new OPRegister(_T("HVRJUMPLIMIT"), k++));
    }
    if (objectSize > 26) {
      pObject->registers.push_back(new OPRegister(_T("XEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDISTHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDISTHI"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SELFCAPHOVERCTECONFIG_T102:
    k = 0;
    pObject = new OPObject(_T("SPT_SELFCAPHOVERCTECONFIG_T102"), SPT_SELFCAPHOVERCTECONFIG_T102, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    if (objectSize < 26) {
      pObject->registers.push_back(new OPRegister(_T("TUNTHR"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("TUNAVGCYCLES"), k++));
      pObject->registers.push_back(new OPRegister(_T("TUNCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
      pObject->registers.push_back(new OPRegister(_T("PRECHRGTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("CHRGTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
      pObject->registers.push_back(new OPRegister(_T("IDLESYNCSPERL"), k++));
      pObject->registers.push_back(new OPRegister(_T("ACTVSYNCSPERL"), k++));
      pObject->registers.push_back(new OPRegister(_T("DRIFTINT"), k++));
      pObject->registers.push_back(new OPRegister(_T("DRIFTST"), k++));
      pObject->registers.push_back(new OPRegister(_T("DRIFTSTHRSF"), k++));
      pObject->registers.push_back(new OPRegister(_T("FILTER"), k++));
      pObject->registers.push_back(new OPRegister(_T("FILTCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRU"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("BACKGROUND"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRCLMP"), k++));
    } else {
      pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
      pObject->registers.push_back(new OPRegister(_T("TUNTHR"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("TUNHYST"), k++, 2));
      k++;
      pObject->registers.push_back(new OPRegister(_T("TUNAVGCYCLES"), k++));
      pObject->registers.push_back(new OPRegister(_T("TUNCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("TUNSYNCSPERL"), k++));
      pObject->registers.push_back(new OPRegister(_T("TUNGAIN"), k++));
      pObject->registers.push_back(new OPRegister(_T("BGTUNCTRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("PRECHRGTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("CHRGTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
      pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
      pObject->registers.push_back(new OPRegister(_T("IDLESYNCSPERL"), k++));
      pObject->registers.push_back(new OPRegister(_T("ACTVSYNCSPERL"), k++));
      pObject->registers.push_back(new OPRegister(_T("DRIFT"), k++));
      pObject->registers.push_back(new OPRegister(_T("DRIFTST"), k++));
      pObject->registers.push_back(new OPRegister(_T("DRIFTSTHRSF"), k++));
      pObject->registers.push_back(new OPRegister(_T("FILTER"), k++));
      pObject->registers.push_back(new OPRegister(_T("FILTCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRU"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRCLMP"), k++));
      pObject->registers.push_back(new OPRegister(_T("RECALCFG"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_SCHNOISESUPPRESSION_T103:
    k = 0;
    pObject = new OPObject(_T("PROCI_SCHNOISESUPPRESSION_T103"), PROCI_SCHNOISESUPPRESSION_T103, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("MSGRATE"), k++));
    pObject->registers.push_back(new OPRegister(_T("PROCST"), k++));
    pObject->registers.push_back(new OPRegister(_T("SCHSIGLOTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SCHSIGHITHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("NLXGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLYGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLLOTHR"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("NLHITHR"), k++, 2));
    k++;
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_AUXTOUCHCONFIG_T104:
    k = 0;
    pObject = new OPObject(_T("SPT_AUXTOUCHCONFIG_T104"), SPT_AUXTOUCHCONFIG_T104, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XTCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("XTCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XINTTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("XINTHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YTCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YTCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YINTTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YINTHYST"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

   case SPT_DRIVENPLATEHOVERCONFIG_T105:
    k = 0;
    pObject = new OPObject(_T("SPT_DRIVENPLATEHOVERCONFIG_T105"), SPT_DRIVENPLATEHOVERCONFIG_T105, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("MODE"), k++));
    pObject->registers.push_back(new OPRegister(_T("PRECHRGTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("CHRGTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLESYNCSPERL"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVSYNCSPERL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFT"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFTST"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFTSTHRSF"), k++));
    pObject->registers.push_back(new OPRegister(_T("FILTER"), k++));
    pObject->registers.push_back(new OPRegister(_T("FILTCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRU"), k++));
    pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DYNIIRCLMP"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;
   
   case SPT_ACTIVESTYLUSMMBCONFIG_T106:
    k = 0;
    pObject = new OPObject(_T("SPT_ACTIVESTYLUSMMBCONFIG_T106"), SPT_ACTIVESTYLUSMMBCONFIG_T106, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XCLUSTERSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("YCLUSTERSIZE"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTDELAY"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("INTERLINEDELAY"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLESYNCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVSYNCSPERX"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMSTHRHVR"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMSTHRTCH"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_ACTIVESTYLUS_T107:
    k = 0;
    pObject = new OPObject(_T("PROCI_ACTIVESTYLUS_T107"), PROCI_ACTIVESTYLUS_T107, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("XGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("XINRNGTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("XINRNGHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XHVRTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("XHVRHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XTCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("XTCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("XEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YINRNGTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YINRNGHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YHVRTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YHVRHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YTCHTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("YTCHHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("YEDGEDIST"), k++));
    pObject->registers.push_back(new OPRegister(_T("INRNGDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("HVRDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("TCHDI"), k++));
    pObject->registers.push_back(new OPRegister(_T("FREQHOPDIEXT"), k++));
    pObject->registers.push_back(new OPRegister(_T("XPOSADJ"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("YPOSADJ"), k++, 1, TRUE));
    pObject->registers.push_back(new OPRegister(_T("FULLPOSADJLIM"), k++));
    pObject->registers.push_back(new OPRegister(_T("ZEROPOSADJLIM"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVFILTER"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVSMOOTH"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVPRED"), k++));
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTI"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOVHYSTN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("PRSTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("PRSHYST"), k++));
    pObject->registers.push_back(new OPRegister(_T("STYAUX"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMSCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("VLDDATATHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("PRSVARLIM"), k++));
    pObject->registers.push_back(new OPRegister(_T("COMMSRNGTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("BUTTONDILIM"), k++));
    if (objectSize > 43) {
      pObject->registers.push_back(new OPRegister(_T("CFILTCTRL"), k++));
      for (j = 0; j < 6; ++j) {
        _stprintf(itemName, _T("CFC1STPASS[%d]"), j);
        pObject->registers.push_back(new OPRegister(itemName, k++, 2));
        k++;
      }
      for (j = 0; j < 6; ++j) {
        _stprintf(itemName, _T("CFC2NDPASS[%d]"), j);
        pObject->registers.push_back(new OPRegister(itemName, k++, 2));
        k++;
      }
      pObject->registers.push_back(new OPRegister(_T("CFTHRSTATE0"), k++, 2, TRUE));
      k++;
      pObject->registers.push_back(new OPRegister(_T("CFTHRSTATE1"), k++, 2, TRUE));
      k++;
      pObject->registers.push_back(new OPRegister(_T("COMMSLENCMASK"), k++, 4));
      k += 3;
      pObject->registers.push_back(new OPRegister(_T("XEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("XEDGEDISTHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGECFGHI"), k++));
      pObject->registers.push_back(new OPRegister(_T("YEDGEDISTHI"), k++));
    }
     if (objectSize != k) errorOnObject = Type;
    break;

  case PROCG_NOISESUPSELFCAP_T108:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPSELFCAP_T108"), PROCG_NOISESUPSELFCAP_T108, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGCLIM"), k++));
    pObject->registers.push_back(new OPRegister(_T("IIRCOEFF"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTDDIFF"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTDHOP"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[0]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[1]"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPST"), k++));

    pObject->registers.push_back(new OPRegister(_T("STABCTRL"), k++));
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("STABFREQ[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("STABTCHAPX[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("STABNOTCHAPX[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    pObject->registers.push_back(new OPRegister(_T("RESERVED[2]"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[3]"), k++));
    pObject->registers.push_back(new OPRegister(_T("STABHINLTDTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[4]"), k++));

    pObject->registers.push_back(new OPRegister(_T("NOISCTRL"), k++));
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("NOISFREQ[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("NOISTCHAPX[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("NOISNOTCHAPX[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    pObject->registers.push_back(new OPRegister(_T("RESERVED[5]"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISLONLTDTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISHINLTDTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("NOISCNT"), k++));

    pObject->registers.push_back(new OPRegister(_T("VNOICTRL"), k++));
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("VNOIFREQ[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("VNOITCHAPX[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    for (i = 0; i < 5; ++i) {
      _stprintf(itemName, _T("VNOINOTCHAPX[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    pObject->registers.push_back(new OPRegister(_T("RESERVED[6]"), k++));
    pObject->registers.push_back(new OPRegister(_T("VNOILONLTDTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[7]"), k++));
    pObject->registers.push_back(new OPRegister(_T("VNOICNT"), k++));

    pObject->registers.push_back(new OPRegister(_T("BLKNLTDTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("RESERVED[8]"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

/*
  case PROCG_NOISESUPSELFCAP_T108:
    k = 0;
    pObject = new OPObject(_T("PROCG_NOISESUPSELFCAP_T108"), PROCG_NOISESUPSELFCAP_T108, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG2"), k++));
    pObject->registers.push_back(new OPRegister(_T("CFG3"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("NLGCLIM"), k++));
    pObject->registers.push_back(new OPRegister(_T("IIRCOEFF"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTDDIFF"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINNLTDHOP"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINCSDIFF"), k++));
    pObject->registers.push_back(new OPRegister(_T("MINCSHOP"), k++));
    pObject->registers.push_back(new OPRegister(_T("HOPST"), k++));
    for (i = 0; i < 3; ++i) {
      _stprintf(itemName, _T("CTRL[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
      for (j = 0; j < 5; ++j) {
        _stprintf(itemName, _T("FREQ[%d][%d]"), i, j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
      }
      for (j = 0; j < 5; ++j) {
        _stprintf(itemName, _T("TCHAPX[%d][%d]"), i, j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
      }
      for (j = 0; j < 5; ++j) {
        _stprintf(itemName, _T("NOTCHAPX[%d][%d]"), i, j);
        pObject->registers.push_back(new OPRegister(itemName, k++));
      }
      _stprintf(itemName, _T("RESERVED[%d][1]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));

      _stprintf(itemName, _T("LONLTDLVL[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
      _stprintf(itemName, _T("HINLTDLVL[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
      _stprintf(itemName, _T("CNT[%d]"), i);
      pObject->registers.push_back((new OPRegister(itemName, k++)));  
    }
    pObject->registers.push_back(new OPRegister(_T("BLKNLTDTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("BLKCSTHR"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;
*/

  case SPT_SELFCAPGLOBALCONFIG_T109:
    k = 0;
    pObject = new OPObject(_T("SPT_SELFCAPGLOBALCONFIG_T109"), SPT_SELFCAPGLOBALCONFIG_T109, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DBGCTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMDONRESET"), k++));
    pObject->registers.push_back(new OPRegister(_T("CMD"), k++));
    pObject->registers.push_back(new OPRegister(_T("LFCOMPSFX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("LFCOMPSFY"), k++, 2));
    k++;
    if (objectSize > k) {
      pObject->registers.push_back(new OPRegister(_T("TUNECFG"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SELFCAPTUNINGPARAMS_T110:
    k = 0;
    pObject = new OPObject(_T("SPT_SELFCAPTUNINGPARAMS_T110"), SPT_SELFCAPTUNINGPARAMS_T110, Instance);
    for (i = 0; i < DeviceMemoryMap.MatrixSizeY; ++i) {
      _stprintf(itemName, _T("PARAMS[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++, 2));
      k++;
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SELFCAPCONFIG_T111:
    k = 0;
    pObject = new OPObject(_T("SPT_SELFCAPCONFIG_T111"), SPT_SELFCAPCONFIG_T111, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DBGCTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("DELAYTIME"), k++));
    pObject->registers.push_back(new OPRegister(_T("IDLESYNCSPERL"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVSYNCSPERL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFT"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFTST"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRIFTSTHRSF"), k++));
    pObject->registers.push_back(new OPRegister(_T("CALRECSTR"), k++));
    if (objectSize == 11) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
    }
    if (objectSize == 12) {
      pObject->registers.push_back(new OPRegister(_T("ALTINTTIMEX"), k++));
      pObject->registers.push_back(new OPRegister(_T("ALTDELAYTIMEX"), k++));
    }
    if (objectSize == 13) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("ALTINTTIMEX"), k++));
      pObject->registers.push_back(new OPRegister(_T("ALTDELAYTIMEX"), k++));
    }
    if (objectSize > 14) {
      pObject->registers.push_back(new OPRegister(_T("FILTER"), k++));
      pObject->registers.push_back(new OPRegister(_T("FILTERCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRU"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRTHRL"), k++));
      pObject->registers.push_back(new OPRegister(_T("DYNIIRCLMP"), k++));
    }
    if (objectSize == 16) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
    }
    if (objectSize == 17) {
      pObject->registers.push_back(new OPRegister(_T("ALTINTTIMEX"), k++));
      pObject->registers.push_back(new OPRegister(_T("ALTDELAYTIMEX"), k++));
    }
    if (objectSize == 18) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("ALTINTTIMEX"), k++));
      pObject->registers.push_back(new OPRegister(_T("ALTDELAYTIMEX"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_SELFCAPGRIPSUPPRESSION_T112:
    k = 0;
    pObject = new OPObject(_T("PROCI_SELFCAPGRIPSUPPRESSION_T112"), PROCI_SELFCAPGRIPSUPPRESSION_T112, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("CHGSHPTHRX"), k++));
    pObject->registers.push_back(new OPRegister(_T("CHGSHPTHRY"), k++));
    pObject->registers.push_back(new OPRegister(_T("APPRSHPTHRX"), k++));
    pObject->registers.push_back(new OPRegister(_T("APPRSHPTHRY"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SELFCAPMEASURECONFIG_T113:
    k = 0;
    pObject = new OPObject(_T("SPT_SELFCAPMEASURECONFIG_T113"), SPT_SELFCAPMEASURECONFIG_T113, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XGAIN"), k++));
    pObject->registers.push_back(new OPRegister(_T("YGAIN"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_ACTIVESTYLUSMEASCONFIG_T114:
    k = 0;
    pObject = new OPObject(_T("SPT_ACTIVESTYLUSMEASCONFIG_T114"), SPT_ACTIVESTYLUSMEASCONFIG_T114, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCSPERLINE"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYNCPULSECFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATXSCAN0"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATXSCAN1"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATXSCAN2"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATXSCAN3"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATYSCAN0"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATYSCAN1"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATYSCAN2"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVPATYSCAN3"), k++));
    pObject->registers.push_back(new OPRegister(_T("DRVSCANCFG"), k++));
    pObject->registers.push_back(new OPRegister(_T("SAMPLEPERIOD"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTDELAY"), k++));
    pObject->registers.push_back(new OPRegister(_T("INTTIME"), k++));
    if (objectSize == 16) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
    }
    if (objectSize == 17) {
      pObject->registers.push_back(new OPRegister(_T("ATCHCALSTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("DBGCTRL"), k++));
    }
    if (objectSize == 18) {
      pObject->registers.push_back(new OPRegister(_T("INRUSHCFG"), k++));
      pObject->registers.push_back(new OPRegister(_T("ATCHCALSTHR"), k++));
      pObject->registers.push_back(new OPRegister(_T("DBGCTRL"), k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_SYMBOLGESTURE_T115:
    k = 0;
    pObject = new OPObject(_T("PROCI_SYMBOLGESTURE_T115"), PROCI_SYMBOLGESTURE_T115, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("XMIN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("XMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YMIN"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("YMAX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOVX"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("MOVY"), k++, 2));
    k++;
    pObject->registers.push_back(new OPRegister(_T("SYMTO"), k++));
    pObject->registers.push_back(new OPRegister(_T("SYMTIMEMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("EXTSTRCNTMAX"), k++));
    pObject->registers.push_back(new OPRegister(_T("LSTRCFG1"), k++));
    pObject->registers.push_back(new OPRegister(_T("LSTRCNTTHR"), k++));
    pObject->registers.push_back(new OPRegister(_T("LSTRNUMTCH"), k++));
    pObject->registers.push_back(new OPRegister(_T("LSTRANGLE"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_SYMBOLGESTURECONFIG_T116:
    k = 0;
    pObject = new OPObject(_T("SPT_SYMBOLGESTURECONFIG_T116"), SPT_SYMBOLGESTURECONFIG_T116, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    for (i = 0; i < (UINT)objectSize - 1; ++i) {
      _stprintf(itemName, _T("SYMDATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_DATACONTAINER_T117:
    k = 0;
    pObject = new OPObject(_T("SPT_DATACONTAINER_T117"), SPT_DATACONTAINER_T117, Instance);
    for (i = 0; i < objectSize; ++i) {
      _stprintf(itemName, _T("DATA[%d]"), i);
      pObject->registers.push_back(new OPRegister(itemName, k++));
    }
    if (objectSize != k) errorOnObject = Type;
    break;

  case SPT_DATACONTAINERCTRL_T118:
    k = 0;
    pObject = new OPObject(_T("SPT_DATACONTAINERCTRL_T118"), SPT_DATACONTAINERCTRL_T118, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DATATYPE"), k++));
    pObject->registers.push_back(new OPRegister(_T("HEADERTYPE"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

  case PROCI_KEYSTATEPROCESSOR_T119:
    k = 0;
    pObject = new OPObject(_T("PROCI_KEYSTATEPROCESSOR_T119"), PROCI_KEYSTATEPROCESSOR_T119, Instance);
    pObject->registers.push_back(new OPRegister(_T("CTRL"), k++));
    pObject->registers.push_back(new OPRegister(_T("DECTKEYMASK"), k++, 4));
    k += 3;
    pObject->registers.push_back(new OPRegister(_T("GUARDKEYMASK"), k++, 4));
    k += 3;
    if (objectSize != k) errorOnObject = Type;
    break;

  case GEN_SLAVEPOWERCONFIG_T120:
    k = 0;
    pObject = new OPObject(_T("GEN_SLAVEPOWERCONFIG_T120"), GEN_SLAVEPOWERCONFIG_T120, Instance);
    pObject->registers.push_back(new OPRegister(_T("IDLEACQINT"), k++));
    pObject->registers.push_back(new OPRegister(_T("ACTVACQINT"), k++));
    if (objectSize != k) errorOnObject = Type;
    break;

    // If we get here, we got some new object type
    // this means that we do not know how to deal with it
    // in other words, this version of the utility cannot handle it properly
    // we can try to deal with it as a bunch of bytes
  default:
    _stprintf(itemName, _T("UNKNOWN_T%d"), Type);

    pObject = new OPObject(itemName, Type, Instance);
    for (k = 0; k < objectSize; ++k) {
      _stprintf(itemName, _T("UNKNOWN[%d]"), k);
      pObject->registers.push_back(new OPRegister(itemName, k));
    }
    _stprintf(PrintBuffer, _T("Found a new object T%d. Its structure is unknown to this version of MxtUpdater.\nObject saved/configured as byte fields.\n"), Type);
    _TPRINTF(PrintBuffer);
    break;
  }

  if (errorOnObject) {
    if (!objectSize) {
      _stprintf(PrintBuffer, _T("Mismatch between configuration and firmware: object T%d not found\n"), errorOnObject);
      _TPRINTF(PrintBuffer);
    } else {
      if (k < objectSize) {
        for (i = k; i < objectSize; ++i) {
          _stprintf(itemName, _T("UNKNOWN[%d]"), i);
          pObject->registers.push_back(new OPRegister(itemName, i));
        }
      } else {
        UINT z, cnt = 0;
        for (i = 0; i < pObject->registers.size(); i++)
        {
          cnt += pObject->registers[i]->size;
          if (cnt > objectSize) {
            for (z = (UINT)pObject->registers.size(); z > i; z--) {
              pObject->registers.pop_back();
            }
            break;
          }
        }
      }
    }       
  }
  // T35 is treated differently
  if (Type == SPT_PROTOTYPE_T35) {
    return pObject;
  }
  // Save object address
  for (i = 0; i < NumberOfObjects; ++i) {
    if (AllObjects[i]->Type == Type) {
      UINT16 address = AllObjects[i]->LSB | (AllObjects[i]->MSB << 8);
      if (Instance && AllObjects[i]->Instances) {
        address += (UINT16)((AllObjects[i]->Size + 1)*Instance);
      }
      pObject->start_address = address;
      pObject->objectSize = AllObjects[i]->Size + 1;
      return pObject;
    }
  }

  delete pObject;
  return NULL;
}

/*============================================================================
Name : CRC_soft24
------------------------------------------------------------------------------
Purpose: Calculate 24 bit checksum of input data
Input  : UINT32 crc   : current checksum
UINT8 byte1  : new lower byte to calculate crc
UINT8 byte2  : new upper byte to calculate crc
Output : Current checksum
Notes  : Algorithm is as described in Object protocol document, section
"Info Block Checksum"
============================================================================*/
UINT32 CRC_soft24(UINT32 crc, UINT8 byte1, UINT8 byte2)
{
  static const UINT32 crcpoly = 0x0080001B;
  UINT32 result;
  UINT16 data_word;
  data_word = (UINT16)( (UINT16)( byte2 << 8u ) | byte1 );
  result = ((crc << 1u) ^ (UINT32)data_word);
  if(result & 0x1000000) // If bit 25 is set
  {
    result ^= crcpoly; // XOR result with crcpoly
  }
  return(result);// & (0xFFFFFF));
}

// WARNING: if the number of bytes (Size) is odd
// it uses an extra byte of PData by zeroing it.
// if this is not acceptable, please allocate
// a suitable buffer on calling it...
UINT32 CalculateCRC(UINT8 *PData, UINT Size, UINT32 Crc)
{
  UINT totalSize;
  totalSize = Size;
  if (Size  & 0x0001) {
    ++totalSize;
    PData[Size] = 0;
  }

  // calculate CRC
  UINT32 crc = Crc;
  for(UINT i = 0; i < totalSize - 1; i += 2) {
    crc = CRC_soft24(crc, PData[i], PData[i + 1]);
  }
  crc &= 0x00FFFFFFu;

  return crc;
}

/*
// We need to read the real NVM checksum...
BOOL ReadNVMChecksum(UINT32 *Checksum)
{
  UINT8 msg[128], CmdProcReportId;
  BOOL bMsgRcvd = FALSE;
  UINT8 LastReportID=0xFF;    // Add by Miracle.
  int i;
  int timeoutCounter;

  CmdProcReportId = GetReportIdFromType(GEN_COMMANDPROCESSOR_T6);

  for(i=0; i<3; i++) {
    if (!SendCmdToDeviceWithAck(eReportAll, RESET)) {
      DBGPRINT(_T("Cannot send report all command\n"));
      return FALSE;
    }

    for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
      if (ReadMessage(msg)) {
        if (ConnectFWType == I2CBRIDGE && Protocol_HID) {
          // Added by Miracle.
          if(msg[5] == LastReportID)
            break;    // if report ID reproduce. It means all message be read.
          LastReportID = msg[5];
          DBGPRINT(_T("LastReportID(Bridge): %d\n"), LastReportID);
        }
        else {   // HIDI2C inbox driver
          LastReportID = msg[0];
          if (msg[0] == CmdProcReportId) {
            *Checksum = *((UINT32 *)&msg[2]);
            bMsgRcvd = TRUE;
          }
          else {
          }
         DBGPRINT(_T("LastReportID(Inbox): %d\n"), LastReportID);
        }
        //else {
        //}
      }
      else {
        DBGPRINT(_T("ReadMessage error\n"));
        if (bMsgRcvd) break;
      }
  
    }
    DBGPRINT(_T("Timeout counter: %d\n"), timeoutCounter);
    if ( timeoutCounter == MAX_COUNT ) {
      //return FALSE;
      Sleep(1000);
      continue;
    }
    return TRUE;
  }
  if(i==3)
  {
    DBGPRINT(_T("Max retry but still fail\n"));
    return FALSE;
  }
  return TRUE;
}
*/

// We need to read the real NVM checksum...
BOOL ReadNVMChecksum(UINT32 *Checksum)
{
  UINT8 msg[128], CmdProcReportId;
  BOOL bMsgRcvd = FALSE;
  UINT8 LastReportID=0xFF;    // Add by Miracle.
  int i;
  int timeoutCounter;

  CmdProcReportId = GetReportIdFromType(GEN_COMMANDPROCESSOR_T6);

  for(i = 0; i < 10; i++) {
    if (!SendCmdToDeviceWithAck(eReportAll, RESET)) {
      //_stprintf(PrintBuffer, _T("Cannot send report all command\n"));
//      _stprintf(PrintBuffer, _T("*"));    // Miracle. 2014/06/16. For debug purpose.
//      _TPRINTF(PrintBuffer);
      Sleep(10);
      continue; // Resend report all command.
      // return FALSE;
    }
    MessageMode();
    for (timeoutCounter = 0; timeoutCounter < 30; ++timeoutCounter) {
      if (ReadMessage(msg)) {
        if (ConnectFWType == I2CBRIDGE && Protocol_HID) {
          if(msg[0] == CmdProcReportId) {
            *Checksum = 256*256*msg[4] + 256*msg[3] + msg[2];
            return TRUE;
          }
          // Added by Miracle.
          if(msg[0] == LastReportID)
            break;    // if report ID reproduce. It means all message be read.
          LastReportID = msg[0];
          //_stprintf(PrintBuffer, _T("LastReportID(Bridge): %d\n"), LastReportID);
          //_TPRINTF(PrintBuffer);
        }
        else {   // HIDI2C inbox driver
          LastReportID = msg[0];
          if (msg[0] == CmdProcReportId) {
            //*Checksum = *((UINT32 *)&msg[2]);
            *Checksum = 256*256*msg[4] + 256*msg[3] + msg[2];   // 3 bytes checksum.
            bMsgRcvd = TRUE;
            return TRUE;
          }
          else {
          }
          //_stprintf(PrintBuffer, _T("LastReportID(Inbox): %d\n"), LastReportID);
          //_TPRINTF(PrintBuffer);
        }
        //else {
        //}
      }
      else {
        //_stprintf(PrintBuffer, _T("ReadMessage error\n"));
        //_TPRINTF(PrintBuffer);
        //if (bMsgRcvd) break;
        //break;
      }
  
    }
    //_stprintf(PrintBuffer, _T("Timeout counter: %d\n"), timeoutCounter);
    //_TPRINTF(PrintBuffer);
    if ( timeoutCounter == 30 ) {
      //return FALSE;
      Sleep(100 /*1000*/);  // Miracle. 4/25. Shorter delay.
      continue;
    }
    //return TRUE;
  }
  if(i == 10) {
    _stprintf(PrintBuffer, _T("Could not read checksum after 10 retries.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  return TRUE;
}

//
// This function calculates checksum of current config
//
BOOL CalculateInFWChecksum(UINT32 *Checksum)
{
  if (ObjectsInFW.size() == 0) {
    ReadConfigFromDevice();
  }
  return (CalculateObjectsCRC(Checksum, &ObjectsInFW));
}

/*
  UINT16 firstAddr, lastAddr;
  UINT numBytes; 
  DWORD bytesRead;
  UINT8 *pData;

  if(!ReadMemoryMap()) return FALSE;

  // get first writable object address...
  if (!GetObjectAddressAndSize(GEN_POWERCONFIG_T7, &firstAddr)) {
    // Cannot get Power Config address...
    return FALSE;
  }
  lastAddr = AllObjects.back()->LSB;
  lastAddr |= (AllObjects.back()->MSB<<8) & 0xFF00;
  lastAddr += (AllObjects.back()->Size + 1) * (AllObjects.back()->Instances + 1);

  // calculate number of bytes to read
  numBytes = lastAddr - firstAddr; 
  bytesRead = numBytes;

  pData = (UINT8 *)new UINT8[numBytes+1];

  if (!ReadFromDevice(firstAddr, pData, &bytesRead)) return FALSE;

  // add a dummy trailing zero if required to force even number of bytes
  if(numBytes & 0x0001) {
    pData[numBytes] = 0;
  }

  // calculate CRC
  UINT32 crc = 0;
  for(UINT i = 0; i < numBytes; i += 2) {
    crc = CRC_soft24(crc, pData[i], pData[i + 1]);
  }

  // mask result to 24 bits
  crc &= 0x00FFFFFFu;
  *Checksum = crc;
  delete[] pData;

  return TRUE;
}
*/

BOOL CalculateObjectsCRC(UINT32 * Checksum, vector<OPObject*> *PObiects)
{
  UINT totalSize = 0;
  UINT j, obj = 0;  
  UINT8 *pData;

  // Now we have the configuration values in the Objects vector...
  vector<OPObject*>::const_iterator current = PObiects->begin();

  // Calculate the total size of the objects on which we will calculate the checksum
  while (current != PObiects->end()) {
    if (((*current)->type != DEBUG_DIAGNOSTIC_T37) && ((*current)->type != SPT_USERDATA_T38) &&
      ((*current)->type != SPT_SERIALDATACOMMAND_T68)) {
      totalSize += (*current)->objectSize;
    }
    ++current;
  }
  pData = (UINT8 *)new UINT8[totalSize+2];
  ZeroMemory(pData, totalSize+2);
  current = PObiects->begin();
  vector<OPRegister*>::const_iterator regcurrent;
  j = totalSize;
  while (current != PObiects->end()) {
    if (((*current)->type != DEBUG_DIAGNOSTIC_T37) && ((*current)->type != SPT_USERDATA_T38) &&
      ((*current)->type != SPT_SERIALDATACOMMAND_T68)) {
      // copy all the objects into a piece of memory
      regcurrent = (*current)->registers.begin();
      while (regcurrent != (*current)->registers.end()) {
        pData[obj + (*regcurrent)->offset] = (*regcurrent)->value & 0x00FF;
        if ((*regcurrent)->size == 2) {
          pData[obj + (*regcurrent)->offset + 1] = ((*regcurrent)->value >> 8) & 0x00FF;
        }
        if ((*regcurrent)->size == 4) {
          pData[obj + (*regcurrent)->offset + 1] = ((*regcurrent)->value >> 8) & 0x00FF;
          pData[obj + (*regcurrent)->offset + 2] = ((*regcurrent)->value >> 16) & 0x00FF;
          pData[obj + (*regcurrent)->offset + 3] = ((*regcurrent)->value >> 24) & 0x00FF;
        }
        ++regcurrent;
      }
      obj += (*current)->objectSize;
    }
    ++current;
  }
  if(j & 0x0001) {
    pData[++j] = 0;
    totalSize = j;
  }
  // calculate CRC
  UINT32 crc = 0;
  for(UINT i = 0; i < totalSize; i += 2) {
    crc = CRC_soft24(crc, pData[i], pData[i + 1]);
  }

  // mask result to 24 bits
  crc &= 0x00FFFFFFu;
  *Checksum = crc;
  delete[] pData;

  return TRUE;
}

BOOL GetTempFile(TCHAR *FilePath)
{
  DWORD dwRetVal = 0;

  //  Gets the temp path env string (no guarantee it's a valid path).
  dwRetVal = GetTempPath(MAX_PATH, FilePath);
  if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
    _stprintf(PrintBuffer, _T("Cannot create temp file. Please save configuration to a file first, then upgrade firmware.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  _tcscat(FilePath, _T("maXTouchConfig.tmp"));

  FILE *hCfgTmp = _tfopen(FilePath, _T("w"));
  if (hCfgTmp == NULL) {
    _stprintf(PrintBuffer, _T("Could not create temp file to save configuration.\nPlease save configuration to a file first, then upgrade firmware.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  fclose(hCfgTmp);
  return TRUE;
}

//
// This function find all connected HID devices
// After calling this function and using the returned info, you need
// to call FreeMemHidDevices to release all the allocated memory
//
BOOL FindHidDevices(int Order)
{
  HDEVINFO                            hardwareDeviceInfo;
  SP_DEVICE_INTERFACE_DATA            deviceInfoData;
  int                                 i;
  BOOLEAN                             done;
  GUID                                hidGuid;
  PSP_DEVICE_INTERFACE_DETAIL_DATA    functionClassDeviceData = NULL;
  ULONG                               predictedLength = 0;
  ULONG                               requiredLength = 0;
  PHID_DEVICE                         newHidDevice;
  int                                 iDevicePathSize;
  MAXTOUCHDEVICES                     *pDevice;
  TCHAR                               buffer[256];
  BOOL                                bReturncode = TRUE;
  UINT                                deviceIndex = 1;
  HidD_GetHidGuid (&hidGuid);

  //
  // Open a handle to the plug and play dev node.
  //
  hardwareDeviceInfo = SetupDiGetClassDevs ( &hidGuid,
    NULL, // Define no enumerator (global)
    NULL, // Define no
    (DIGCF_PRESENT | // Only Devices present
    DIGCF_DEVICEINTERFACE)); // Function class devices.

  if (INVALID_HANDLE_VALUE == hardwareDeviceInfo) {
    return FALSE;
  }

  done = FALSE;
  deviceInfoData.cbSize = sizeof (SP_DEVICE_INTERFACE_DATA);
  i = -1;
  while (!done) {
    i++;
    newHidDevice = new (HID_DEVICE);
    if (SetupDiEnumDeviceInterfaces (hardwareDeviceInfo,
      0, // No care about specific PDOs
      &hidGuid,
      i,
      &deviceInfoData)) {
        //
        // allocate a function class device data structure to receive the
        // goods about this particular device.
        //

        SetupDiGetDeviceInterfaceDetail (hardwareDeviceInfo, &deviceInfoData,
          NULL, // probing so no output buffer yet
          0, // probing so output buffer length of zero
          &requiredLength,
          NULL); // not interested in the specific dev-node


        predictedLength = requiredLength;

        functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc (predictedLength);
        if (functionClassDeviceData) {
          functionClassDeviceData->cbSize = sizeof (SP_DEVICE_INTERFACE_DETAIL_DATA);
          ZeroMemory(functionClassDeviceData->DevicePath, sizeof(functionClassDeviceData->DevicePath));
        } else {
          SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
          delete newHidDevice;
          return FALSE;
        }

        //
        // Retrieve the information from Plug and Play.
        //

        if (! SetupDiGetDeviceInterfaceDetail (hardwareDeviceInfo, &deviceInfoData,
          functionClassDeviceData, predictedLength,
          &requiredLength, NULL)) {
            SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
            free(functionClassDeviceData);
            delete newHidDevice;
            return FALSE;
        }

        if ((LPTSTR)functionClassDeviceData->DevicePath == NULL) {
          free(functionClassDeviceData);
          delete newHidDevice;
          return FALSE;
        }

        iDevicePathSize = (INT)_tcsclen((LPTSTR)functionClassDeviceData->DevicePath) + 1;
        newHidDevice->DevicePath = (PTCHAR)malloc(iDevicePathSize*sizeof(TCHAR));

        if (newHidDevice->DevicePath == NULL) {
          free(functionClassDeviceData);
          delete newHidDevice;
          return FALSE;
        }    

        StringCbCopy(newHidDevice->DevicePath, iDevicePathSize*sizeof(TCHAR), (LPTSTR)functionClassDeviceData->DevicePath);

        newHidDevice->HidDevice = CreateFile (newHidDevice->DevicePath, 0,
          FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,    // no SECURITY_ATTRIBUTES structure
          OPEN_EXISTING,   // No special create flags
          0,               // Open device as non-overlapped so we can get data
          NULL);           // No template file

        if (INVALID_HANDLE_VALUE == newHidDevice->HidDevice) {
          free(newHidDevice->DevicePath);
          newHidDevice->DevicePath = NULL ;
          free(functionClassDeviceData);
          delete newHidDevice;
          continue;
        }

        if (!HidD_GetPreparsedData (newHidDevice->HidDevice, &newHidDevice->Ppd)) {
          free(newHidDevice->DevicePath);
          newHidDevice->DevicePath = NULL ;
          CloseHandle(newHidDevice->HidDevice);
          newHidDevice->HidDevice = INVALID_HANDLE_VALUE ;
          free(functionClassDeviceData);
          delete newHidDevice;
          continue;
        }

        if (!HidD_GetAttributes (newHidDevice->HidDevice, &newHidDevice->Attributes)) {
          free(newHidDevice->DevicePath);
          newHidDevice->DevicePath = NULL;
          CloseHandle(newHidDevice->HidDevice);
          newHidDevice->HidDevice = INVALID_HANDLE_VALUE;
          HidD_FreePreparsedData (newHidDevice->Ppd);
          newHidDevice->Ppd = NULL;
          free(functionClassDeviceData);
          delete newHidDevice;
          continue;
        }

        if (!HidP_GetCaps (newHidDevice->Ppd, &newHidDevice->Caps)) {
          free(newHidDevice->DevicePath);
          newHidDevice->DevicePath = NULL;
          CloseHandle(newHidDevice->HidDevice);
          newHidDevice->HidDevice = INVALID_HANDLE_VALUE;
          HidD_FreePreparsedData (newHidDevice->Ppd);
          newHidDevice->Ppd = NULL;
          free(functionClassDeviceData);
          delete newHidDevice;
          continue;
        }
        if (newHidDevice->Attributes.VendorID != VENDORID) {
          free(newHidDevice->DevicePath);
          newHidDevice->DevicePath = NULL;
          CloseHandle(newHidDevice->HidDevice);
          newHidDevice->HidDevice = INVALID_HANDLE_VALUE;
          HidD_FreePreparsedData (newHidDevice->Ppd);
          newHidDevice->Ppd = NULL;
          free(functionClassDeviceData);
          delete newHidDevice;
          continue;
        }
        free(functionClassDeviceData);
        // Save the control interface ID to support multiple MaxTouch devices
        bReturncode = HidD_GetProductString(newHidDevice->HidDevice, buffer, 256);
        // Check exclusions first...
        if (!_tcsnicmp(buffer, ATMEL_EXCL_STRING[0], 4)) {
          delete newHidDevice;  // delete last allocated and not used...
          newHidDevice = NULL;
        } else if (!_tcscmp(buffer, ATMEL_CTRL_STRING[0])) {
          if (UsbI2c == 2) {
            // get only I2C devices
            delete newHidDevice;  // delete last allocated and not used...
            newHidDevice = NULL;
          } else {
            pDevice = new(MAXTOUCHDEVICES);
            AllPids[deviceIndex -1] = newHidDevice->Attributes.ProductID;
            pDevice->DeviceIndex = deviceIndex++;
            pDevice->DeviceData = newHidDevice;
            pDevice->DeviceType = USB_NORMAL;
            pDevice->ReportId = ATMEL_REPORT_ID_USB;
            pDevice->ConnectFWType = HIDUSB;
            MxtCtrlDevice[Order].push_back(pDevice);
          }
        }
        // Added by Miracle.
        else if (!_tcscmp(buffer, ATMEL_CTRL_STRING[1]) || !_tcscmp(buffer, ATMEL_CTRL_STRING[2])) {   // support 5055 bridge board.
          pDevice = new(MAXTOUCHDEVICES);
          // This is not correct because this PID is the PID of the bridge!
          //          AllPids[deviceIndex -1] = newHidDevice->Attributes.ProductID;
          pDevice->DeviceIndex = deviceIndex++;
          pDevice->DeviceData = newHidDevice;
          pDevice->DeviceType = USB_NORMAL;
          pDevice->ReportId = ATMEL_REPORT_ID_I2CBridge;
          pDevice->ConnectFWType = I2CBRIDGE;
          MxtCtrlDevice[Order].push_back(pDevice);
        }
        // Check if in Bootloader mode
        else if (!_tcscmp(buffer, ATMEL_BOOTLOADER_STRING[0]) || !_tcscmp(buffer, ATMEL_BOOTLOADER_STRING[1])) {
          if (UsbI2c == 2) {
            // get only I2C devices
            delete newHidDevice;  // delete last allocated and not used...
            newHidDevice = NULL;
          } else {
            ++DeviceInBootloader[Order];
            pDevice = new(MAXTOUCHDEVICES);
            AllPids[deviceIndex -1] = newHidDevice->Attributes.ProductID;
            pDevice->DeviceIndex = deviceIndex++;
            pDevice->DeviceData = newHidDevice;
            pDevice->DeviceType = USB_BOOTLOADER;
            pDevice->ReportId = ATMEL_REPORT_ID_USB;
            pDevice->ConnectFWType = HIDUSB;
            MxtCtrlDevice[Order].push_back(pDevice);
          }
        } else if (newHidDevice->Caps.UsagePage == NORMAL_USAGE_PAGE) {
          if (UsbI2c == 1) {
            // get only USB devices
            delete newHidDevice;  // delete last allocated and not used...
            newHidDevice = NULL;
          } else {
            pDevice = new(MAXTOUCHDEVICES);
            AllPids[deviceIndex -1] = newHidDevice->Attributes.ProductID;
            pDevice->DeviceIndex = deviceIndex++;
            pDevice->DeviceData = newHidDevice;
            pDevice->DeviceType = I2C_NORMAL;
            pDevice->ReportId = ATMEL_REPORT_ID_I2C;
            if (newHidDevice->Attributes.ProductID == 0x214c) {
              pDevice->ReportId = ATMEL_REPORT_ID2_I2C;
            }
            pDevice->ConnectFWType = HIDI2C;
            MxtCtrlDevice[Order].push_back(pDevice);
          }
        } else {
          delete newHidDevice;  // delete last allocated and not used...
          newHidDevice = NULL;
        }
    } else {
      delete newHidDevice;  // delete last allocated and not used...
      newHidDevice = NULL;
      if (ERROR_NO_MORE_ITEMS == GetLastError()) {
        done = TRUE;
      }
    }
    if (newHidDevice) {
      // Save valid device
      HidDevice[Order].push_back(newHidDevice);
    }
  }
  SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);

  SavePIDBridgeDevices(Order);

  return TRUE;
}

VOID FreeMemHidDevices(int Order)
{
  vector<HID_DEVICE *>::iterator devCurrent = HidDevice[Order].begin();
  while (devCurrent != HidDevice[Order].end()) {
    if ((*devCurrent)->DevicePath) {
      free((*devCurrent)->DevicePath);
      (*devCurrent)->DevicePath = NULL;
    }
    if ((*devCurrent)->HidDevice) {
      CloseHandle((*devCurrent)->HidDevice);
      (*devCurrent)->HidDevice = NULL;
    }
    delete (*devCurrent);
    ++devCurrent;
  }
  HidDevice[Order].clear();
}

BOOL SavePIDBridgeDevices(int Order)
{
  vector<MAXTOUCHDEVICES *>::iterator devCurrent = MxtCtrlDevice[Order].begin();
  while (devCurrent != MxtCtrlDevice[Order].end()) {
    if ((*devCurrent)->ConnectFWType == I2CBRIDGE) {
      // We need to select the correct device requested by the user
      // fix multi-devices via bridge
      if ((*devCurrent)->DeviceIndex != DeviceIndex) {
        ++devCurrent;
        continue;
      }
        // Close handle if previously open...
      if (mXTDevice.HidDevice && (mXTDevice.HidDevice != INVALID_HANDLE_VALUE)) {
        CloseHandle(mXTDevice.HidDevice);
        mXTDevice.HidDevice = 0;
      }
      mXTDevice.InputReportByteLength = ((*devCurrent)->DeviceData)->Caps.InputReportByteLength;
      mXTDevice.OutputReportByteLength = ((*devCurrent)->DeviceData)->Caps.OutputReportByteLength;
      mXTDevice.HidDevice = CreateFile (((*devCurrent)->DeviceData)->DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,          // no SECURITY_ATTRIBUTES structure
        OPEN_EXISTING, // No special create flags
        0,             // Open device as non-overlapped so we can get data
        NULL);         // No template file

      if (mXTDevice.HidDevice == INVALID_HANDLE_VALUE) {
        return FALSE;
      }
      DeviceAlreadyIdentified = FALSE;
      AtmelReportId = ATMEL_REPORT_ID_I2CBridge;
      ConnectFWType = I2CBRIDGE;
      if (ReadMemoryMap_BridgeBoard()) {
        ((*devCurrent)->DeviceData)->Attributes.ProductID = (int)(256*HIDHEADER[HIDDSC_Product_MSB] + HIDHEADER[HIDDSC_Product_LSB]);
        AllPids[(*devCurrent)->DeviceIndex-1] = ((*devCurrent)->DeviceData)->Attributes.ProductID;
      }
    }
    ++devCurrent;
  }
  return TRUE;
}

//
// Not ready yet: work in progress...
//
BOOL StoreGoldenRefs()
{
  //  BOOL bReturncode = TRUE;
  UINT8 buffer[512];
  UINT16 address;
  UINT8 t66_ctrl;
  UINT8 size;
  UINT8 index;
  UINT8 reportId;
  UINT8 status;
  UINT8 state;
  UINT8 timeoutCounter;
  DWORD bytesRead, bytesToWrite;
  std::vector<UINT8> t66Data;

  if (!ReadMemoryMap()) return FALSE;
  // Check if Object T66 exists
  // get T66 address and size...
  if (!GetObjectIndex(SPT_GOLDENREFERENCES_T66, &index)) {
    // Cannot get T66 address... Wrong FW?
    _stprintf(PrintBuffer, _T("Store Golden References not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  address = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  size = AllObjects[index]->Size + 1;
  
  // get T66 report ID
  reportId = GetReportIdFromType(SPT_GOLDENREFERENCES_T66);
  if (reportId == 0) {
    _stprintf(PrintBuffer, _T("Store Golden References not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  bytesRead = size;
  if (!(ReadFromDevice(address, buffer, &bytesRead) && (bytesRead == (DWORD)size))) {
    DBGPRINT(_T("\nStoreGoldenRefs: Error on ReadFromDevice - object T66, code = %d\n"), GetLastError());
    return FALSE;
  }
  // Read object T66 and save it
  for (UINT i = 0; i < bytesRead; ++i) {
    t66Data.push_back(buffer[i]);
  }
  MsgProcAddress = 0;
  GetObjectAddressAndSize(5, &MsgProcAddress, &MsgProcSize);

  t66_ctrl = t66Data[0];
  t66_ctrl |= T66_CONFIG_CTRL_ENABLE;
  t66_ctrl |= T66_CONFIG_CTRL_RPTEN;

  // in ctrl field, set prime command
  t66_ctrl &= ~T66_CONFIG_CTRL_FCALCMD;
  t66_ctrl |= T66_CONFIG_CTRL_FCALCMD_PRIME;
  bytesToWrite = 1;

  if (!WriteToDevice(address, &t66_ctrl, &bytesToWrite)) {
    DBGPRINT(_T("\nStoreGoldenRefs: Error on WriteToDevice, code = %d\n"), GetLastError());
    return FALSE;
  }

  // Check for primed message
  bytesRead = 1;
  ZeroMemory(buffer, 512);
  Sleep(100);
  bytesRead = MsgProcSize;

  for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
    if (!ReadFromDevice(MsgProcAddress, buffer, &bytesRead)) {
      DBGPRINT(_T("\nStoreGoldenRefs: Error on ReadFromDevice, code = %d\n"), GetLastError());
      return FALSE;
    } else {
      bytesRead = MsgProcSize;
      if (buffer[0] == reportId) {
        status = buffer[1];
        state = (UINT8)( status & T66_MSG_STATUS_FCALSTATE );
        if( T66_MSG_STATUS_FCALSTATE_PRIMED == state ) {
          break;
        } else {
          DBGPRINT(_T("\nStoreGoldenRefs: failed to enter primed state.\n"));
          return FALSE;
        }
      }
    }
    Sleep(300);
  }

  if ( timeoutCounter == MAX_COUNT ) {
    DBGPRINT(_T("\nStoreGoldenRefs: time-out. Status msg did not arrive (PRIMED)\n"));
    return FALSE;
  }

  t66_ctrl = t66Data[0];
  t66_ctrl |= T66_CONFIG_CTRL_ENABLE;
  t66_ctrl |= T66_CONFIG_CTRL_RPTEN;

  // in ctrl field, set generate command
  t66_ctrl &= ~T66_CONFIG_CTRL_FCALCMD;
  t66_ctrl |= T66_CONFIG_CTRL_FCALCMD_GENERATE;

  if (!WriteToDevice(address, &t66_ctrl, &bytesToWrite)) {
    DBGPRINT(_T("\nStoreGoldenRefs: Error on WriteToDevice, code = %d\n"), GetLastError());
    return FALSE;
  }

  // Check for generated message
  bytesRead = 1;
  ZeroMemory(buffer, 512);
  Sleep(100);
  bytesRead = MsgProcSize;
  for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
    if (!ReadFromDevice(MsgProcAddress, buffer, &bytesRead)) {
      DBGPRINT(_T("\nStoreGoldenRefs: Error on ReadFromDevice, code = %d\n"), GetLastError());
      return FALSE;
    } else {
      bytesRead = MsgProcSize;
      if (buffer[0] == reportId) {
        status = buffer[1];
        state = (UINT8)( status & T66_MSG_STATUS_FCALSTATE );
        if ((T66_MSG_STATUS_FCALSTATE_GENERATED == state) && (T66_MSG_STATUS_FCALPASS & status )) {
          break;
        } else {
          DBGPRINT(_T("\nStoreGoldenRefs: GENERATED and FCALPASS not set. Status = 0x%x\n"), status);
          return FALSE;
        }
      }
    }
    Sleep(300);
  }

  if ( timeoutCounter == MAX_COUNT ) {
    DBGPRINT(_T("\nStoreGoldenRefs: time-out. Status msg did not arrive (GENERATED/FCALPASS)\n"));
    return FALSE;
  }

  // Do a store command
  t66_ctrl = t66Data[0];
  t66_ctrl |= T66_CONFIG_CTRL_ENABLE;
  t66_ctrl |= T66_CONFIG_CTRL_RPTEN;

  // in ctrl field, set store command
  t66_ctrl &= ~T66_CONFIG_CTRL_FCALCMD;
  t66_ctrl |= T66_CONFIG_CTRL_FCALCMD_STORE;

  if (!WriteToDevice(address, &t66_ctrl, &bytesToWrite)) {
    DBGPRINT(_T("\nStoreGoldenRefs: Error on WriteToDevice, code = %d\n"), GetLastError());
    return FALSE;
  }

  // Check for store completion message
  bytesRead = 1;
  ZeroMemory(buffer, 512);
  Sleep(100);
  bytesRead = MsgProcSize;
  for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
    if (!ReadFromDevice(MsgProcAddress, buffer, &bytesRead)) {
      DBGPRINT(_T("\nStoreGoldenRefs: Error on ReadFromDevice, code = %d\n"), GetLastError());
      return FALSE;
    } else {
      bytesRead = MsgProcSize;
      if (buffer[0] == reportId) {
        status = buffer[1];
        state = (UINT8)( status & T66_MSG_STATUS_FCALSTATE );
        if ((T66_MSG_STATUS_FCALSTATE_IDLE == state) && (T66_MSG_STATUS_FCALSEQDONE & status)) {
          break;
        } else {
          DBGPRINT(_T("\nStoreGoldenRefs: failed to store data.\n"));
          return FALSE;
        }
      }
    }
    Sleep(300);
  }

  if ( timeoutCounter == MAX_COUNT ) {
    DBGPRINT(_T("\nStoreGoldenRefs: time-out. Status msg did not arrive (FCALSTATE_IDLE/FCALSEQDONE)\n"));
    return FALSE;
  }

  return TRUE;
}

UINT8 GetReportIdFromType(UINT8 Type)
{
  UINT8 j;
  for (j = 0; j < 255; ++j) { 
    if (ReportIds[j] == Type) {
      return(j);
    }
  }
  return 0; //error not found
}

enum GRSTATUS GoldenRefsStatus()
{
  //  BOOL bReturncode = TRUE;
  UINT8 buffer[512];
  UINT16 address;
  UINT8 t66_ctrl;
  UINT8 size;
  UINT8 index;
  UINT8 reportId;
  UINT8 status;
  UINT8 state;
  UINT8 timeoutCounter;
  DWORD bytesRead, bytesToWrite;
  std::vector<UINT8> t66Data;

  if (!ReadMemoryMap()) return eCouldNotGet;
  // Check if Object T66 exists
  // get T66 address and size...
  if (!GetObjectIndex(SPT_GOLDENREFERENCES_T66, &index)) {
    // Cannot get T66 address... Wrong FW?
    DBGPRINT(_T("Golden References not supported by this version of firmware\n"));
    return eNotSupported;
  }
  address = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  size = AllObjects[index]->Size + 1;
  
  // get T66 report ID
  reportId = GetReportIdFromType(SPT_GOLDENREFERENCES_T66);
  if (reportId == 0) {
    DBGPRINT(_T("\nStore Golden References not supported by this version of firmware\n"));
    return eNotSupported;
  }

  bytesRead = size;
  if (!(ReadFromDevice(address, buffer, &bytesRead) && (bytesRead == (DWORD)size))) {
    DBGPRINT(_T("\nGoldenRefsStatus: Error on ReadFromDevice - object T66, code = %d\n"), GetLastError());
    return eCouldNotGet;
  }
  // Read object T66 and save it
  for (UINT i = 0; i < bytesRead; ++i) {
    t66Data.push_back(buffer[i]);
  }
  MsgProcAddress = 0;
  GetObjectAddressAndSize(5, &MsgProcAddress, &MsgProcSize);
  MsgProcSize;

  t66_ctrl = t66Data[0];
  t66_ctrl |= T66_CONFIG_CTRL_ENABLE;
  t66_ctrl |= T66_CONFIG_CTRL_RPTEN;
  bytesToWrite = 1;

  if (!WriteToDevice(address, &t66_ctrl, &bytesToWrite)) {
    DBGPRINT(_T("\nGoldenRefsStatus:: Error on WriteToDevice, code = %d\n"), GetLastError());
    return eCouldNotGet;
  }

  // do a calibrate to generate a message
  if (!CalibrateDevice()) {
    DBGPRINT(_T("\nGoldenRefsStatus:: Error on CalibrateDevice, code = %d\n"), GetLastError());
    return eCouldNotGet;
  }

  // Check for generated message
  bytesRead = 1;
  ZeroMemory(buffer, 512);
  Sleep(100);
  bytesRead = MsgProcSize;
  for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
    if (!ReadFromDevice(MsgProcAddress, buffer, &bytesRead)) {
      DBGPRINT(_T("\nGoldenRefsStatus: Error on ReadFromDevice, code = %d\n"), GetLastError());
      return eCouldNotGet;
    } else {
      bytesRead = MsgProcSize;
      if (buffer[0] == reportId) {
        status = buffer[1];
        state = (UINT8)( status & T66_MSG_STATUS_FCALSTATE );
        if (T66_MSG_STATUS_BADSTOREDDATA & status) {
          return eBadStoredData;
        } else {
          DBGPRINT(_T("\nStoreGoldenRefs: GENERATED and FCALPASS not set.\n"));
          return eGRStoredFailed;
        }
      }
    }
    Sleep(300);
  }
  if ( timeoutCounter == MAX_COUNT ) {
    DBGPRINT(_T("\nStoreGoldenRefs: time-out. Status msg did not arrive.\n"));
    return eGoodGRStored;
  }
  return eGoodGRStored;
}

// If PID = 0 (default) use the standard file. If not 0, use PID and directly Thqa bin file

BOOL WriteThqaString(fstream *File, UINT Pid, BOOL DummyThqa)
{
  UNREFERENCED_PARAMETER(File);
  UNREFERENCED_PARAMETER(Pid);
  UNREFERENCED_PARAMETER(DummyThqa);
  fstream thqaFile;
  UINT8 t68Data[512];
  UINT8 t5Msg[16];
  char buffer[512];
  UINT16 address, addressT7, addressT68, cmdAddressT68, addressT5;
  UINT8 t68Size, t5Size;
  UINT8 index, ctrl;
  UINT8 values[16];
  UINT numT68DataBytes, numBytesToCopy;
  UINT totalDataBytes;
  UINT8 T68WriteType;
  DWORD bytesRead, bytesToWrite;
  UINT i, j;
  UINT8 reportId=0;
  UINT8 timeoutCounter;
  UINT32 readCrc, partialCrc=0;

  if (!ReadMemoryMap()) return FALSE;

  if (ConnectFWType == HIDI2CNOINBOX || ConnectFWType == I2CSTOCK) {
    // Old FW: exit
    _stprintf(PrintBuffer, _T("THQA read/writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  // Check if Object T68 exists
  if (!GetObjectIndex(SPT_SERIALDATACOMMAND_T68, &index)) {
    // Cannot get T68 address... Wrong FW?
    _stprintf(PrintBuffer, _T("THQA read/writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  addressT68 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t68Size = AllObjects[index]->Size + 1;

  if (!GetObjectIndex(GEN_MESSAGEPROCESSOR_T5, &index)) {
    // Cannot get T5 address...should never happen...
    DBGPRINT(_T("Cannot read object T5.\n"));
    return FALSE;
  }
  addressT5 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t5Size = AllObjects[index]->Size;

  DBGPRINT(_T("Before start\n"));
  if (!DummyThqa) {
    if (!GetObjectIndex(GEN_POWERCONFIG_T7, &index)) {
      // Cannot get T7 address...should never happen...
      DBGPRINT(_T("Cannot read object T7.\n"));
      return FALSE;
    }
    addressT7 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
    bytesRead = AllObjects[index]->Size + 1;

//  _stprintf(PrintBuffer, _T("Address T7: 0x%x  "), addressT7);
//  _TPRINTF(PrintBuffer);
//  _stprintf(PrintBuffer, _T("bytes: 0x%x\n"), bytesRead);
//  _TPRINTF(PrintBuffer);

  if (!ReadFromDevice(addressT7, t68Data, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T7.\n"));
      return FALSE;
    }
    if (t68Data[0] == 0) {
      _stprintf(PrintBuffer, _T("The chip is in deep sleep, and so will not process T68 serial data commands.\nPlease modify T7 object (IDLEACQINT not 0).\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }

  bytesToWrite = 1;
  ctrl = 3;
  if (!WriteToDevice(addressT68, &ctrl, &bytesToWrite)) {
    DBGPRINT(_T("Failed to write to device - Object 68. Error: %d\n"), GetLastError());
    return FALSE;
  }

  // get reportId of SPT_SERIALDATACOMMAND_T68
  reportId = GetReportIdFromType(SPT_SERIALDATACOMMAND_T68);
  if (reportId == 0) {
    _stprintf(PrintBuffer, _T("THQA read/writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

#ifndef AUTO_UPDATER
  // if Pid is non-zero, use Pid and Thqa file directly
  if (Pid) {
    values[0] = (PidToWrite >> 8) & 0x00FF;
    values[1] = PidToWrite & 0x00FF;
    values[2] = 0x03;
    values[3] = 0x01;
    values[4] = 0x05;

    File->seekg (0, ios::end);
    totalDataBytes = (UINT)File->tellg();
    File->seekg (0, ios::beg);
    File->read((char *)buffer, totalDataBytes);

  } else {
    File->getline(buffer, 512);
    if (buffer[0] != '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    File->getline(buffer, 512);
    if (buffer[0] == '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    ReplaceChar(buffer);
    sscanf(buffer, "%hhx %hhx", &values[0], &values[1]);

    File->getline(buffer, 512);
    if (buffer[0] != '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    File->getline(buffer, 512);
    if (buffer[0] == '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    ReplaceChar(buffer);
    sscanf(buffer, "%hhx %hhx", &values[2], &values[3]);

    File->getline(buffer, 512);
    if (buffer[0] != '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    File->getline(buffer, 512);
    if (buffer[0] == '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    ReplaceChar(buffer);
    sscanf(buffer, "%hhx", &values[4]);

    File->getline(buffer, 512);
    if (buffer[0] != '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    File->getline(buffer, 512);
    if (buffer[0] == '[') {
      _stprintf(PrintBuffer, _T("Invalid structure of Input file.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }

    thqaFile.open(buffer, ifstream::in | ifstream::binary);
    if(!thqaFile.is_open()) {
      _stprintf(PrintBuffer, _T("The THQA binary file indicated in your input file was not found.\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }

    // check how much data we have to send
    // get length of file:
    thqaFile.seekg (0, ios::end);
    totalDataBytes = (UINT)thqaFile.tellg();
    thqaFile.seekg (0, ios::beg);

    thqaFile.read((char *)buffer, totalDataBytes);
  }
#else

    values[0] = (PidToWrite >> 8) & 0x00FF;
    values[1] = PidToWrite & 0x00FF;
    values[2] = 0x03;
    values[3] = 0x01;
    values[4] = 0x05;

    totalDataBytes = ThqaFileSize;
    for (i = 0; i < ThqaFileSize; ++i) {
      buffer[i] = ThqaFileRes[i];
    }

#endif

  } else {
    values[0] = 0x21;
    values[1] = 0x2C;
    values[2] = 0x03;
    values[3] = 0x01;
    values[4] = 0x05;

    totalDataBytes = 256;
    t68Data[0] = 0xFC;
    t68Data[1] = 0x28;
    t68Data[2] = 0xFE;
    t68Data[3] = 0x40;

    for (i = 0; i < totalDataBytes; i+=4) {
      for(j = 0; j < 4; ++j) {
        buffer[i+j] =  t68Data[j];
      }
    }
  }
  ZeroMemory(t68Data, 512);

  numT68DataBytes = t68Size - 9;

  // set the T68 ctrl field: enable, report enable
  ZeroMemory(t68Data, 512);
  t68Data[0] = 3;
  t68Data[3] = DATA_TYPE_LOW;
  t68Data[4] = DATA_TYPE_HIGH;
  // Read from input file...
  for (i = 0; i < 5; ++i) {
    t68Data[6+i] = values[i];
  }
  
  T68WriteType = T68_CMD_START;
  cmdAddressT68 = addressT68 + 6 + (UINT16)numT68DataBytes;
  UINT k = 5;
  j = 0;
  for (;;) {
    // copy this much data from the input data into the T68 object
    numBytesToCopy = min(totalDataBytes, numT68DataBytes);
    for (i = 0; i < numBytesToCopy - k; i++)
    {
      t68Data[6 + k + i] = buffer[j++];
    }
    // set the T68 length field
    t68Data[5] = (byte) numBytesToCopy;
    t68Data[6 + numT68DataBytes] = T68WriteType;
    bytesToWrite = numBytesToCopy + 6;
    partialCrc = CalculateCRC(&t68Data[6], bytesToWrite - 6, partialCrc);
    if (!WriteToDevice(addressT68, t68Data, &bytesToWrite)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
    bytesToWrite = 1;
    if (!WriteToDevice(cmdAddressT68, &T68WriteType, &bytesToWrite)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
    Sleep(300);
    for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
      bytesRead = t5Size;
      if (!ReadFromDevice(addressT5, t5Msg, &bytesRead)) {
        DBGPRINT(_T("Cannot read object T5.\n"));
        return FALSE;
      }
      if (t5Msg[0] == reportId) break;
      if (t5Msg[0] == 0xff) Sleep (300);
    }

    if ( timeoutCounter == MAX_COUNT ) {
      DBGPRINT(_T("WriteThqaString: time-out. T68 msg (1) did not arrive.\n"));
      return FALSE;
    }

    readCrc = (t5Msg[4] << 16) + (t5Msg[3] << 8) + t5Msg[2];
    if (readCrc != partialCrc) {
      _stprintf(PrintBuffer, _T("Crc mismatch: communication error...\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    totalDataBytes = totalDataBytes - numBytesToCopy + k;
    if (k > 0) k = 0; // Only the first time accomodate for 5 extra bytes (PID etc...)
    T68WriteType = T68_CMD_CONTINUE;
    if ((int)totalDataBytes <= 0) {
      T68WriteType = T68_CMD_END;
      t68Data[0] = 3;
      t68Data[3] = DATA_TYPE_LOW;
      t68Data[4] = DATA_TYPE_HIGH;
      t68Data[5] = 0;   //data length 0 (end)
      // set the T68 cmd field
      t68Data[6 + numT68DataBytes] = T68WriteType;
      bytesToWrite = numT68DataBytes + 6;
      if (!WriteToDevice(addressT68, t68Data, &bytesToWrite)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      bytesToWrite = 1;
      if (!WriteToDevice(cmdAddressT68, &T68WriteType, &bytesToWrite)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(300);
      for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
        bytesRead = t5Size;
        if (!ReadFromDevice(addressT5, t5Msg, &bytesRead)) {
          DBGPRINT(_T("Cannot read object T5.\n"));
          return FALSE;
        }
        if (t5Msg[0] == reportId) break;
        if (t5Msg[0] == 0xff) Sleep (300);
      }
      if ( timeoutCounter == MAX_COUNT ) {
        DBGPRINT(_T("WriteThqaString: time-out. T68 msg (2) did not arrive.\n"));
        return FALSE;
      }
      break;
    }
    ZeroMemory(t68Data, 512);
    t68Data[0] = 3;
    t68Data[3] = DATA_TYPE_LOW;
    t68Data[4] = DATA_TYPE_HIGH;
    // Read from input file...
    for (i = 0; i < 5; ++i) {
      t68Data[6+i] = values[i];
    }
  }
  if (!GetObjectIndex(SPT_DIGITIZER_T43, &index)) {
    // Cannot get T7 address...should never happen...
    DBGPRINT(_T("Cannot read object T43.\n"));
    return FALSE;
  }

  address = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  bytesRead = 1;
  if (!ReadFromDevice(address, &ctrl, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T43.\n"));
    return FALSE;
  }
  ctrl |= 0x80; 
  bytesToWrite = 1;
  if (!WriteToDevice(address, &ctrl, &bytesToWrite)) {
    DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  Sleep(1000);
  if (!ResetDevice()) {
    DBGPRINT(_T("Failed to Reset device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  return TRUE;
}

#define LINE_SIZE 4096

BOOL WriteT68(fstream *File)
{
  UINT8 t68Data[80];
  UINT8 t5Msg[16];
  string readString;
  byte *buffer;
  UINT16 addressT7, addressT68, addressT5;
  UINT8 t68Size, t5Size;
  UINT8 index, ctrl;
  UINT numT68DataBytes, numBytesToCopy;
  UINT totalDataBytes;
  UINT8 T68WriteType;
  DWORD bytesRead, bytesToWrite;
  UINT i, j;
  UINT8 reportId;
  UINT8 timeoutCounter;
  UINT32 readCrc, partialCrc=0;
  UINT totalFileSize;

  if (!ReadMemoryMap()) return FALSE;

  // Check if Object T68 exists
  if (!GetObjectIndex(SPT_SERIALDATACOMMAND_T68, &index)) {
    // Cannot get T68 address... Wrong FW?
    _stprintf(PrintBuffer, _T("T68 writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  addressT68 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t68Size = AllObjects[index]->Size + 1;

  // Enable object T68...
  bytesToWrite = 1;
  ctrl = 3;
  if (!WriteToDevice(addressT68, &ctrl, &bytesToWrite)) {
    DBGPRINT(_T("Failed to write to device - Object 68. Error: %d\n"), GetLastError());
    return FALSE;
  }

  // get reportId of SPT_SERIALDATACOMMAND_T68
  reportId = GetReportIdFromType(SPT_SERIALDATACOMMAND_T68);
  if (reportId == 0) {
    _stprintf(PrintBuffer, _T("T68 writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  if (!GetObjectIndex(GEN_MESSAGEPROCESSOR_T5, &index)) {
    // Cannot get T5 address...should never happen...
    DBGPRINT(_T("Cannot read object T5.\n"));
    return FALSE;
  }
  addressT5 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t5Size = AllObjects[index]->Size;

  if (GetObjectIndex(GEN_POWERCONFIG_T7, &index)) {
    addressT7 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
    bytesRead = AllObjects[index]->Size + 1;

    if (!ReadFromDevice(addressT7, t68Data, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T7.\n"));
      return FALSE;
    }
    if (t68Data[0] == 0) {
      _stprintf(PrintBuffer, _T("The chip is in deep sleep, and so will not process T68 serial data commands.\nPlease modify T7 object (IDLEACQINT not 0).\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
  }

  //get total filesize
  File->seekg(0, std::ios::end);
  totalFileSize = (UINT)File->tellg();
  File->seekg(0);
  buffer = new byte[totalFileSize];
  File->read((char *)buffer, totalFileSize);

  T68WriteType = T68_CMD_START;
  numT68DataBytes = t68Size - 9;
  ZeroMemory(t68Data, 80);
  // Enable
  t68Data[0] = 3;
  t68Data[3] = 6;

  j = 0;
  totalDataBytes = totalFileSize;

  for (;;) {
    ZeroMemory(&t68Data[6], 74);
    // copy this much data from the input data into the T68 object
    numBytesToCopy = min(totalDataBytes, numT68DataBytes);
    for (i = 0; i < numBytesToCopy; i++)
    {
      t68Data[6 + i] = buffer[j++];
    }
    // set the T68 length field
    t68Data[5] = (byte) numBytesToCopy;
    t68Data[6 + numT68DataBytes] = T68WriteType;
    // 6 is the header
    bytesToWrite = numBytesToCopy + 6;
    partialCrc = CalculateCRC(&t68Data[6], bytesToWrite - 6, partialCrc);
    // Total bytes to write to T68
    bytesToWrite = numT68DataBytes + 7;
    if (!WriteToDevice(addressT68, t68Data, &bytesToWrite)) {
      DBGPRINT(_T("\nFailed to write to device. Error: %d\n"), GetLastError());
      delete[] buffer;
      return FALSE;
    }
    Sleep(500);

    for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
      bytesRead = t5Size;
      if (!ReadFromDevice(addressT5, t5Msg, &bytesRead)) {
        DBGPRINT(_T("\nCannot read object T5.\n"));
        delete[] buffer;
        return FALSE;
      }
      if (t5Msg[0] == reportId) break;
      if (t5Msg[0] == 0xff) Sleep (300);
    }

    if ( timeoutCounter == MAX_COUNT ) {
      DBGPRINT(_T("\nWriteT68: time-out. T68 msg (1) did not arrive.\n"));
      delete[] buffer;
      return FALSE;
    }

    readCrc = (t5Msg[4] << 16) + (t5Msg[3] << 8) + t5Msg[2];
    if (readCrc != partialCrc) {
      _stprintf(PrintBuffer, _T("\nCrc mismatch: communication error...\n"));
      _TPRINTF(PrintBuffer);
      delete[] buffer;
      return FALSE;
    }

    Sleep(300);
    totalDataBytes = totalDataBytes - numBytesToCopy;
    T68WriteType = T68_CMD_CONTINUE;
    _tprintf(_T("."));
    if ((int)totalDataBytes <= 0) {
      break;
    }
  }

  T68WriteType = T68_CMD_END;
  t68Data[5] = 0;   //data length 0 (end)
  // set the T68 cmd field
  t68Data[6 + numT68DataBytes] = T68WriteType;
  bytesToWrite = numT68DataBytes + 7;
  if (!WriteToDevice(addressT68, t68Data, &bytesToWrite)) {
    DBGPRINT(_T("\nFailed to write to device. Error: %d\n"), GetLastError());
    return FALSE;
  }

  _tprintf(_T("\n"));
  delete[] buffer;
  return TRUE;
}

BOOL WriteSerialNum(fstream *File, BOOL ClearMem)
{
	UINT8 T68Status = 0;
	UINT8 t68SerialData[80];
	UINT8 t5Msg[16];
	byte *buffer;
	UINT16 addressT7, addressT68, addressT5;
	UINT8 t68Size, t5Size;
	UINT8 index, ctrl;
	UINT numT68DataBytes, numBytesToCopy;
	UINT dataArraySize;
	UINT totalDataBytes;
	UINT UTFDataBytes;
	UINT8 T68WriteType;
	DWORD bytesRead, bytesToWrite;
	UINT i, j;
	UINT8 reportId;
	UINT8 timeoutCounter;
	UINT32 readCrc, partialCrc = 0;
	UINT totalFileSize;
	UINT8 WriteAttempt = 3;

	if (!ReadMemoryMap()) return FALSE;

	// Check if Object T68 exists
	if (!GetObjectIndex(SPT_SERIALDATACOMMAND_T68, &index)) {
		// Cannot get T68 address... Wrong FW?
		_stprintf(PrintBuffer, _T("T68 writing not supported by this version of firmware\n"));
		_TPRINTF(PrintBuffer);
		return FALSE;
	}
	addressT68 = AllObjects[index]->LSB | ((AllObjects[index]->MSB << 8) & 0xFF00);
	t68Size = AllObjects[index]->Size + 1;

	// Enable object T68...
	bytesToWrite = 1;
	ctrl = 3;
	if (!WriteToDevice(addressT68, &ctrl, &bytesToWrite)) {
		DBGPRINT(_T("Failed to write to device - Object 68. Error: %d\n"), GetLastError());
		return FALSE;
	}

	// get reportId of SPT_SERIALDATACOMMAND_T68
	reportId = GetReportIdFromType(SPT_SERIALDATACOMMAND_T68);
	if (reportId == 0) {
		_stprintf(PrintBuffer, _T("T68 writing not supported by this version of firmware\n"));
		_TPRINTF(PrintBuffer);
		return FALSE;
	}

	if (!GetObjectIndex(GEN_MESSAGEPROCESSOR_T5, &index)) {
		// Cannot get T5 address...should never happen...
		DBGPRINT(_T("Cannot read object T5.\n"));
		return FALSE;
	}
	addressT5 = AllObjects[index]->LSB | ((AllObjects[index]->MSB << 8) & 0xFF00);
	t5Size = AllObjects[index]->Size;

	if (GetObjectIndex(GEN_POWERCONFIG_T7, &index)) {
		addressT7 = AllObjects[index]->LSB | ((AllObjects[index]->MSB << 8) & 0xFF00);
		bytesRead = AllObjects[index]->Size + 1;

		if (!ReadFromDevice(addressT7, t68SerialData, &bytesRead)) {
			DBGPRINT(_T("Cannot read object T7.\n"));
			return FALSE;
		}
		if (t68SerialData[0] == 0) {
			_stprintf(PrintBuffer, _T("The chip is in deep sleep, and so will not process T68 serial data commands.\nPlease modify T7 object (IDLEACQINT not 0).\n"));
			_TPRINTF(PrintBuffer);
			return FALSE;
		}
	}

	//Get Filesize.  Skip if just clearing T68 buffer
	if (!ClearMem) {
		//get total filesize
		File->seekg(0, std::ios::end);
		totalFileSize = (UINT)File->tellg();
		File->seekg(0);
		buffer = new byte[totalFileSize];
		File->read((char *)buffer, totalFileSize);
	}

	T68WriteType = T68_CMD_START;
	numT68DataBytes = t68Size - 9;	//Always use total object size to caculate the location of the CMD byte.  Independent of length of data array

	if (DeviceConnected == emXT2952TM) {		// Special size is only 60 of data array
		dataArraySize = 60;
	}
	else {
		dataArraySize = numT68DataBytes;		// Temp:  Need to check each product for datasize (TODO: use a table).
	}
	
	ZeroMemory(t68SerialData, 80);		//buffer just needs to be larger than current size of one T68 page
	// Enable
	t68SerialData[0] = 3;
	t68SerialData[3] = 7;	//Datatype is 0x07 for serial numbers

	if (!ClearMem) {
		totalDataBytes = totalFileSize;
	}
	else {
		if (DeviceConnected == emXT2952TM) {	//Only perform if clearing all data array bytes.
			totalDataBytes = dataArraySize;		//Clear only data array, different for each device.
		}
	}

	for (;;)	{
		ZeroMemory(&t68SerialData[6], 74);	//This is total size of the buffer including header
		// copy this much data from the input data into the T68 object
		numBytesToCopy = min(totalDataBytes, numT68DataBytes);

		//Variable to be used for total file size in UTF16 conversion
		if (!ClearMem) UTFDataBytes = (totalDataBytes * 2);

		if (!ClearMem)
		{
			j = 0;

			for (i = 0; i < (numBytesToCopy*2); i++)		//Double the numBytesToCopy to convert from ASCII to UTF16
			{												//Only converting (A-Z), (a-z), and (0-9)
				t68SerialData[6 + i] = buffer[j++];
				t68SerialData[6 + (i+1)] = 0x00;
				i = i + 1;
			}
		}
		// set the T68 length field
		if (ClearMem) {
			if (DeviceConnected == emXT2952TM) {
				t68SerialData[5] = (byte)dataArraySize;
			}
		}
		else { 
			t68SerialData[5] = ((byte)numBytesToCopy * 2); //Double the number of bytes after conversion
		}

		//Convert length value to add to buffer.
		t68SerialData[6 + numT68DataBytes] = T68WriteType;	//Calculate the CMD address

		// Clear T68 memory size is 1x, UTF16 is 2x the size
		if (!ClearMem) {
			bytesToWrite = (numBytesToCopy * 2) + 6;	
		}
		else {
			bytesToWrite = numBytesToCopy + 6;
		}

		partialCrc = CalculateCRC(&t68SerialData[6], bytesToWrite - 6, partialCrc);
		// Total bytes to write to T68
		bytesToWrite = numT68DataBytes + 7;
		
		while (WriteAttempt) {

			//_stprintf(PrintBuffer, _T("\nAttempt Count %d\n"), WriteAttempt);	// Debug Code Only
			//_TPRINTF(PrintBuffer);

			if (!WriteToDevice(addressT68, t68SerialData, &bytesToWrite)) {
				DBGPRINT(_T("\nFailed to write to device. Error: %d\n"), GetLastError());
				if (!ClearMem) delete[] buffer;
				return FALSE;
			}
			Sleep(1000);	
 			for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
				bytesRead = t5Size;
				if (!ReadFromDevice(addressT5, t5Msg, &bytesRead)) {
					DBGPRINT(_T("\nCannot read object T5.\n"));
					if (!ClearMem) delete[] buffer;
					return FALSE;
				}
				if (t5Msg[0] == reportId) break;
				if (t5Msg[0] == 0xff) Sleep(300);
			}

			if (timeoutCounter == MAX_COUNT) {
				DBGPRINT(_T("\nWriteT68: time-out. T68 msg (1) did not arrive.\n"));
				if (!ClearMem) delete[] buffer;
				return FALSE;
			}

			readCrc = (t5Msg[4] << 16) + (t5Msg[3] << 8) + t5Msg[2];	//Get the CRC data from T5 messages
			T68Status = t5Msg[1];	//Get the error status

			Sleep(300);

			if (readCrc != partialCrc) {
				_stprintf(PrintBuffer, _T("\nCrc mismatch: communication error...\n"));
				_TPRINTF(PrintBuffer);

				if (T68Status != 0) {
				_stprintf(PrintBuffer, _T("\nStatus Error...%d\n"), T68Status);
				_TPRINTF(PrintBuffer);
				}
				if (!ClearMem) delete[] buffer;
			}
			else {
				break;
			}
			--WriteAttempt;
		}	//While loop to attempt a CMD start write 3 times.  Done in case the T68 write is out of sequence

		if (WriteAttempt == 0x00) {
			_stprintf(PrintBuffer, _T("\nWrite to T68 Failed.\n"));
			_TPRINTF(PrintBuffer);

			if (!ClearMem) delete[] buffer;
			return FALSE;
		}
		else {
			WriteAttempt = 0x03;  //Reset counter
		}

		Sleep(300);
		T68WriteType = T68_CMD_CONTINUE;

		//Determine if CMD Continue is needed based on the total size of the data
		if (!ClearMem) {
			UTFDataBytes = UTFDataBytes - (numBytesToCopy*2);		//Recalculate this again
		}
		else {
			totalDataBytes = totalDataBytes - dataArraySize;
		}
		
		if (!ClearMem) {
			if ((int)UTFDataBytes <= 0) {
				break;
			}
		}
		else {
			if ((int)totalDataBytes <= 0) {
				break;
			}
		}
	}

	T68WriteType = T68_CMD_END;

	if (!ClearMem) {
		//t68SerialData[5] = 0;   //data length 0 (end)
		t68SerialData[5] = 0x02; //(byte)numBytesToCopy;  For serial number, write two additional 0x00 end points.//
		t68SerialData[6] = 0x00;
		t68SerialData[7] = 0x00;	//Need two byte 0x00 0x00 string for termination
	}
	else {
		t68SerialData[5] = 0x00;   //data length 0 (end)
	}
					  
	// set the T68 cmd field
	t68SerialData[6 + numT68DataBytes] = T68WriteType;
	bytesToWrite = numT68DataBytes + 7;
	//Don't need to loop on this CMD end write
	if (!WriteToDevice(addressT68, t68SerialData, &bytesToWrite)) {
		DBGPRINT(_T("\nFailed to write to device. Error: %d\n"), GetLastError());
		return FALSE;
	}
	else {
	//	_stprintf(PrintBuffer, _T("\nT68 Write Done . . .\n"));
	//	_TPRINTF(PrintBuffer);
	}

	Sleep(300);

	for (timeoutCounter = 0; timeoutCounter < MAX_COUNT; ++timeoutCounter) {
		bytesRead = t5Size;
		if (!ReadFromDevice(addressT5, t5Msg, &bytesRead)) {
			DBGPRINT(_T("\nCannot read object T5.\n"));
			if (!ClearMem) delete[] buffer;
			return FALSE;
		}
		if (t5Msg[0] == reportId) break;
		if (t5Msg[0] == 0xff) Sleep(300);
	}

	if (timeoutCounter == MAX_COUNT) {
		DBGPRINT(_T("\nWriteT68: time-out. T68 msg (1) did not arrive.\n"));
		if (!ClearMem) delete[] buffer;
		return FALSE;
	}

	readCrc = (t5Msg[4] << 16) + (t5Msg[3] << 8) + t5Msg[2];	//Get the CRC data from T5 messages
	T68Status = t5Msg[1];	//Get the error status

	Sleep(300);

	if (T68Status != 0) {
		_stprintf(PrintBuffer, _T("\nStatus Error, Write did not complete %d,\n"), T68Status);
		_TPRINTF(PrintBuffer);
		if (!ClearMem) delete[] buffer;
		return FALSE;
	}

	//_tprintf(_T("\n"));
	if (!ClearMem) delete[] buffer;

	return TRUE;
}

#ifdef AUTO_UPDATER
BOOL ReadThqaString(UINT8 *Thqa)
#else
BOOL ReadThqaString(fstream *File)
#endif
{
  UINT8 thqa[512];
  UINT8 buffer[512];
  UINT16 addressT7, addressT37;
  UINT8 t37Size;
  UINT8 index;
  UINT8 diagField;
  DWORD bytesRead;

  if (!ReadMemoryMap()) return FALSE;

  diagField = T37_THQA;
  if (DeviceConnected == eMXT3432S) {
    diagField = T37_THQA_3432;
  }

  if (ConnectFWType == HIDI2CNOINBOX || ConnectFWType == I2CSTOCK) {
    // Old FW: exit
    _stprintf(PrintBuffer, _T("THQA read/writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  // Check if Object T68 exists
  if (!GetObjectIndex(SPT_SERIALDATACOMMAND_T68, &index)) {
    // Cannot get T68 address... Wrong FW?
    _stprintf(PrintBuffer, _T("THQA read/writing not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }
  if (!GetObjectIndex(GEN_POWERCONFIG_T7, &index)) {
    // Cannot get T7 address...should never happen...
    DBGPRINT(_T("Cannot read object T7.\n"));
    return FALSE;
  }
  addressT7 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  bytesRead = AllObjects[index]->Size + 1;

  if (!ReadFromDevice(addressT7, buffer, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T7.\n"));
    return FALSE;
  }
  if (buffer[0] == 0) {
    _stprintf(PrintBuffer, _T("The chip is in deep sleep, and so will not process T68 serial data commands.\nPlease modify T7 object (IDLEACQINT not 0).\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  addressT37 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t37Size = AllObjects[index]->Size + 1;

  if(!SetDiagnosticField(diagField)) {
    DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
    return FALSE;
  }

  ZeroMemory(thqa, 512);
  for( UINT j = 0; j < 3; ++j) {
    Sleep(50);
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
    // Save buffer somewhere
    memcpy(&thqa[j*(t37Size-2)], &buffer[2], t37Size-2);

    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
  }

#ifdef AUTO_UPDATER
  memcpy(Thqa, (UINT8 *)&thqa[3], 0x100);
#else
  File->write((char *)&thqa[3], 0x100);
  File->flush();
#endif

  return TRUE;
}

BOOL ReadSerialNumString(fstream *File)
{
	UINT8 serialnum[512];
	UINT8 s_buffer[512];
	UINT8 o_buffer[512];
	UINT16 s_addressT7, s_addressT37;
	UINT8 s_t37Size;
	UINT8 s_index;
	UINT8 s_diagField;
	DWORD s_bytesRead;
	UINT8 y = 0;
	UINT8 z = 0;

	if (!ReadMemoryMap()) return FALSE;

	//Set the T37 to read the serial number through T37
	s_diagField = T37_SERIAL_NUM;

/*	TODO:  Place code here to prevent usage if not the 29252TM
if (ConnectFWType == HIDI2CNOINBOX || ConnectFWType == I2CSTOCK) {
		// Old FW: exit
		_stprintf(PrintBuffer, _T("THQA read/writing not supported by this version of firmware\n"));
		_TPRINTF(PrintBuffer);
		return FALSE;
	}
*/

	// Check if Object T68 exists
	if (!GetObjectIndex(SPT_SERIALDATACOMMAND_T68, &s_index)) {
		// Cannot get T68 address... Wrong FW?
		_stprintf(PrintBuffer, _T("Serial read/writing not supported by this version of firmware\n"));
		_TPRINTF(PrintBuffer);
		return FALSE;
	}
	if (!GetObjectIndex(GEN_POWERCONFIG_T7, &s_index)) {
		// Cannot get T7 address...should never happen...
		DBGPRINT(_T("Cannot read object T7.\n"));
		return FALSE;
	}
	s_addressT7 = AllObjects[s_index]->LSB | ((AllObjects[s_index]->MSB << 8) & 0xFF00);
	s_bytesRead = AllObjects[s_index]->Size + 1;

	if (!ReadFromDevice(s_addressT7, s_buffer, &s_bytesRead)) {
		DBGPRINT(_T("Cannot read object T7.\n"));
		return FALSE;
	}
	if (s_buffer[0] == 0) {
		_stprintf(PrintBuffer, _T("The chip is in deep sleep, and so will not process T68 serial data commands.\nPlease modify T7 object (IDLEACQINT not 0).\n"));
		_TPRINTF(PrintBuffer);
		return FALSE;
	}

	if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &s_index)) {
		// Cannot get T37 address...should never happen...
		DBGPRINT(_T("Cannot read object T37.\n"));
		return FALSE;
	}
	s_addressT37 = AllObjects[s_index]->LSB | ((AllObjects[s_index]->MSB << 8) & 0xFF00);
	s_t37Size = AllObjects[s_index]->Size + 1;

	if (!SetDiagnosticField(s_diagField)) {
		DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
		return FALSE;
	}

	SecureZeroMemory(serialnum, 512);		//Buffer size of 512 needs to larger than T37 size
	

	for (UINT j = 0; j < 3; ++j) {
		Sleep(50);
		s_bytesRead = s_t37Size;
		if (!ReadFromDevice(s_addressT37, s_buffer, &s_bytesRead)) {
			DBGPRINT(_T("Cannot read object T37.\n"));
			return FALSE;
		}
		// Save buffer somewhere
		memcpy(&serialnum[j*(s_t37Size - 2)], &s_buffer[2], s_t37Size - 2);
		
		if (!SetDiagnosticField(T37_PAGE_UP)) {
			DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
			return FALSE;
		}
	}

	SecureZeroMemory(o_buffer, 512);		//Use SecureZeroMemory, ZeroMemory will corrupt other buffers being used.

	for (UINT i = 2; i < s_bytesRead; ++i) {
		if (serialnum[i] != 0x00) {
			o_buffer[z] = serialnum[i];
			++z;
		}

		if ((serialnum[i + 1] == 0x00) && (serialnum[i + 2] == 0x00) && (serialnum[i+3] == 0x00)){
		//Found terminator string
			o_buffer[z + 1] = 0x00;	//Add the null to terminate the string
			break;
		}
	}

	

#ifdef AUTO_UPDATER
	memcpy(Thqa, (UINT8 *)&thqa[3], 0x100);
#else
	File->write((char *)&o_buffer[0], (z+1));	//Copy only the amount of characters into the buffer
	File->flush();
#endif

	return TRUE;
}

BOOL ActivatePidThqa(BOOL Enable)
{
  UINT16 address;
  UINT8 index, ctrl;
  DWORD bytesRead, bytesToWrite;

  if (!ReadMemoryMap()) return FALSE;

  if (ConnectFWType == HIDI2CNOINBOX || ConnectFWType == I2CSTOCK) {
    // Old FW: exit
    _stprintf(PrintBuffer, _T("THQA handling not supported by this version of firmware\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  BackupDevice(0x33);
  Sleep(2000);

  if (!GetObjectIndex(SPT_DIGITIZER_T43, &index)) {
    // Cannot get T43 address...should never happen...
    DBGPRINT(_T("Cannot read object T43.\n"));
    return FALSE;
  }

  address = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  bytesRead = 1;
  if (!ReadFromDevice(address, &ctrl, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T43.\n"));
    return FALSE;
  }
  if (Enable) {
    ctrl |= 0x80;
  } else {
    ctrl &= 0x7F;
  }

  bytesToWrite = 1;
  if (!WriteToDevice(address, &ctrl, &bytesToWrite)) {
    DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  Sleep(1000);
  if (!BackupAndResetDevice()) {
    DBGPRINT(_T("Failed to Backup/Reset device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  return TRUE;
}

// removes "," and "u"
void ReplaceChar(char *Line)
{
  char* pos;
  
  do {
    pos = strchr(Line, ',');
    if(pos != 0 ){
      *pos = ' ';
      continue;
    }
    pos = strchr(Line, 'u');
    if(pos != 0 ){
      *pos = ' ';
      continue;
    }
  } while (pos != 0);
}

BOOL SetDiagnosticField(UINT8 Value)
{
  return(SendCmdToDeviceWithAck(eDiagnostic, Value)); 
}

// PData points to a buffer of at least 130 bytes 
// it is caller responsibility to assure it is big enough
BOOL ReadDiagnosticData(UINT8 *PData)
{
  UINT16 address;
  UINT8 size;
  DWORD bytesRead;
  if (!ReadMemoryMap()) return FALSE;
  // get T37 address...
  if (!GetObjectAddressAndSize(DEBUG_DIAGNOSTIC_T37, &address, &size)) {
    // Cannot get T37 address...
    return FALSE;
  }
  bytesRead = size;
  if (!ReadFromDevice(address, PData, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  return TRUE;
}
/*
/////////////////////////////////////
BOOL ReadT37(fstream *File, UINT TotalValues, UINT8 Mode)
{
  char printBuff[1024];
  UINT8 buffer[512];
  UINT16 addressT37;
  UINT8 t37Size;
  UINT8 index;
  DWORD bytesRead;
  UINT pageNum, pageSize; 
  UINT16 *pRefs;
  UINT i, j;
  
  if (!ReadMemoryMap()) return FALSE;

  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  addressT37 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t37Size = AllObjects[index]->Size + 1;

  if(!SetDiagnosticField(Mode)) {
    DBGPRINT(_T("Failed to SetDiagnosticField to device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  pageSize = (t37Size-2)/2;
  pageNum = TotalValues/pageSize;
  ZeroMemory(printBuff, 1024);

  for(i = 0; i <= pageNum; ++i) {
    Sleep(50);
    bytesRead = t37Size;
    pRefs = (UINT16 *)&buffer[2];
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
    ZeroMemory(printBuff, 1024);
    sprintf(printBuff, "Page #: %d\n", i);
    for (j = 0; j < pageSize; ++j) {
      sprintf(printBuff, "%s%.4X,", printBuff, *pRefs++);
    }
    sprintf(printBuff, "%s\n", printBuff);
    File->write(printBuff, strlen(printBuff));
    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
  }
  File->flush();
  return TRUE;
}
/////////////////////////////////////
*/
// Specialized functions for self-cap
BOOL ReadSCDeltasRefs2952(fstream *File, UINT8 Mode)
{
  char printBuff[1024];
  UINT8 buffer[512];
  UINT16 addressT37;
  UINT8 t37Size;
  UINT8 index;
  UINT pageSize;
  DWORD bytesRead;
  UINT xLines, yLines;
  UINT16 *pRefs;
  INT16 *pDeltas;
  UINT i, j, n;
  UINT k = 0;
  UINT reservedPages = 4;
  UINT numPages;

  xLines = DeviceMemoryMap.MatrixSizeX;
  yLines = DeviceMemoryMap.MatrixSizeY;

  if (xLines*yLines == 2952) {
    yLines--;
    reservedPages = 4;
  }

  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  addressT37 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t37Size = AllObjects[index]->Size + 1;
  pageSize = (t37Size-2)/2;
  numPages = (xLines + yLines)/pageSize;
  if ((xLines + yLines)%pageSize) numPages++;

  if(!SetDiagnosticField(Mode)) {
    DBGPRINT(_T("Failed to SetDiagnosticField to device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  Sleep(50);
  sprintf(printBuff, "Self-Cap Touch Deltas\n");
  if (Mode == T37_SC_REFS_MODE) {
    sprintf(printBuff, "Self-Cap Touch References\n");
  }
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  sprintf(printBuff, "Y0,");
  for (n = 1; n < yLines; ++n) {
    sprintf(printBuff, "%sY%i,", printBuff, n);
  }
  sprintf(printBuff, "%s\n", printBuff);
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  for(i = 0; i < numPages; ++i) {
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
    if (Mode == T37_SC_REFS_MODE) {
      pRefs = (UINT16 *)&buffer[2];
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%u,", printBuff, *pRefs++);
        ++k;
        if (k == yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          if (DeviceMemoryMap.MatrixSizeX*DeviceMemoryMap.MatrixSizeY == 2952) {
            // skip two reserved bytes (one UINT16)
            pRefs++;
            ++j;
          }
          ZeroMemory(printBuff, 1024);
          sprintf(printBuff, "X0,");
          for (n = 1; n < xLines; ++n) {
            sprintf(printBuff, "%sX%i,", printBuff, n);
          }
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
        }
        if (k == xLines + yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
        }
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(50);
    } else {
      pDeltas = (INT16 *)&buffer[2];
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%d,", printBuff, *pDeltas++);
        ++k;
        if (k == yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          // skip two reserved bytes (one INT16)
          if (DeviceMemoryMap.MatrixSizeX*DeviceMemoryMap.MatrixSizeY == 2952) {
            pDeltas++;
            ++j;
          }
          ZeroMemory(printBuff, 1024);
          sprintf(printBuff, "X0,");
          for (n = 1; n < xLines; ++n) {
            sprintf(printBuff, "%sX%i,", printBuff, n);
          }
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
          continue;
        }
        if (k == xLines + yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
          break;
        }
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
    }
  }
  // now do the single-ended
  // skip pages
  for(i = 0; i < reservedPages; ++i) {
    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
    Sleep(50);
  }
  sprintf(printBuff, "Single-ended Y Deltas\n");
  if (Mode == T37_SC_REFS_MODE) {
    sprintf(printBuff, "Single-ended Y References\n");
  }
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  sprintf(printBuff, "Y0,");
  for (n = 1; n < yLines; ++n) {
    sprintf(printBuff, "%sY%i,", printBuff, n);
  }
  sprintf(printBuff, "%s\n", printBuff);
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  k=0;
  for(i = 0; i < numPages; ++i) {
    Sleep(50);
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
    if (Mode == T37_SC_REFS_MODE) {
      pRefs = (UINT16 *)&buffer[2];
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%u,", printBuff, *pRefs++);
        ++k;
        if (k == yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
        }
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(50);
    } else {
      pDeltas = (INT16 *)&buffer[2];
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%d,", printBuff, *pDeltas++);
        ++k;
        if (k == yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          break;
        }
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(50);
    }
  }
  File->flush();
  return TRUE;
}

BOOL ReadSCDeltasRefs640(fstream *File, UINT8 Mode)
{
  char printBuff[1024];
  UINT8 buffer[512];
  UINT16 addressT37;
  UINT8 t37Size;
  UINT8 index;
  UINT pageSize;
  DWORD bytesRead;
  UINT xLines, yLines;
  UINT16 *pRefs, *p;
  UINT16 p1Buff16[128], p2Buff16[128];
  INT16 *pDeltas;
  UINT i, j, n, z;
  UINT k = 0;
  UINT reservedPages = 1;
  UINT numPages;
  UINT reservedSpace = 2;
  UINT offset;

  xLines = DeviceMemoryMap.MatrixSizeX;
  yLines = DeviceMemoryMap.MatrixSizeY;
  ZeroMemory(p1Buff16, 256);
  ZeroMemory(p2Buff16, 256);
  ZeroMemory(buffer, 512);

  p = p1Buff16;

  if (xLines*yLines == 640) {
    reservedSpace = 4;
  } else if (xLines*yLines == 1066) {
    reservedSpace = 5;
    reservedPages = 2;
  }
  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  addressT37 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t37Size = AllObjects[index]->Size + 1;
  pageSize = (t37Size-2)/2;
  numPages = (xLines + yLines)/pageSize;
  if ((xLines + yLines)%pageSize) numPages++;

  if(!SetDiagnosticField(Mode)) {
    DBGPRINT(_T("Failed to SetDiagnosticField to device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  Sleep(50);
  sprintf(printBuff, "Self-Cap Touch Deltas\n");
  if (Mode == T37_SC_REFS_MODE) {
    sprintf(printBuff, "Self-Cap Touch References\n");
  }
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  sprintf(printBuff, "Y0,");
  for (n = 1; n < yLines; ++n) {
    sprintf(printBuff, "%sY%i,", printBuff, n);
  }
  sprintf(printBuff, "%s\n", printBuff);
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  for(i = 0; i < numPages; ++i) {
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }

    if (Mode == T37_SC_REFS_MODE) {
      pRefs = (UINT16 *)&buffer[2];
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%u,", printBuff, *pRefs);
        ++k;
        if (k == yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));

          ZeroMemory(printBuff, 1024);
          sprintf(printBuff, "X0,");
          for (n = 1; n < xLines; ++n) {
            sprintf(printBuff, "%sX%i,", printBuff, n);
          }
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
        }
        if (k > yLines) {
          *p = *pRefs;
          p++;
        }
        if (k == yLines + xLines + reservedSpace) {
          // get even and odd values
          ZeroMemory(printBuff, 1024);
          p = p1Buff16;
          offset = xLines/2+reservedSpace;
          if (xLines%2) ++offset;
          for (z = 0; z < xLines/2; ++z) {
            p2Buff16[z*2]=p1Buff16[z];
            p2Buff16[z*2+1]=p1Buff16[z+offset];
            sprintf(printBuff, "%s%u,", printBuff, p2Buff16[z*2]);
            sprintf(printBuff, "%s%u,", printBuff, p2Buff16[z*2+1]);
          }
          // in case of odd number of lines...
          if (xLines%2) {
            p2Buff16[z*2]=p1Buff16[z];
            sprintf(printBuff, "%s%u,", printBuff, p2Buff16[z*2]);
          }
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          break;
        }
        pRefs++;
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(50);
    } else {
      pDeltas = (INT16 *)&buffer[2];
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%d,", printBuff, *pDeltas++);
        ++k;
        if (k == yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
          sprintf(printBuff, "X0,");
          for (n = 1; n < xLines; ++n) {
            sprintf(printBuff, "%sX%i,", printBuff, n);
          }
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
        }
        if (k == xLines + yLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          ZeroMemory(printBuff, 1024);
          break;
        }
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(50);
    }
  }
  // now do the single-ended
  // skip pages
  for(i = 0; i < reservedPages; ++i) {
    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
    Sleep(50);
  }
  sprintf(printBuff, "Single-ended X Deltas\n");
  if (Mode == T37_SC_REFS_MODE) {
    sprintf(printBuff, "Single-ended X References\n");
  }
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  sprintf(printBuff, "X0,");
  for (n = 1; n < xLines; ++n) {
    sprintf(printBuff, "%sX%i,", printBuff, n);
  }
  sprintf(printBuff, "%s\n", printBuff);
  File->write(printBuff, strlen(printBuff));
  ZeroMemory(printBuff, 1024);
  k=0;
  for(i = 0; i < numPages; ++i) {
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }

    if (Mode == T37_SC_REFS_MODE) {
      pRefs = (UINT16 *)&buffer[2];
      if (i == 0) {
        pRefs += yLines;
        p = p1Buff16;
      }
      for (j = 0; j < pageSize-yLines; ++j) {
        *p++ = *pRefs++;
        ++k;
        if (k == xLines + reservedSpace) break;
      }
      // check if 2 pages and first step
      if (i == 0 && numPages == 2) {
        if(!SetDiagnosticField(T37_PAGE_UP)) {
          DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
          return FALSE;
        }
        Sleep(50);
        continue;
      }
      // get even and odd values
      ZeroMemory(printBuff, 1024);
      offset = xLines/2+reservedSpace;
      if (xLines%2) ++offset;
      for (z = 0; z < xLines/2; ++z) {
        p2Buff16[z*2]=p1Buff16[z];
        p2Buff16[z*2+1]=p1Buff16[z+offset];
        sprintf(printBuff, "%s%u,", printBuff, p2Buff16[z*2]);
        sprintf(printBuff, "%s%u,", printBuff, p2Buff16[z*2+1]);
      }
      // in case of odd number of lines...
      if (xLines%2) {
        p2Buff16[z*2]=p1Buff16[z];
        sprintf(printBuff, "%s%u,", printBuff, p2Buff16[z*2]);
      }
      sprintf(printBuff, "%s\n", printBuff);
      File->write(printBuff, strlen(printBuff));
    } else {
      pDeltas = (INT16 *)&buffer[2];
      pDeltas += yLines;
      for (j = 0; j < pageSize; ++j) {
        sprintf(printBuff, "%s%d,", printBuff, *pDeltas++);
        ++k;
        if (k == xLines) {
          sprintf(printBuff, "%s\n", printBuff);
          File->write(printBuff, strlen(printBuff));
          break;
        }
      }
      if(!SetDiagnosticField(T37_PAGE_UP)) {
        DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
        return FALSE;
      }
      Sleep(50);
    }
  }
  File->flush();
  return TRUE;
}

BOOL ReadDeltasRefs(fstream *File, UINT8 Mode, BOOL Page)
{
  char printBuff[1024];
  UINT8 buffer[512];
  UINT16 addressT37;
  UINT8 t37Size;
  UINT8 index;
  DWORD bytesRead;
  UINT pageNum, xChannel = 0, pageSize; 
  int totalValues;
  UINT16 *pRefs;
  INT16 *pDeltas;
  UINT i, j;
  UINT k = 0;

  if (!ReadMemoryMap()) return FALSE;
  if (DeltasRefsFirstTime) {
    DeltasRefsFirstTime = FALSE;
    _stprintf(PrintBuffer,_T("Device Matrix X: %02d\n"), DeviceMemoryMap.MatrixSizeX);
    _TPRINTF(PrintBuffer);
    _stprintf(PrintBuffer,_T("Device Matrix Y: %02d\n"), DeviceMemoryMap.MatrixSizeY);
    _TPRINTF(PrintBuffer);
  }

  if (Mode == T37_SC_DELTAS_MODE || Mode == T37_SC_REFS_MODE) {
    if (DeviceMemoryMap.MatrixSizeY > DeviceMemoryMap.MatrixSizeX) {
      return(ReadSCDeltasRefs2952(File, Mode));
    } else {
      return(ReadSCDeltasRefs640(File, Mode));
    }
  }
  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  addressT37 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t37Size = AllObjects[index]->Size + 1;

  if(!SetDiagnosticField(Mode)) {
    DBGPRINT(_T("Failed to SetDiagnosticField to device. Error: %d\n"), GetLastError());
    return FALSE;
  }
  totalValues = DeviceMemoryMap.MatrixSizeX*DeviceMemoryMap.MatrixSizeY;
  pageSize = (t37Size-2)/2;
  pageNum = totalValues/pageSize;
  ZeroMemory(printBuff, 1024);
  if (!Page) {
    sprintf(printBuff, "X-Y,Y0,");
    for (j = 1; j < DeviceMemoryMap.MatrixSizeY; ++j) {
      sprintf(printBuff, "%sY%i,", printBuff, j);
    }
    sprintf(printBuff, "%s\n", printBuff);
    File->write(printBuff, strlen(printBuff));
    sprintf(printBuff, "X%i,", xChannel);
  }
  for(i = 0; i <= pageNum; ++i) {
    Sleep(50);
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
    if (Page) {
      if (Mode == T37_REFS_MODE) {
        pRefs = (UINT16 *)&buffer[2];
        sprintf(printBuff, "Page %02u - %04u: %u", i, xChannel, *pRefs);
        ++pRefs;
        for (j = 0; j < min((UINT)totalValues, pageSize) - 1; ++j) {
          sprintf(printBuff, "%s,%u", printBuff, *pRefs++);
        }
      } else {
        pDeltas = (INT16 *)&buffer[2];
        sprintf(printBuff, "Page %02u - %04u: %i", i, xChannel, *pDeltas);
        ++pDeltas;
        for (j = 0; j < min((UINT)totalValues, pageSize) - 1; ++j) {
          sprintf(printBuff, "%s,%i", printBuff, *pDeltas++);
        }
      }
      sprintf(printBuff, "%s\n", printBuff);
      xChannel += pageSize;
    } else {
      if (Mode == T37_REFS_MODE) {
        pRefs = (UINT16 *)&buffer[2];
        for (j = 0; j < min((UINT)totalValues, pageSize); ++j) {
          sprintf(printBuff, "%s%u,", printBuff, *pRefs++);
          ++k;
          if (k == (UINT)DeviceMemoryMap.MatrixSizeY) {
            sprintf(printBuff, "%s\n", printBuff);
            ++xChannel;
            k = 0;
            if (xChannel < DeviceMemoryMap.MatrixSizeX) {
              sprintf(printBuff, "%sX%i,", printBuff, xChannel);
            }
          }
        }
      } else {
        pDeltas = (INT16 *)&buffer[2];
        for (j = 0; j < min((UINT)totalValues, pageSize); ++j) {
          sprintf(printBuff, "%s%i,", printBuff, *pDeltas++);
          ++k;
          if (k == (UINT)DeviceMemoryMap.MatrixSizeY) {
            sprintf(printBuff, "%s\n", printBuff);
            ++xChannel;
            k = 0;
            if (xChannel < DeviceMemoryMap.MatrixSizeX) {
              sprintf(printBuff, "%sX%i,", printBuff, xChannel);
            }
          }
        }
      }
    }
    File->write(printBuff, strlen(printBuff));
    ZeroMemory(printBuff, 1024);
    totalValues -= pageSize;
    if (totalValues <= 0) break;  // we are done here...
    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
  }
  File->flush();
  return TRUE;
}

BOOL ReadPageDeltasRefs(int PreviousPage, UINT PageRequested, UINT8 *PData)
{
  UINT16 addressT37;
  UINT8 t37Size, PageDownUp;
  UINT8 index;
  DWORD bytesRead;
  int deltaPageNum;
  int i;
  BOOL bDataRead = FALSE;

  if (!ReadMemoryMap()) return FALSE;

  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  addressT37 = AllObjects[index]->LSB | ((AllObjects[index]->MSB<<8) & 0xFF00);
  t37Size = AllObjects[index]->Size + 1;
  if (PreviousPage < 0) {
    bytesRead = t37Size;
    bDataRead = TRUE;
    if (!ReadFromDevice(addressT37, PData, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
    PreviousPage = (int)PData[1];
  }
  // Check the requested page...
  deltaPageNum = PreviousPage - PageRequested;
  if (bDataRead && deltaPageNum == 0) return TRUE;

  PageDownUp = T37_PAGE_UP;
  if (deltaPageNum > 0) {
    PageDownUp = T37_PAGE_DOWN;
  }
  deltaPageNum = abs(deltaPageNum);
  for (i = 0; i < deltaPageNum; ++i) {
    if(!SetDiagnosticField(PageDownUp)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return FALSE;
    }
    Sleep(50);
  }
  Sleep(50);
  if (!bDataRead) {
    bytesRead = t37Size;
    if (!ReadFromDevice(addressT37, PData, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return FALSE;
    }
  }
  return TRUE;
}

BOOL GetDeviceDebugParams(UINT8 *T37Size, UINT8 *XLines, UINT8 *YLines)
{
  UINT8 index;

  if (!ReadMemoryMap()) return FALSE;
  if (!GetObjectIndex(DEBUG_DIAGNOSTIC_T37, &index)) {
    // Cannot get T37 address...should never happen...
    DBGPRINT(_T("Cannot read object T37.\n"));
    return FALSE;
  }
  *T37Size = AllObjects[index]->Size + 1;
  *XLines = DeviceMemoryMap.MatrixSizeX;
  *YLines = DeviceMemoryMap.MatrixSizeY;
  return TRUE;
}

int _tprintflog(TCHAR *Buffer)
{
  int ret=0;
  if (LogMode || LogModeSvc) {
    WriteLogFile(Buffer);
  }
  if (!SilentMode) {
    ret = _tprintf(Buffer);
    fflush(stdout);
  }
  return(ret);
}

BOOL WriteLogFile(TCHAR *Buffer)
{
  char buffer[512];
  int numChar;
  numChar = WideCharToMultiByte(CP_UTF8, NULL, (WCHAR *)Buffer, -1, buffer, 512, NULL, NULL);
  if (numChar > 0) {
    LogFile.write(buffer, strlen(buffer));
    return TRUE;
  }
  return FALSE;
}

BOOL RegReadFromAddress(UINT16 Address, UINT16 NumBytes)
{
  UINT8 buffer[4096];
  UINT i;
  PrintBuffer[0] = 0;

  if (!ReadMemoryMap()) return FALSE;
  if (!ReadFromDevice(Address, buffer, (DWORD *)&NumBytes)) {
    return FALSE;
  }
  for (i = 0; i < NumBytes; ++i) {
    _stprintf(PrintBuffer, _T("%s %.2X"), PrintBuffer, buffer[i]);
  }  
  _stprintf(PrintBuffer, _T("%s\nROK\n"), PrintBuffer);
  _TPRINTF(PrintBuffer);
  return TRUE;
}

BOOL ShowActiveArea()
{
  UINT16 address;
  UINT8 objSize;
  UINT8 buffer[64];
  UINT16 NumBytes;
  UINT8 org_x, org_y, size_x, size_y, tgain=0;
  int xrange=0, yrange=0;
  bool INVERTX = false, INVERTY = false, SWITCHXY = false;

  if (!ReadMemoryMap()) {
    _stprintf(PrintBuffer, _T("ERROR: Cannot Read memory Map.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  if (GetObjectAddressAndSize((UINT8)100, &address, &objSize)) {  // Check T100 exist.
    NumBytes = 14;
    if (!ReadFromDevice(address+1, buffer, (DWORD *)&NumBytes)) {
      _stprintf(PrintBuffer, _T("ERROR: Cannot read T100 data from chip1\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    INVERTX = (buffer[0] & 0x80) > 0;
    INVERTY = (buffer[0] & 0x40) > 0;
    SWITCHXY = (buffer[0] & 0x20) > 0;
    org_x = buffer[7];
    size_x = buffer[8];
    xrange = 256 * buffer[13] + buffer[12];

    NumBytes = 10;
    if (!ReadFromDevice(address + 19, buffer, (DWORD *)&NumBytes)) {
      _stprintf(PrintBuffer, _T("ERROR: Cannot read T100 data from chip2\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    org_y = buffer[0];
    size_y = buffer[1];
    yrange = 256 * buffer[6] + buffer[5];
    tgain = buffer[9];

  }
  else if (GetObjectAddressAndSize((UINT8)9, &address, &objSize)) { // Check T9 exist.
    NumBytes = 9;
    if (!ReadFromDevice(address + 1, buffer, (DWORD *)&NumBytes)) {
      _stprintf(PrintBuffer, _T("ERROR: Cannot read T9 data from chip\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    org_x = buffer[0];
    org_y = buffer[1];
    size_x = buffer[2];
    size_y = buffer[3];
    tgain = buffer[5];
    INVERTX = (buffer[8] & 0x2) > 0;
    INVERTY = (buffer[8] & 0x4) > 0;
    SWITCHXY = (buffer[8] & 0x01) > 0;

    NumBytes = 4;
    if (!ReadFromDevice(address + 18, buffer, (DWORD *)&NumBytes)) {
      _stprintf(PrintBuffer, _T("ERROR: Cannot read T9 data from chip\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    xrange = 256 * buffer[1] + buffer[0];
    yrange = 256 * buffer[3] + buffer[2];
  }
  else {
    _stprintf(PrintBuffer, _T("ERROR: Cannot find active area.\n"));
    _TPRINTF(PrintBuffer);
    return FALSE;
  }

  _stprintf(PrintBuffer, _T("\nStart_X=%d "), org_x);
  _TPRINTF(PrintBuffer);
  _stprintf(PrintBuffer, _T("Start_Y=%d "), org_y);
  _TPRINTF(PrintBuffer);
  _stprintf(PrintBuffer, _T("Size_X=%d "), size_x);
  _TPRINTF(PrintBuffer);
  _stprintf(PrintBuffer, _T("Size_Y=%d "), size_y);
  _TPRINTF(PrintBuffer);
  _stprintf(PrintBuffer, _T("Touch_Gain=%d "), tgain);
  _TPRINTF(PrintBuffer);
  _stprintf(PrintBuffer, _T("XRange=%d "), xrange);
  _TPRINTF(PrintBuffer);
  _stprintf(PrintBuffer, _T("YRange=%d "), yrange);
  _TPRINTF(PrintBuffer);
  if (INVERTX)
    _TPRINTF(_T("INVERTX "));
  if (INVERTY)
    _TPRINTF(_T("INVERTY "));
  if (SWITCHXY)
    _TPRINTF(_T("SWITCHXY "));
  _TPRINTF(_T("\n"));


  if (GetObjectAddressAndSize((UINT8)15, &address, &objSize)) { // Check T15 exist.
    NumBytes = 7;
    if (!ReadFromDevice(address, buffer, (DWORD *)&NumBytes)) {
      _stprintf(PrintBuffer, _T("ERROR: Cannot read T15 data from chip\n"));
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    else {
      if ((buffer[0] & 0x01) == 0x01) {
        _stprintf(PrintBuffer, _T("Key_X=%d "), buffer[1]);
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("Key_Y=%d "), buffer[2]);
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("Ker_XSize=%d "), buffer[3]);
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("Key_YSize=%d "), buffer[4]);
        _TPRINTF(PrintBuffer);
        _stprintf(PrintBuffer, _T("Key_Gain=%d\n"), buffer[6]);
        _TPRINTF(PrintBuffer);
      }
      else {
        _stprintf(PrintBuffer, _T("Key_X=-1 Key_Y=-1 Ker_XSize=-1 Key_YSize=-1 Key_Gain=0"));
        _TPRINTF(PrintBuffer);
      }
    }
  }

  _TPRINTF(_T("\n"));
  _TPRINTF(_T("Done.\n"));
  _TPRINTF(_T("\n"));

  return TRUE;
}

BOOL ShowMessage(int typeMsg, int tm)
{
  //UINT reportId = 0;
  unsigned int start;

  ShowObjMsg = (UINT8)(typeMsg&0xFF);

  BYTE tid;
  if ((tid = GetReportIdFromType(6)) > 0) {
    _stprintf(PrintBuffer, _T("Message T6: %02X\n"), tid);
    _TPRINTF(PrintBuffer);
  }
  if ((tid = GetReportIdFromType(25)) > 0) {
    _stprintf(PrintBuffer, _T("Message T25: %02X\n"), tid);
    _TPRINTF(PrintBuffer);
  }
  if ((tid = GetReportIdFromType(9)) > 0) {
    _stprintf(PrintBuffer, _T("Message T9: %02X\n"), tid);
    _TPRINTF(PrintBuffer);
  }
  if ((tid = GetReportIdFromType(100)) > 0) {
    _stprintf(PrintBuffer, _T("Message T100: %02X\n"), tid);
    _TPRINTF(PrintBuffer);
  }

  _TPRINTF(_T("\nStart message report\n"));

  MessageMode();

//  thread mth(Read_TouchEvent);

  start = clock();
  for (;;) {
    if (((clock() - start) / 1000) > (unsigned int)tm)
      break;
  }

//  mth.detach();

  //_stprintf(PrintBuffer, _T("Finish\n"));
  //_TPRINTF(PrintBuffer);

  _TPRINTF(_T("\nStop message report\n"));

  return TRUE;
}

BOOL MessageMode()
{
  //if (ConnectFWType != I2CBRIDGE)
  //  return FALSE;

  // Add by Miracle. Looks need this command to inform chip report correct message.
  BOOL ret;
  UINT8 bufw[65];
  UINT8 bufr[65];
  UINT8 idx = 0;

  if (!GetObjectAddressAndSize(5, &MsgProcAddress, &MsgProcSize))
    return FALSE;

  if (Protocol_HID) {

    // set message mode for HIDI2C.
    UINT8 writeBuffer[128];
    ZeroMemory(writeBuffer, 65);
    writeBuffer[0] = HIDHEADER[HIDDSC_OutputRegister_LSB];
    writeBuffer[1] = HIDHEADER[HIDDSC_OutputRegister_MSB];
    writeBuffer[2] = 0x04;
    writeBuffer[3] = 0x00;
    writeBuffer[4] = 0x06;
    writeBuffer[5] = 0x88;
    if (!I2CBridge_I2C_ReadWrite(6, writeBuffer, 0, NULL)) return FALSE;

    // repeat mode.
    bufw[idx++] = 0x88;
    bufw[idx++] = 0x58;
    bufw[idx++] = 0x00;
    bufw[idx++] = 0x14;
    bufw[idx++] = 0x00;

    ret = I2CBridge_ReadWrite_Physical(idx, bufw, 60, bufr);
  }
  else {

    bufw[idx++] = 0x88;
    bufw[idx++] = 0x58;
    bufw[idx++] = 0x02;
    bufw[idx++] = MsgProcSize;
    bufw[idx++] = (UINT8)(MsgProcAddress & 0xFF);
    bufw[idx++] = (UINT8)((MsgProcAddress>>8) & 0xFF);

    ret = I2CBridge_ReadWrite_Physical(idx, bufw, 60, bufr);
  }

  return TRUE;
}

void Read_TouchEvent()
{
  UINT8 buf_write[100], buf_read[100];
  UINT8 touchid = 0;
  BOOL isTouch[40];
  UINT8 touchIdx = 0;
  UINT8 touchStatus = 0;
  UINT8 reportid = 0;

  for (int i = 0; i < 40; i++) {
    isTouch[i] = FALSE;
  }

  touchid = GetReportIdFromType((UINT8)100);    // Touch event route to T100
  if (touchid == 0)
    touchid = GetReportIdFromType((UINT8)9);  // Touch event route to T9
  else
    touchid = touchid + 2;            // T100 touch report id start from offset 2.

  for (;;) {
    USBCMD(0, buf_write, 60, buf_read);

    UINT j = 0;
    if (Protocol_HID) {
      // Object events.
      //  0  1  2  3  4  5  6  7  8  9 10
      // 00 9A 00 14 00 06 FA 00 28 10 02 00 02 02
      // 00 9A 00 14 00 06 FA 00 28 10 02 00 02 02
      if (buf_read[5] == 0x06 && buf_read[6] == 0xFA) {
        reportid = buf_read[8];
        if (ShowObjMsg < 0xFF && ReportIds[reportid] != ShowObjMsg)
          continue;
        for (int i = 0; i < MsgProcSize; ++i) {
          _stprintf(&PrintBuffer[j], _T("%.2X "), buf_read[i+8]);
          j += 3;
        }
      }
      else {
        // Finger touch...
        //  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15                          0  1  2  3  4  5  6
        // 00 9A 00 14 00 01 01 01 46 0f 00 00 98 03 00 00 00 00 00 00 3d 99 00 -> 2d 91 46 0f 98 03 00 00 00
        // 00 9A 00 14 00 01 01 01 46 0f 00 00 98 03 00 00 00 00 00 00 81 99 02 -> 2d 91 46 0f 98 03 00 00 00
        // 00 9A 00 14 00 01 01 00 54 0d 00 00 87 02 00 00 00 00 00 00 81 99 00 -> 2c 91 54 0d 87 02 00 00 00
        // 00 9A 00 14 00 01 01 00 e9 0e 00 00 31 02 00 00 00 00 00 00 00 00 01 -> 2c 94 e9 0e 31 02 00 00 00
        // 00 9A 00 14 00 01 01 00 e9 0e 00 00 31 02 00 00 00 00 00 00 45 00 01 -> 2c 91 e9 0e 31 02 00 00 00
        // 00 9A 00 14 00 01 00 00 e9 0e 00 00 14 02 00 00 00 00 00 00 57 03 01 -> 2c 15 e9 0e 14 02 00 00 00
        // [0] T9/T100 report id + 
        // [1] first touch 94->moving 91->release 15
        if (ShowObjMsg < 0xFF && ShowObjMsg!=100 && ShowObjMsg!=9)
          continue;

        touchIdx = buf_read[7];
        if (touchIdx >= 40) continue; // strange touch index. ignore this package.

        if (buf_read[6] == 0x00) {
          touchStatus = 0x15;
          isTouch[touchIdx] = FALSE;
        }
        else if (buf_read[6] == 0x01) {
          if (isTouch[touchIdx])
            touchStatus = 0x91;
          else
            touchStatus = 0x94;
          isTouch[touchIdx] = TRUE;
        }
        else
          continue; // Unknown release status. skip this message.

        _stprintf(&PrintBuffer[0], _T("%.2X "), buf_read[7] + touchid);
        _stprintf(&PrintBuffer[3], _T("%.2X "), touchStatus);
        _stprintf(&PrintBuffer[6], _T("%.2X "), buf_read[8]);
        _stprintf(&PrintBuffer[9], _T("%.2X "), buf_read[9]);
        _stprintf(&PrintBuffer[12], _T("%.2X "), buf_read[12]);
        _stprintf(&PrintBuffer[15], _T("%.2X "), buf_read[13]);
        j = 18;
        //for (int i = 0; i < HIDHEADER[HIDDSC_InputLength_LSB]; ++i) {
        //  _stprintf(&PrintBuffer[j], _T("%.2X "), buf_read[i]);
        //  j += 3;
        //}

      }
    }
    else {
      // Object event. For OBP, touch event come from T9/T100 object.
      // 00 9A 00 31 20 04 00 10 00 03 06 FF 00 00
      // 00 9A 00 31 20 04 00 10 00 02 06 FF 00 00
      reportid = buf_read[3];
      if (ShowObjMsg < 0xFF && ReportIds[reportid] != ShowObjMsg)
        continue;

      for (int i = 0; i < MsgProcSize; ++i) {
        _stprintf(&PrintBuffer[j], _T("%.2X "), buf_read[i+3]);
        j += 3;
      }
    }
    _stprintf(&PrintBuffer[j], _T("%c"), '\n');
    _TPRINTF(PrintBuffer);
  }

}

// Miracle. 2014/05/23. After write object address and read data back. If object message return in the same time, the message data will be consider as object data. This function prevent this problem.
BOOL BridgeBoard_HIDI2C_ReadData(UINT8 *Msg)
{
  DWORD bytesRead;
  //UINT8 writeBuffer[128];
  UINT8 readBuffer[128];
  BOOL ret;
  int i;

  // Added by Miracle.
  if (ConnectFWType == I2CBRIDGE && Protocol_HID) {
    //ZeroMemory(writeBuffer, 65);
    for (i = 0; i < 40; i++) {
      ret = I2CBridge_I2C_ReadWrite(0, NULL, HIDHEADER[HIDDSC_OutputLength_LSB], readBuffer);

      if (!ret) {
        _stprintf(PrintBuffer, _T("ReadMessage: I2CBridge_I2C_ReadWrite error. \n"));
        _TPRINTF(PrintBuffer);
        return FALSE;
      }
      if (readBuffer[2] == 0x06 && readBuffer[3] == 0x00) // Miracle. 2014/05/23. This is
      {
        for (i = 0; i < HIDHEADER[HIDDSC_OutputLength_LSB]; i++) {
          Msg[i] = readBuffer[5 + i];
        }
        MsgProcSize = 8;
        return TRUE;
      }
    }
  }

  // Check if not initialized yet...
  if (!MsgProcAddress) {
    GetObjectAddressAndSize(5, &MsgProcAddress, &MsgProcSize);
  }

  bytesRead = MsgProcSize;
  if (!ReadFromDevice(MsgProcAddress, Msg, &bytesRead)) {
    _stprintf(PrintBuffer, _T("ReadMessage: ReadFromDevice error. \n"));
    _TPRINTF(PrintBuffer);
    Sleep(10);    // Add by Miracle. Prevent read too fast from chip and doesn't get data.
    return FALSE;
  }
  // Check if any valid message was read...
  if (Msg[0] == 0xFF) {
    //_stprintf(PrintBuffer, _T("Read 0xFF\n"));
    //_TPRINTF(PrintBuffer);
    Sleep(50/*100*/);   // Add by Miracle. Prevent read too fast from chip and doesn't get data.
    return FALSE;
  }
  return TRUE;
}


BOOL ReadMessage(UINT8 *Msg)
{
  DWORD bytesRead;
  //UINT8 writeBuffer[128];
  UINT8 readBuffer[128];
  BOOL ret;


  // Added by Miracle.
  if(ConnectFWType == I2CBRIDGE && Protocol_HID) {
    //ZeroMemory(writeBuffer, 65);
    ret = I2CBridge_I2C_ReadWrite(0, NULL, HIDHEADER[HIDDSC_OutputLength_LSB], readBuffer);

    if(! ret) {
      //_stprintf(PrintBuffer, _T("ReadMessage: I2CBridge_I2C_ReadWrite error. \n"));
      //_TPRINTF(PrintBuffer);
      return FALSE;
    }
    for(int i=0; i<HIDHEADER[HIDDSC_OutputLength_LSB]; i++) {
      Msg[i] = readBuffer[5+i];
    }
    MsgProcSize = 8;
    return TRUE;
  }

  // Check if not initialized yet...
  if (!MsgProcAddress) {
    GetObjectAddressAndSize(5, &MsgProcAddress, &MsgProcSize);
  }

  bytesRead = MsgProcSize;
  if (!ReadFromDevice(MsgProcAddress, Msg, &bytesRead)) {
    //_stprintf(PrintBuffer, _T("ReadMessage: ReadFromDevice error. \n"));
//  _stprintf(PrintBuffer, _T("."));  // Miracle. 2014/06/16. For debugging purpose.
//  _TPRINTF(PrintBuffer);
    Sleep(10);    // Add by Miracle. Prevent read too fast from chip and doesn't get data.
    return FALSE;
  }
  // Check if any valid message was read...
  if (Msg[0] == 0xFF) {
    //_stprintf(PrintBuffer, _T("Read 0xFF\n"));
    //_TPRINTF(PrintBuffer);
    Sleep(50/*100*/);   // Add by Miracle. Prevent read too fast from chip and doesn't get data.
    return FALSE;
  }
  return TRUE;
}

int ReadDeviceUniqueID()
{
  UINT16 address;
  UINT8 size;
  UINT8 buffer[130];
  DWORD bytesRead;
  UINT j=0;
  UINT8 pageUpDown = 0;
  UINT8 currentPage;
  int i, numPages;

  if (UniqueIDPage == -1) return 1; // Unique ID for this device is not supported

  if (!ReadMemoryMap()) return -1;
  // get T37 address...
  if (!GetObjectAddressAndSize(DEBUG_DIAGNOSTIC_T37, &address, &size)) {
    // Cannot get T37 address...
    return -1;
  }
  if(!SetDiagnosticField(T37_DEVID_MODE)) {
    DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
    return -1;
  }
  // read to get current page
  bytesRead = 40;
  if (!ReadFromDevice(address, buffer, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T37.\n"));
    return -1;
  }
  pageUpDown = T37_PAGE_UP;
  numPages = UniqueIDPage - buffer[1];
  if (numPages < 0) {
    pageUpDown = T37_PAGE_DOWN;
    numPages = abs(numPages);
  }
  for (i = 0; i < numPages; ++i) {
    if(!SetDiagnosticField(pageUpDown)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return -1;
    }
    Sleep(200);
  }
  bytesRead = 130;
  if (!ReadFromDevice(address, buffer, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T37.\n"));
    return -1;
  }
  currentPage = buffer[1];
  _stprintf(PrintBuffer, _T("%s Device Unique ID:\n"), DEVICE_NAME[DeviceConnected]);
  _TPRINTF(PrintBuffer);
  j = 0;
  for (i = 0; i < (int)UniqueIDSize; ++i) {
    _stprintf(&PrintBuffer[j], _T("%0.2X "), buffer[UniqueIDStart+2+i]);
    j += 3;
  }
  _stprintf(&PrintBuffer[j], _T("\n"));
  _TPRINTF(PrintBuffer);
  if (DeviceConnected == eMXT3432S) {
    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return -1;
    }
    bytesRead = 130;
    if (!ReadFromDevice(address, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return -1;
    }
    currentPage = buffer[1];
    j = 0;
    for (i = 0; i < (int)UniqueIDSize; ++i) {
      _stprintf(&PrintBuffer[j], _T("%0.2X "), buffer[UniqueIDStart+2+i]);
      j += 3;
    }
    _stprintf(&PrintBuffer[j], _T("\n"));
    _TPRINTF(PrintBuffer);
  }
  if (UniqueIDRevIDPage == -1) return 0;

  pageUpDown = T37_PAGE_UP;
  numPages = UniqueIDRevIDPage - currentPage;
  if (numPages < 0) {
    pageUpDown = T37_PAGE_DOWN;
    numPages = abs(numPages);
  }
  for (i = 0; i < numPages; ++i) {
    if(!SetDiagnosticField(pageUpDown)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return -1;
    }
    Sleep(200);
  }
  bytesRead = 130;
  if (!ReadFromDevice(address, buffer, &bytesRead)) {
    DBGPRINT(_T("Cannot read object T37.\n"));
    return -1;
  }
  currentPage = buffer[1];

  _stprintf(PrintBuffer, _T("Revision ID:\n%0.2X \n"), buffer[UniqueIDRevIDStart+2]);
  _TPRINTF(PrintBuffer);

  if (DeviceConnected == eMXT3432S) {
    if(!SetDiagnosticField(T37_PAGE_UP)) {
      DBGPRINT(_T("Failed to write to device. Error: %d\n"), GetLastError());
      return -1;
    }
    bytesRead = 130;
    if (!ReadFromDevice(address, buffer, &bytesRead)) {
      DBGPRINT(_T("Cannot read object T37.\n"));
      return -1;
    }
    _stprintf(PrintBuffer, _T("%0.2X \n"), buffer[UniqueIDRevIDStart+2]);
    _TPRINTF(PrintBuffer);
  }
  return 0;
}

BOOL FW2952TWorkaround()
{
  BOOL bRetCode = TRUE;

  if ((DeviceConnected == eMXT2952T) || (DeviceConnected == eMXT1664T)) {
    Sleep(1000);
    CleanSavedDeviceInfo(); // in case we are saving old info
    for (int i = 0; i < 3; i++) {
      Sleep(2000);
      if (IdentifyConnectFWType() >= 0) {
        break;
      }
      CleanSavedDeviceInfo(); // in case we are saving old info
    }

    if (!ActivateT7ForThqa()) {
      //      _TPRINTF(_T("Error on ActivateT7ForThqa()\n"));
      return FALSE;
    }
    if (!WriteThqaString()) {
      return FALSE;
    }
  }
  return bRetCode;
}

BOOL ActivateT7ForThqa()
{
  UINT16 address;
  UINT16 buffer= 0xA0A;
  DWORD bytesToWrite = 2;

  if (GetObjectAddressAndSize(GEN_POWERCONFIG_T7, &address)) {
    if (WriteToDevice(address, (UINT8 *)&buffer, &bytesToWrite)) {
      Sleep(1000);
      return TRUE;
    }
  }
  return FALSE;
}

UINT DisableDevice(TCHAR* DeviceName, BOOL Disable)
{
  HDEVINFO hDevInfo;
  SP_DEVINFO_DATA devData;
  UINT exitReason = 0; // SUCCESS
  DWORD dwSize, dwPropertyRegDataType;
  TCHAR szHardwareIDs[4096];
  SP_PROPCHANGE_PARAMS propertyChangeParams;
  DWORD error = 0;

  hDevInfo = SetupDiGetClassDevsW(&GUID_DEVCLASS_HIDCLASS, DeviceName, NULL, DIGCF_PRESENT);

  // Get first device matching device criterion.
  for (UINT i = 0; ; i++)
  {
    devData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiEnumDeviceInfo(hDevInfo, i, &devData)) {
      exitReason = 1;   // Failed enumeration
      break;
    }
    if (SetupDiGetDeviceRegistryProperty (hDevInfo, &devData, SPDRP_HARDWAREID,
      &dwPropertyRegDataType, (BYTE*)szHardwareIDs,
      sizeof(szHardwareIDs),    // The size, in bytes
      &dwSize)) {

      if (FindString(szHardwareIDs, DeviceName, dwSize)) {
        break;
      }
    }
    // if no items match filter, exit
    if (GetLastError() == ERROR_NO_MORE_ITEMS) {
      exitReason = 2; // Device not found
      break;
    }
  }

  if (!exitReason) {
    propertyChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    propertyChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    propertyChangeParams.StateChange = Disable ? DICS_DISABLE : DICS_ENABLE;
    propertyChangeParams.Scope = DICS_FLAG_GLOBAL;
    propertyChangeParams.HwProfile = 0;

    if (SetupDiSetClassInstallParams(hDevInfo, &devData, (PSP_CLASSINSTALL_HEADER)&propertyChangeParams, sizeof(SP_PROPCHANGE_PARAMS))) {
      if (SetupDiChangeState(hDevInfo, &devData)) {
      } else {
        error = GetLastError();
        if (error == 5) {
          exitReason = 3;   // insufficient rights (not admin)
        } else {
          exitReason = (UINT) error;
        }
      }
    }
  }
  
  SetupDiDestroyDeviceInfoList(hDevInfo);

  return exitReason;
}

BOOL FindString(const TCHAR *s, const TCHAR *t, int size)
{
  int i, j, k;
  for (i = 0; i < size; i++) 
  {
    for (j=i, k=0; t[k]!='\0' && s[j]==t[k]; j++, k++);
    if (k > 0 && t[k] == '\0')
      return TRUE;
  }
  return FALSE;
}

#ifndef AUTO_UPDATER

// returns pointer to read encoded XML if success (either external or internal)
// if internal (DevicesInfo.bin not present), sets XMLFilePresent to FALSE if encoded XML file is not found in the same directory
// returns NULL if it fails
// since it allocates the memory, it is the caller's responsibility to free memory when done
string *ReadDevicesDefinitionFile(const TCHAR *DevicesInfo)
{
  UINT size, i, j;
  string encoded;
  string *decoded = new (string);

  ifstream file(DevicesInfo, ios::in | ios::binary);

  XMLFilePresent = TRUE;

  if(!file) {
    // file is not present: let's read the embedded
    XMLFilePresent = FALSE;
    // this is already UTF8
    encoded = string((char *)XMLFileRes);
  } else {
    file.seekg(0,ios::end);
    size=(unsigned)file.tellg();
    file.seekg(ios::beg);
    file.clear();
    string encodedUTF16 (size, 0);
    // check if memory allocation was ok, else return
    file.read(&encodedUTF16[0], size);
    file.close();

    encoded.resize(size/2);
    j = 0;
    for (i = 0; i < size; i += 2) {
      encoded[j++] = encodedUTF16[i];
    }
  }
// This adds chars at the end - hence it does not work properly
//  numChar = WideCharToMultiByte(CP_UTF8, NULL, (WCHAR *)encFileUni, -1, encFile, size, NULL, NULL);

  if (!Decrypt(&encoded, decoded)) {
    if (XMLFilePresent) { // do this when external file has problems...
      // file is present but has issues: let's read the embedded data instead
      XMLFilePresent = FALSE;
      // this is already UTF8
      encoded = string((char *)XMLFileRes);
      if (!Decrypt(&encoded, decoded)) return NULL;
    }
  }

  return decoded;
}

void DecodeXml(UINT v[2])
{
  UINT k[4]={0x41363135,0x31314241,0x34324146,0x46344431}; // encryption key used in MaxTouchStudio

  UINT y = v[0];
  UINT z = v[1];
  UINT sum = 0xC6EF3720;
  UINT delta = 0x9E3779B9;
  UINT n = 32;

  while (n-- > 0)
  {
    z -= (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
    sum -= delta;
    y -= (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
  }

  v[0] = y;
  v[1] = z;
}

/// <summary>
/// decrypt a string
/// </summary>
/// <param name="ciphertext">the string to be decrypted</param>
/// <param name="key">the decryption key</param>
/// <param name="plaintext">the decrypted string</param>
/// <param name="checksum">the checksum info embedded in the encrypted string</param>
/// <returns>flag: decryption succeeded</returns>
BOOL Decrypt(string *ciphertext, string *plaintext)
{
  byte checksum[16];
  byte *dataPlusCrcBytes;
  byte *dataBytes;
  UINT tempData[2], i;
  UINT size;

  // default return value
  BOOL rtnval = FALSE;

  // The input ciphertext is expected to be a multiple of 16 characters (i.e., 8 bytes) long.
  // If it's not, return failure.
  size = (UINT)ciphertext->length();
  if (size % 16 == 0)
  {
    // get the byte contents of the input string
    int x = 0;
    dataPlusCrcBytes = new (std::nothrow)byte[size / 8];
    if (!dataPlusCrcBytes) return rtnval;

    for (i = 0; i < size; i += 16)
    {
      tempData[0] = strtoul((ciphertext->substr(i, 8)).c_str(), NULL, 16);
      tempData[1] = strtoul((ciphertext->substr(i + 8, 8)).c_str(), NULL, 16);
      DecodeXml(tempData);
      dataPlusCrcBytes[x++] = (byte)tempData[0];
      dataPlusCrcBytes[x++] = (byte)tempData[1];
    }

    // extract the MD5 checksum from the end of the decrypted data
    memcpy(checksum, &dataPlusCrcBytes[x-16], 16);

    // get the data portion of the decrypted data
    dataBytes = new (std::nothrow)byte[x - 16];
    if (!dataBytes) {
      delete[] dataPlusCrcBytes;
      return rtnval;
    }
    memcpy(dataBytes, dataPlusCrcBytes, (x-16));

    *plaintext = string((char *)dataBytes);
    delete[] dataPlusCrcBytes;
    delete[] dataBytes;

    rtnval = TRUE;
  }
  return rtnval;
}

// returns 0 if success
// sets XMLFilePresent to FALSE if XML file is not found in the same directory
// returns 1 if name is not found in the file
// returns 2 generic error
// returns 3 error loading XML file
INT GetDeviceAppName(UINT FamilyId, UINT VariantId, string &Name)
{
  pugi::xml_document doc;
  pugi::xml_node family, node, subnode;
  pugi::xml_parse_result result;
  UINT familyId, bootLoaderId;
  string pId, *strBuffer;
  char *pch, temp1[16], *next_token = NULL;;
  UINT i, j, value[16];
  char *contents = NULL;

  strBuffer = ReadDevicesDefinitionFile(DevicesDefinition);
  if (strBuffer == NULL || !strBuffer->size()) {
      return 2;  // error
  }
  contents = new char[strBuffer->size()];
  if (!contents) {
    delete strBuffer;
    return 2;
  }
  memcpy(contents, strBuffer->c_str(), strBuffer->size());
  result = doc.load_buffer_inplace(contents, strBuffer->size());
  delete strBuffer;
  if (!result) {
    if (contents) delete[] contents;
    return 3;  // error 
  }

  family = doc.child("Devices");

  for (node = family.first_child(); node; node = node.next_sibling()) {
    familyId = node.attribute("Id").as_uint();
    if (familyId == FamilyId) {
      for (subnode = node.first_child(); subnode; subnode = subnode.next_sibling()) {        
        pId = subnode.attribute("Id").as_string();  // in case we have multiple Ids
        bootLoaderId = subnode.attribute("BootLoaderId").as_uint();
        Name = subnode.attribute("Name").as_string();

        // check if multiple Ids, like: <Variant Id="23,25,35" BootLoaderId="40" Name="mXT336S" Settings="mXT336S"/>
        i = 0;
        strcpy_s(temp1, pId.c_str());
        pch = strtok_s(temp1, ",", &next_token);
        sscanf(pch, "%d", &value[i]);
        for (i = 1;;++i) {
          pch = strtok_s(NULL, ",", &next_token);
          if (pch) {
            sscanf(pch, "%d", &value[i]);
          } else {
            break;
          }
        }
        j = i; // number of values
        for (i = 0; i < j; ++i) {
          if (value[i] == VariantId) {
            if (contents) delete[] contents;
            return 0; // success
          }
        }
      }
      break; // not found
    }
  }
  if (contents) delete[] contents;
  return 1; // element not found: use hardcoded internal data
}

// returns 0 if success
// sets XMLFilePresent to FALSE if XML file is not found in the same directory
// returns 1 if name is not found in the file
// returns 2 if memory allocation error (rare): use internal hardcoded data
// returns 3 if error loading internal file (possible?): use internal hardcoded data
INT GetDeviceBtlName(UINT BootloaderId, string &Name)
{
  pugi::xml_document doc;
  pugi::xml_node family, node, subnode;
  pugi::xml_parse_result result;
  UINT bootLoaderId;
  string pId, *strBuffer;
  char *contents = NULL;

  strBuffer = ReadDevicesDefinitionFile(DevicesDefinition);
  if (strBuffer == NULL || !strBuffer->size()) {
      return 2;  // error
  }
  contents = new char[strBuffer->size()];
  if (!contents) {
    delete strBuffer;
    return 2;
  }
  memcpy(contents, strBuffer->c_str(), strBuffer->size());
  result = doc.load_buffer_inplace(contents, strBuffer->size());
  delete strBuffer;
  if (!result) {
    if (contents) delete[] contents;
    return 3;  // error 
  }

  family = doc.child("Devices");
  for (node = family.first_child(); node; node = node.next_sibling()) {
    for (subnode = node.first_child(); subnode; subnode = subnode.next_sibling()) {        
      bootLoaderId = subnode.attribute("BootLoaderId").as_uint();
      Name = subnode.attribute("Settings").as_string();
      if (bootLoaderId == BootloaderId) {
        if (contents) delete[] contents;
        return 0; // found!
      }
    }
  }
  if (contents) delete[] contents;
  return 1; // not found
}
#endif

//
// From here on, calls used for HIDI2C
//
int ReadWithRetryFromMxt(HANDLE File, UINT16 Address, UINT16 Numbytes, UINT8 *Buffer)
{
  int k, iRetCode;
  for (k = 0; k < 7; ++k) {
    iRetCode = ReadFromMxt(File, Address, Numbytes, Buffer);
    if (iRetCode > 0) {
      break;
    }
    DBGPRINT(_T("Error reading from Mxt chip. Retry #: %d\n"), k);
  }
  return iRetCode;
}

int WriteWithRetryToMxt(HANDLE File, UINT16 Address, UINT16 Numbytes, UINT8 *Buffer)
{
  int k, iRetCode;
  for (k = 0; k < 7; ++k) {
    iRetCode = WriteToMxt(File, Address, Numbytes, Buffer);
    if (iRetCode != -1) {
      break;
    }
    DBGPRINT(_T("Error writing to Mxt chip. Retry #: %d\n"), k);
  }
  return iRetCode;
}

BOOL InitI2CNormal(BOOL Quiet)
{
  if (DebugMode) Quiet = TRUE;
  // Check if we already initialized
  if (!InitI2CNormalDone) {
    //try to connect to SpbTestTool/I2C driver
    hI2CDriver = CreateFileW(MXTBRIDGE_USERMODE_PATH, (GENERIC_READ | GENERIC_WRITE), 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (hI2CDriver == INVALID_HANDLE_VALUE && !Quiet){
      _stprintf(PrintBuffer, _T("Error opening I2C MxtBridge.sys driver: 0x%x. Please verify that it is installed.\n"), GetLastError());
      _TPRINTF(PrintBuffer);
      return FALSE;
    }
    if (i2cBridgeInit(hI2CDriver) == -1) {
      if (!Quiet) {
        if (GetLastError() == ERROR_BUSY) {
          _stprintf(PrintBuffer, _T("Error on i2cBridgeInit(): unable to lock access to device.\nDid you disable Windows HidI2C driver from Device Manager?\n\n"));
          _TPRINTF(PrintBuffer);
        } else {
          _stprintf(PrintBuffer, _T("Error on i2cBridgeInit().\n"));
          _TPRINTF(PrintBuffer);
        }
      }
      return FALSE;
    }
    InitI2CNormalDone = TRUE;
  }
  if (ConnectFWType == HIDI2CNOINBOX) {
    if(!ReadHIDDescriptorWithRetry(hI2CDriver, 5)){
      if (!Quiet) {
        _stprintf(PrintBuffer, _T("Error on ReadHIDDescriptor().\n"));
        _TPRINTF(PrintBuffer);
      }
      return FALSE;
    }
  }
  return TRUE;
}

BOOL InitI2CBootloader(BOOL Quiet)
{
  // Added by Miracle.
  if (ConnectFWType == I2CBRIDGE) { 
    return TRUE;
  }

  // Check if we already initialized
  if (!InitI2CBootloaderDone) {
    //try to connect to SpbTestTool/I2C driver
    hI2CDriverBootloader = CreateFileW(MXTBRIDGEBOOT_USERMODE_PATH, (GENERIC_READ | GENERIC_WRITE), 0, nullptr, OPEN_EXISTING, 0, nullptr); 
    if (hI2CDriverBootloader == INVALID_HANDLE_VALUE){
      if (!Quiet) {
        _stprintf(PrintBuffer, _T("Error opening I2C MxtBootBridge.sys driver: 0x%x. Please verify that it is installed.\n"), GetLastError());
        _TPRINTF(PrintBuffer);
      }
      return FALSE;
    }
    if (i2cBridgeInit(hI2CDriverBootloader) == -1) {
      if (!Quiet) {
        _stprintf(PrintBuffer, _T("Error on i2cBridgeInit().\n"));
        _TPRINTF(PrintBuffer);
      }
      return FALSE;
    }
    InitI2CBootloaderDone = TRUE;
  }
  return TRUE;
}

BOOL ReadHIDDescriptorWithRetry(HANDLE file, int Retries)
{
  int k; 
  BOOL bRetCode = FALSE;
  for (k = 0; k < Retries; ++k) {
    bRetCode = ReadHIDDescriptor(file);
    if (bRetCode) {
      break;
    }
    DBGPRINT(_T("Error on ReadHIDDescriptor(). Retry #: %d\n"), k);
    Sleep(30*k);
  }
  return bRetCode;
}

#ifdef LENOVO_COLTRANE
BOOL Is1664()
{
  return (DeviceConnected == eMXT1664S || DeviceConnected == eMXT1664T);
}
#endif