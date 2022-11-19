#include "Common.h"

DWORD COpalFactory::EnumOpalDevices(IN OUT list<OPAL_DEVICE_INFO>& result)
{
    list<tstring> disklist;
    DWORD error = ERROR_SUCCESS;
    
    error = EnumDisks(disklist);
    if (ERROR_SUCCESS != error)
        return error;

    for(tstring &disk : disklist)
    {
        OPAL_DEVICE_INFO info = { BusTypeUnknown, 0};
        error = OpalDiscovery0(disk, info);
        if (ERROR_SUCCESS == error)
            result.push_back(info);
    }

    if(0 == result.size())
        return ERROR_NOT_FOUND;

    return ERROR_SUCCESS;
}
DWORD COpalFactory::QueryOpalDevices(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result) 
{
}
DWORD COpalFactory::OpalDiscovery0(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result) 
{
//check 
}


DWORD COpalFactory::EnumDisks(IN OUT list<tstring> &result)
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

