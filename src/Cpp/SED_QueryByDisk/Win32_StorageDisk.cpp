#include "Common.h"
#pragma comment( lib, "setupapi.lib" )

bool IdentifyStorageType(OUT STORAGE_BUS_TYPE& type, IN tstring& diskname)
{
    bool ok = false;
    DWORD ret_size = 0;
    STORAGE_PROPERTY_QUERY cmd;
    STORAGE_DEVICE_DESCRIPTOR result = { 0 };
    type = BusTypeUnknown;

    HANDLE device = CreateFile(diskname.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), diskname.c_str(), GetLastError());
        return false;
    }

    cmd.PropertyId = StorageDeviceProperty;
    cmd.QueryType = PropertyStandardQuery;

    ok = DeviceIoControl(device,
        IOCTL_STORAGE_QUERY_PROPERTY,
        &cmd,
        sizeof(STORAGE_PROPERTY_QUERY),
        &result,
        sizeof(STORAGE_DEVICE_DESCRIPTOR),
        &ret_size,
        FALSE);

    if (!ok)
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());
    else
        type = result.BusType;

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return ok;
}

size_t EnumPhysicalDisks(list<tstring> &result) 
{
    HDEVINFO devinfo = NULL;
    GUID disk_class_guid = GUID_DEVINTERFACE_DISK;
    GUID* debug = &disk_class_guid;
    devinfo = SetupDiGetClassDevs(&disk_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE != devinfo)
    {
        SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        DWORD devid = 0;
        while (TRUE == SetupDiEnumDeviceInterfaces(devinfo, NULL, &disk_class_guid, devid, &ifdata))
        {
            DWORD need_size = 0;
            DWORD return_size = 0;
            BOOL ok = FALSE;
            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
            devid++;
            SetupDiGetDeviceInterfaceDetail(devinfo, &ifdata, NULL, 0, &need_size, NULL);
            need_size = need_size * 2;
            BYTE* buffer = new BYTE[need_size];
            ZeroMemory(buffer, need_size);

            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            ok = SetupDiGetDeviceInterfaceDetail(devinfo, &ifdata, ifdetail, need_size, &need_size, NULL);
            if (TRUE == ok)
            {
                HANDLE device = CreateFile(ifdetail->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, 0, NULL);
                if (INVALID_HANDLE_VALUE != device)
                {
                    STORAGE_DEVICE_NUMBER disk_number = { 0 };
                    return_size = 0;
                    ok = DeviceIoControl(device,
                        IOCTL_STORAGE_GET_DEVICE_NUMBER,
                        NULL,
                        0,
                        &disk_number,
                        sizeof(STORAGE_DEVICE_NUMBER),
                        &return_size,
                        NULL);

                    if (TRUE == ok)
                    {
                        wchar_t temp[32] = {0};
                        swprintf_s(temp, 32, PHYSICAL_DISK_FORMAT, disk_number.DeviceNumber);
                        result.push_back(temp);
                    }
                    CloseHandle(device);
                }
                else
                    printf("CreateFile() failed, LastError=%d\n", GetLastError());
            }
            else
                printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
            delete[] buffer;
        }
    }

    return result.size();
}
