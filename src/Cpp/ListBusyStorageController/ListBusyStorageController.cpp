// ListBusyStorageController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"


BOOL QueryDevInterfaceByHwId(tstring& devpath, tstring& hwid)
{
    HDEVINFO infoset;
    tstring enum_str = _T("");
    DWORD flag = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT;
    DEVPROPTYPE type = DEVPROP_TYPE_STRING;

    infoset = SetupDiGetClassDevs(&GUID_DEVINTERFACE_STORAGEPORT, NULL, NULL, flag);

    if (INVALID_HANDLE_VALUE != infoset && NULL != infoset)
    {
        DWORD id = 0;
        tstring found_devif = _T("");
        SP_DEVINFO_DATA infodata = { 0 };
        infodata.cbSize = sizeof(SP_DEVINFO_DATA);
        while (SetupDiEnumDeviceInfo(infoset, id, &infodata))
        {
            id++;
            DEVPROPTYPE type = 0;
            tstring found_hwid = _T("");

            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
            SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
            BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
            ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, &GUID_DEVINTERFACE_STORAGEPORT, 0, &ifdata))
                continue;

            BOOL ok = SetupDiGetDeviceProperty(infoset, &infodata, &DEVPKEY_Device_HardwareIds,
                &type, buffer, BIG_BUFFER_SIZE, NULL, 0);

            if (!ok)
                continue;
            found_hwid = (wchar_t*)buffer;

            std::transform(hwid.begin(), hwid.end(), hwid.begin(), tcsupper);
            std::transform(found_hwid.begin(), found_hwid.end(), found_hwid.begin(), tcsupper);
            if (0 != hwid.compare(found_hwid))
                continue;

            RtlZeroMemory(buffer, BIG_BUFFER_SIZE);
            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            if (!SetupDiGetDeviceInterfaceDetail(
                infoset, &ifdata, ifdetail, BIG_BUFFER_SIZE, NULL, NULL))
                continue;

            devpath = ifdetail->DevicePath;
            break;
        }
        SetupDiDestroyDeviceInfoList(infoset);
    }
}

BOOL QueryParentController(PHYDISK_INFO& info, HDEVINFO infoset, PSP_DEVINFO_DATA infodata)
{
    DEVPROPTYPE type = DEVPROP_TYPE_STRING;
    BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
    BOOL ok = SetupDiGetDeviceProperty(infoset, infodata, &DEVPKEY_Device_Parent,
        &type, buffer, BIG_BUFFER_SIZE, NULL, 0);
    if (!ok)
    {
        DWORD error = GetLastError();
        //result = _T("");
        return FALSE;
    }

    info.ParentHwID = (wchar_t*)buffer;
    tstring devpath;
    if (QueryDevInterfaceByHwId(info.ParentDevPath, info.ParentHwID))
        return TRUE;

    return FALSE;
}



int _tmain(int argc, TCHAR* argv[])
{

    return 0;
}
