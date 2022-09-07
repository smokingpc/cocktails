#include "Common.h"

//devpath example : "\\.\PhysicalDrive4"
size_t EnumPhysicalDisks(list<tstring> &devpath_list)
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
            unique_ptr<BYTE> bufptr(buffer);
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
                    CloseHandle(device);

                    TCHAR temp[TINY_BUFFER_SIZE] = {0};
                    _stprintf_s(temp, TINY_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_number.DeviceNumber);
                    devpath_list.push_back(temp);
                }
                else
                    printf("CreateFile() failed, LastError=%d\n", GetLastError());
            }
            else
                printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
        }
    }

    printf("EnumPhysicalDisk() Finished....\n");

    return devpath_list.size();
}

//devpath => physical disk devpath
// e.g.  "\\.\PhysicalDrive4"
BOOL GetPartitionList(IN OUT BYTE* buffer, DWORD buf_size, tstring& devpath)
{
    DWORD ret_size = 0;
    BOOL ok = FALSE;
    HANDLE disk = CreateFile(devpath.c_str(), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE != disk)
    {
        //IOCTL_DISK_GET_DRIVE_LAYOUT_EX won't tell you "how big of buffer it need."
        // so we have to allocate a huge buffer which big enough to query...
        ok = DeviceIoControl(disk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, buffer, buf_size, &ret_size, NULL);

        if (!ok)
        {
            _tprintf(_T("IOCTL_DISK_GET_DRIVE_LAYOUT_EX failed, last error=%d\n"), GetLastError());
        }

        CloseHandle(disk);
    }
    else
        _tprintf(_T("Open physical disk [%s] failed. LastError=%d\n"), devpath.c_str(), GetLastError());

    return ok;
}