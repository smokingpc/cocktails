#include "Common.h"
using namespace std;
#pragma comment( lib, "setupapi.lib" )

namespace OpalFactory
{
void EnumOpalDevices(IN OUT std::list<COpalDevice*>& result)
{
    list<tstring> disklist;
    EnumWin32Disks(disklist);
    
    //disk ==> physical disk name. e.g. "\\.\PhysicalDrive5"
    for(tstring &disk : disklist)
    {
        STORAGE_BUS_TYPE type = QueryStorageType(disk);
        if (BusTypeNvme != type)
        {
            _tprintf(_T("disk[%s] BusType[%d] != %d, skipping...\n"), disk.c_str(), BusTypeNvme, type);
            continue;
        }

        COpalDevice *device = new COpalNvme(disk);
        result.push_back(device);
    }
}
DWORD QueryOpalDevices(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result) 
{ 
    return ERROR_NOT_SUPPORTED; 
}
OPAL_DEVICE_INFO* CreateOpalDevice(IN tstring& diskname)
{
    return NULL;
}
size_t EnumOpalDevice(IN tstring& diskname, IN OUT list< COpalDevice*> devlist)
{
    return 0;
}

STORAGE_BUS_TYPE QueryStorageType(IN tstring& diskname)
{
    bool ok = false;
    DWORD ret_size = 0;
    STORAGE_PROPERTY_QUERY cmd;
    STORAGE_DEVICE_DESCRIPTOR result = { 0 };
    STORAGE_BUS_TYPE type = BusTypeUnknown;

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
        return BusTypeUnknown;
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
    {
        type = BusTypeUnknown;
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());
    }
    else
        type = result.BusType;

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return type;
}

DWORD EnumWin32Disks(IN OUT list<tstring> &result)
{
    HDEVINFO devinfo = NULL;
    GUID class_guid = GUID_DEVINTERFACE_DISK;
    //find all "currently exist" DiskClass objects, query their device interface (DevPath)
    devinfo = SetupDiGetClassDevs(&class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if(INVALID_HANDLE_VALUE == devinfo)
        return ERROR_INVALID_CLASS;

    SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
    ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    DWORD devid = 0;
    while (TRUE == SetupDiEnumDeviceInterfaces(devinfo, NULL, &class_guid, devid, &ifdata))
    {
        DWORD need_size = 0;
        DWORD ret_size = 0;
        BOOL ok = FALSE;
        PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
        devid++;
        SetupDiGetDeviceInterfaceDetail(devinfo, &ifdata, NULL, 0, &need_size, NULL);
        need_size = need_size * 2;  //for unicode query, double the buffer size.

        BYTE* buffer = new BYTE[need_size];
        unique_ptr<BYTE> bufptr(buffer);
        ZeroMemory(buffer, need_size);

        ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
        ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        ok = SetupDiGetDeviceInterfaceDetail(devinfo, &ifdata, ifdetail, need_size, &need_size, NULL);
        if(!ok)
        {
            printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
            continue;
        }

        HANDLE device = CreateFile(ifdetail->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, 0, NULL);

        if (INVALID_HANDLE_VALUE == device)
        {
            printf("CreateFile() failed, LastError=%d\n", GetLastError());
            continue;
        }

        STORAGE_DEVICE_NUMBER disk_number = { 0 };
        ret_size = 0;
        ok = DeviceIoControl(device,
            IOCTL_STORAGE_GET_DEVICE_NUMBER,
            NULL,
            0,
            &disk_number,
            sizeof(STORAGE_DEVICE_NUMBER),
            &ret_size,
            NULL);

        if (TRUE == ok)
        {
            wchar_t temp[32] = { 0 };
            swprintf_s(temp, 32, PHYSICAL_DISK_FORMAT, disk_number.DeviceNumber);
            result.push_back(temp);
        }
        CloseHandle(device);
    }

    if(0==result.size())
        return ERROR_NOT_FOUND;
    
    return ERROR_SUCCESS;
}

}