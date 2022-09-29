#pragma once
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

typedef struct _DISK_INFO {
    tstring DevPath;
    tstring PhyDiskName;
    tstring CtrlDevPath;
}DISK_INFO, * PDISK_INFO;

typedef struct _MOUNTED_VOLUME_INFO {
    tstring VolumeName;
    list<DISK_INFO> BelongDisks;
}MOUNTED_VOLUME_INFO, * PMOUNTED_VOLUME_INFO;


size_t EnumPhysicalDisks(list<tstring>& devpath_list);
//size_t EnumVolumesByDriveLetter(list<VOLUME_INFO>& volume_list);
size_t EnumVolumes(list<VOLUME_INFO>& volume_list);
BOOL IsVolumeReady(tstring vol_name);
BOOL GetPartitionList(IN OUT BYTE* buffer, DWORD buf_size, tstring& devpath);
size_t EnumDiskInfo(list<DISK_INFO>& result);
size_t EnumPhysicalDisks(list<tstring>& devpath_list);