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
    bool IsReady;
    tstring VolumeName;
    list<tstring> MountPointList;   //mount point of this volume
    list<tstring> PhyDisks;      //physical disk which contains this volume. A volume could span to multiple disks.
}VOLUME_INFO, * PVOLUME_INFO;

typedef struct _DISK_INFO {
    tstring DevPath;            //(Device Interface name) Device Path of this PhysicalDisk.
    tstring PhyDiskName;        
    tstring CtrlDevPath;        //(Device Interface name) Controller of this PhysicalDisk.
}DISK_INFO, * PDISK_INFO;

typedef struct _DISK_VOLUME_INFO {
    tstring DevPath;            //(Device Interface name) Device Path of this PhysicalDisk.
    tstring PhyDiskName;
    list<tstring> Volumes;      //list volume names(not device interface name of volume) which belong this disk
}DISK_VOLUME_INFO, * PDISK_VOLUME_INFO;

typedef struct _CONTROLLER_INFO {
    tstring DevPath;            //Device Interface Name of Controller (not PCI location)
    list<DISK_VOLUME_INFO> Disks;
}CONTROLLER_INFO, * PCONTROLLER_INFO;

typedef struct _DRIVE_MOUNT_INFO {
    tstring DriveName;      //e.g. "C:" , "D:"
    tstring MountVolume;
}DRIVE_MOUNT_INFO, * PDRIVE_MOUNT_INFO;

TCHAR tcsupper(TCHAR in);
size_t EnumPhysicalDisks(list<tstring>& devpath_list);
BOOL IsVolumeReady(tstring vol_name);
BOOL IsVolumeMounted(tstring vol_name);
BOOL GetPartitionList(IN OUT BYTE* buffer, DWORD buf_size, tstring& devpath);
size_t EnumDiskInfo(list<DISK_INFO>& result);
size_t EnumVolumeInfo(list<VOLUME_INFO>& result);
size_t EnumVolume(list<tstring> &result);
