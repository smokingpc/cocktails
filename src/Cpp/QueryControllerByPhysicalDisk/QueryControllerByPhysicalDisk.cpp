// QueryControllerByPhysicalDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

//found volume interface name in result example =>
//  "\\?\storage#volume#{2904e7e5-d0c4-11eb-8357-806e6f6e6963}#00000076fda00000#{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"
static size_t EnumVolumes(list<tstring> &result)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id = 0;
    const GUID* class_guid = &GUID_DEVINTERFACE_VOLUME;

    DWORD flag = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT;
    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE);

    if(INVALID_HANDLE_VALUE == infoset)
        return 0;

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    id = 0;
    while (SetupDiEnumDeviceInfo(infoset, id++, &infodata))
    {
        ZeroMemory(&ifdata, sizeof(SP_DEVICE_INTERFACE_DATA));
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, class_guid, 0, &ifdata))
            continue;

        if ((ifdata.Flags & SPINT_ACTIVE) == 0)
            continue;

        PSP_DEVICE_INTERFACE_DETAIL_DATA detail;
        DWORD need_size = 0;

        SetupDiGetDeviceInterfaceDetail(
            infoset,
            &ifdata,
            NULL,
            0,
            &need_size,
            NULL);

        detail = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new BYTE[need_size];
        if (NULL == detail)
            continue;

        ZeroMemory(detail, need_size);
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(
            infoset, &ifdata, detail, need_size,
            NULL, NULL))
            continue;

        result.push_back(detail->DevicePath);
        delete[] detail;
    }
    
    SetupDiDestroyDeviceInfoList(infoset);
    return result.size();
}

int _tmain(int argc, TCHAR* argv[])
{
    list<DISK_INFO> result;
    EnumDiskInfo(result);

    for(auto &info : result)
    {
        _tprintf(_T("found disk: %s\n"), info.PhyDiskName.c_str());
        _tprintf(_T("\tDevPath=%s\n"), info.DevPath.c_str());
        _tprintf(_T("\tParent Controller=%s\n"), info.CtrlDevPath.c_str());
    }

    return 0;
}

