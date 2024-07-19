// EnumDeviceByCompatibleID.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <algorithm>
#include <string>
#include <memory>

using namespace std;
typedef std::basic_string<TCHAR> tstring;

#pragma comment(lib, "setupapi.lib")

#define BIG_BUFFER_SIZE     1024
#define SMALL_BUFFER_SIZE   256
#define TINY_BUFFER_SIZE    64
#define FORMAT_16_BYTES     _T("%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X")
#define FORMAT_SYSTEMTIME   _T("%04d/%02d/%02d %02d:%02d:%02d.%03d")

__inline void ParsePropertyToString(BYTE* data, DEVPROPTYPE type, tstring& result)
{
    switch (type)
    {
    case DEVPROP_TYPE_BOOLEAN:
    {
        result = (data[0] > 0) ? _T("TRUE") : _T("FALSE");
    }
    break;

    case DEVPROP_TYPE_SBYTE:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((char*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_BYTE:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((unsigned char*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_INT16:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((short*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_UINT16:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((unsigned short*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_INT32:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((int*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_UINT32:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((unsigned int*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_INT64:
    case DEVPROP_TYPE_CURRENCY:     //64bit signed int
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%lld"), ((INT64*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_UINT64:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%lld"), ((UINT64*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_FLOAT:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%f"), ((float*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_DOUBLE:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%lf"), ((double*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_DECIMAL:      //128bit int
    case DEVPROP_TYPE_GUID:
    {
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, FORMAT_16_BYTES,
            ((TCHAR*)data)[0], ((TCHAR*)data)[1], ((TCHAR*)data)[2], ((TCHAR*)data)[3],
            ((TCHAR*)data)[4], ((TCHAR*)data)[5], ((TCHAR*)data)[6], ((TCHAR*)data)[7],
            ((TCHAR*)data)[8], ((TCHAR*)data)[9], ((TCHAR*)data)[10], ((TCHAR*)data)[11],
            ((TCHAR*)data)[12], ((TCHAR*)data)[13], ((TCHAR*)data)[14], ((TCHAR*)data)[15]);
        result = buf;
    }
    break;

    case DEVPROP_TYPE_DATE:     //DATE is OLE Automation date. 1899/12/30 0:0:0 ==> 0.00, 1900/01/01 0:0:0 is 2.00
    {
        //todo: implement this convertion
    }
    break;
    case DEVPROP_TYPE_FILETIME:
    {
        SYSTEMTIME systime = { 0 };
        FileTimeToSystemTime((FILETIME*)data, &systime);
        TCHAR buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, FORMAT_SYSTEMTIME,
            systime.wYear, systime.wMonth, systime.wDay,
            systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
        result = buf;
    }
    break;

    case DEVPROP_TYPE_STRING:
    {
        result = (TCHAR*)data;
    }
    break;

    case DEVPROP_TYPE_STRING_LIST:
    {
        TCHAR* ptr = (TCHAR*)data;
        while (ptr[0] != _T('\0'))
        {
            size_t len = _tcslen(ptr);
            if (0 == len)
                break;
            if (result.size() > 0)
                result += _T(",");
            result += ptr;
            ptr += (len + 1);
        }
    }
    break;
    }

}

__inline void GetDevicePropertry(HDEVINFO infoset, PSP_DEVINFO_DATA infodata,
    CONST DEVPROPKEY* key, DEVPROPTYPE type, tstring& result)
{
    BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
    BOOL ok = SetupDiGetDeviceProperty(infoset, infodata, key, &type, buffer, BIG_BUFFER_SIZE, NULL, 0);
    if (!ok)
    {
        result = _T("");
        return;
    }
    ParsePropertyToString(buffer, type, result);
}

inline void ParseEnumeratorByCCId(OUT tstring& enumerator, tstring& id)
{
    enumerator = _T("");
    size_t found = id.find(_T('\\'), 0);
    if (tstring::npos != found)
    {
        enumerator = id.substr(0, found);
    }
}

void EnumerateDevice(TCHAR *compatible_id)
{
    HDEVINFO infoset = NULL;
    SP_DEVINFO_DATA infodata = {0};
    SP_DEVICE_INTERFACE_DATA ifdata = {0};
    DWORD id = 0;
    DWORD flags = DIGCF_ALLCLASSES | DIGCF_PRESENT;
    tstring ccid = compatible_id;
    tstring enumerator = _T("");

    ParseEnumeratorByCCId(enumerator, ccid);

    infoset = SetupDiGetClassDevsEx(
        NULL,
        enumerator.c_str(),
        NULL,
        flags,
        NULL,
        NULL,
        NULL);

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    id = 0;

    while (SetupDiEnumDeviceInfo(infoset, id++, &infodata))
    {
        tstring cc_ids = _T("");
        tstring desc = _T("");
        tstring hwid = _T("");
        DEVPROPTYPE type = DEVPROP_TYPE_STRING;
        GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_CompatibleIds, type, cc_ids);
        size_t found = cc_ids.find(compatible_id, 0);
        if (tstring::npos != found)
        {
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_DeviceDesc, type, desc);
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_HardwareIds, type, hwid);
            _tprintf(_T("%s=>\n\t%s\n"), hwid.c_str(), desc.c_str());
        }
    }

    SetupDiDestroyDeviceInfoList(infoset);
}


void Usage()
{
    _tprintf(_T("Usage:\n"));
    _tprintf(_T("EnumDeviceByCompatibleID.exe [Compatible ID to search]\n\n"));
    _tprintf(_T("Example: to search PCI\\CC_010802, type\n"));
    _tprintf(_T("EnumDeviceByCompatibleID.exe \"PCI\\CC_010802\" \n\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    if(argc < 2)
    {
        Usage();
        return -1;
    }

    EnumerateDevice(argv[1]);
}
