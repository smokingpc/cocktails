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
#include <map>

using namespace std;
typedef std::basic_string<TCHAR> tstring;

#include "Utils.h"
#pragma comment(lib, "setupapi.lib")

#define BIG_BUFFER_SIZE     1024
#define SMALL_BUFFER_SIZE   256
#define TINY_BUFFER_SIZE    64
#define PHYSICAL_DISK_FORMAT    _T("\\\\.\\PhysicalDrive%d")
#define DOSDRIVE_FORMAT         _T("\\\\.\\%s\\")               //this is DOS DEVICE NAME format in NT system.

typedef struct _VOLUME_INFO {
    tstring VolumeName;
    list<tstring> MountPointList;
    list<tstring> PhyDisks;      //physical disk which contains this volume. A volume could span to multiple physical disks.

    //check if the specified physical disk contains this volume?
    BOOL IsDiskHasThisVolume(tstring diskname)
    {
        for (auto& disk : PhyDisks)
        {
            if (true == StrCompare(diskname, disk))
                return TRUE;
        }
        return FALSE;
    }
    BOOL IsVolumeMounted()
    {
        return (MountPointList.size()>0)? TRUE : FALSE;
    }

#if 0
    BOOL IsDriveMountedByVolume(tstring drive_name)
    {
        tstring temp = drive_name;
        if (temp.back() == _T('\\'))
            temp.pop_back();

        HANDLE device = CreateFile(temp.c_str(), 0,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

        BOOL mounted = FALSE;
        if (device != INVALID_HANDLE_VALUE)
        {
            DWORD ret_size = 0;
            //this IOCTL returns FALSE if volume is locked and mount.
            BOOL ok = DeviceIoControl(device, FSCTL_IS_VOLUME_MOUNTED,
                NULL, 0, NULL, 0, &ret_size, NULL);
            DWORD error = GetLastError();
            mounted = !ok;
            CloseHandle(device);
        }

        return mounted;
    }
#endif
}VOLUME_INFO, * PVOLUME_INFO;

typedef struct _DRIVE_MOUNT_INFO {
    tstring DriveName;
    tstring VolumeName;
}DRIVE_MOUNT_INFO, *PDRIVE_MOUNT_INFO;

typedef struct _CONTROLLER_INFO {
    tstring DevPath;
    tstring InstanceId;
    bool IsBusy;
}CONTROLLER_INFO, * PCONTROLLER_INFO;

typedef struct _PHYDISK_INFO {
    tstring ParentDevPath;            //CONTROLLER DevPath
    tstring ParentInstanceID;               //CONTROLLER HardwareId
    tstring PhyDisk;
}PHYDISK_INFO, * PPHYDISK_INFO;

BOOL EnumControllers(OUT list<CONTROLLER_INFO>& busy_list, OUT list<CONTROLLER_INFO>& free_list,
                    OUT list<PHYDISK_INFO>& disk_list, IN list<VOLUME_INFO>& vol_list);
size_t EnumVolumes(OUT list<VOLUME_INFO>& result);

