// QueryDeviceBDF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <SetupAPI.h>
#include <winioctl.h>
#include <initguid.h>
#include <devpkey.h>
#include <list>
#include <regex>
#include <string>
#include <iostream>

typedef std::basic_string<TCHAR> tstring;
using namespace std;

#pragma comment(lib, "setupapi.lib")

enum ID_TYPE
{
    HWID = 0,
    DEV_INTERFACE = 1,
};

typedef struct _CMD_OPTION
{
    tstring Id = _T("");
    ID_TYPE Type = HWID;
}CMD_OPTION, * PCMD_OPTION;

static void ParseBDF(wchar_t* src, uint32_t& bus, uint32_t& dev, uint32_t& func)
{
    //wregex test(L"^PCI[^[0-9]]*(\\d+).*(\\d+).*(\\d+)$");
    wregex test(L"^PCI[^0-9]*(\\d+)[^0-9]*(\\d+)[^0-9]*(\\d+)$");
    wsmatch match;
    wstring src_str = src;
    if (regex_search(src_str, match, test))
    {
        bus = _wtol(match[1].str().c_str());
        dev = _wtol(match[2].str().c_str());
        func = _wtol(match[3].str().c_str());
    }
}


BOOL ParseCmd(CMD_OPTION& option, int argc, TCHAR* argv[])
{
    if (argc < 3)
        return FALSE;

    option.Id = argv[2];

    if (0 == _tcsnicmp(argv[1], _T("hwid"), 4))
        option.Type = HWID;
    else if (0 == _tcsnicmp(argv[1], _T("devinterface"), 12))
        option.Type = DEV_INTERFACE;
    else
        return FALSE;
    return TRUE;
}
void Usage()
{
}

int _tmain(int argc, TCHAR* argv[])
{
    CMD_OPTION option;
    if(!ParseCmd(option, argc, argv))
    {
        Usage();
        return -1;
    }

    return 0;
}

