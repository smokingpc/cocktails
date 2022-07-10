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

using namespace std;
typedef std::basic_string<TCHAR> tstring;

#pragma comment(lib, "setupapi.lib")

#define BIG_BUFFER_SIZE     4096
#define SMALL_BUFFER_SIZE   256
#define TINY_BUFFER_SIZE    64
#define FORMAT_16_BYTES     _T("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X")
#define FORMAT_SYSTEMTIME   _T("%04d/%02d/%02d %02d:%02d:%02d.%03d")

enum ID_TYPE
{
    HWID = 0,
    DEV_IF = 1,
    NQN = 2,
};

typedef struct _CMD_OPTION
{
    tstring Id = _T("");
    ID_TYPE Type = HWID;
}CMD_OPTION, * PCMD_OPTION;

typedef union _BDF_INFO
{
    struct {
        BYTE Bus;
        BYTE Dev;
        BYTE Func;
    } DUMMYSTRUCTNAME;

    unsigned long AsUlong = 0;
}BDF_INFO, * PBDF_INFO;

typedef struct _DEV_INFO
{
    tstring Hwid = _T("");

    BDF_INFO BDF = { 0 };
    //parent id is always "device instance id"
    tstring ParentId = _T("");
    BDF_INFO ParentBDF = { 0 };
}DEV_INFO, * PDEV_INFO;

#include "NVMe.h"
#include "Utils.h"
