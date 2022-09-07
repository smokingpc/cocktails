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

#define BIG_BUFFER_SIZE     4096
#define SMALL_BUFFER_SIZE   256
#define TINY_BUFFER_SIZE    64
#define PHYSICAL_DISK_FORMAT    _T("\\\\.\\PhysicalDrive%d")

size_t EnumPhysicalDisks(list<tstring>& devpath_list);
BOOL GetPartitionList(IN OUT BYTE* buffer, DWORD buf_size, tstring& devpath);

typedef struct _PARTITION_MOUNT_INFO
{
    tstring 
}PARTITION_MOUNT_INFO, *PPARTITION_MOUNT_INFO;
