#pragma once

//must define this if use ntddscsi.h in usermode app.
#define _NTSCSI_USER_MODE_

#include <tchar.h>
#include <windows.h>
#include <ntddscsi.h>
#include <ntddstor.h>
#include <scsi.h>
#include <Setupapi.h>
#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;
typedef basic_string<TCHAR> tstring;

#include "Utils.h"
#include "Opal_Constant_and_Enum.h"
#include "Opal_Structures.h"
#include "Opal_CmdPacket.h"
#include "Opal_Device.h"
#include "Opal_OS.h"
#include "Opal_Factory.h"

#define BIG_BUFFER_SIZE         2048
#define SMALL_BUFFER_SIZE       256
#define TINY_BUFFER_SIZE        128

//physical drive name example:  "\\.\PhysicalDrive2"
//#define PHYSICAL_DISK_FORMAT    L"\\\\.\\PhysicalDrive%d"


