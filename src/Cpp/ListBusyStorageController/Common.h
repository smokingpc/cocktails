#pragma once

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <SetupAPI.h>
#include <winioctl.h>
#include <initguid.h>
#include <devpkey.h>
#include <list>
#include <regex>
#include <algorithm>
#include <string>
#include <memory>

#include "Utils.h"

using namespace std;
typedef std::basic_string<TCHAR> tstring;

#pragma comment(lib, "setupapi.lib")

#define BIG_BUFFER_SIZE     1024
#define SMALL_BUFFER_SIZE   256
#define TINY_BUFFER_SIZE    64
#define PHYSICAL_DISK_FORMAT    _T("\\\\.\\PhysicalDrive%d")
#define DOSDRIVE_FORMAT         _T("\\\\.\\%s\\")

typedef struct _VOLUME_INFO {
    tstring VolumeName;
    list<tstring> MountPointList;
    list<tstring> PhyDisks;      //physical disk which contains this volume. A volume could span to multiple physical disks.
}VOLUME_INFO, * PVOLUME_INFO;

typedef struct _DRIVE_MOUNT_INFO {
    tstring DriveName;
    tstring MountVolume;
}DRIVE_MOUNT_INFO, * PDRIVE_MOUNT_INFO;

typedef struct _CONTROLLER_INFO {
    tstring DevPath;
    list<tstring> PhyDisks;      //physical disk belongs to this controller
}CONTROLLER_INFO, * PCONTROLLER_INFO;

typedef struct _PHYDISK_INFO {
    tstring ParentDevPath;            //CONTROLLER DevPath
    tstring ParentHwID;               //CONTROLLER HardwareId
    tstring PhyDisk;
}PHYDISK_INFO, * PPHYDISK_INFO;

