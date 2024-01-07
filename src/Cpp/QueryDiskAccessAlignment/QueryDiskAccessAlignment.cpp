// QueryDiskAccessAlignment.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <string>
#include <list>
#include <SetupAPI.h>
#include <iostream>
#include <initguid.h>
#include <ntddstor.h>
#include <nvme.h>

#define PAGE_SIZE   4096
#define QUERY_BUF_SIZE  (PAGE_SIZE*2)
using std::list;
typedef std::basic_string<TCHAR> tstring;
#define PHYSICAL_DISK_NAME_FORMAT    _T("\\\\.\\PhysicalDrive%d")

#pragma comment(lib, "setupapi.lib")

//disklist returns "\\.\PhysicalDriveXX" name list.
size_t EnumPhysicalDisks(list<tstring> &disklist)
{
    HDEVINFO devinfo = NULL;
    GUID class_guid = GUID_DEVINTERFACE_DISK;
    GUID* debug = &class_guid;
    devinfo = SetupDiGetClassDevs(&class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE != devinfo)
    {
        SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        DWORD devid = 0;
        while (TRUE == SetupDiEnumDeviceInterfaces(devinfo, NULL, &class_guid, devid, &ifdata))
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
                HANDLE device = CreateFile(ifdetail->DevicePath, 
                                    GENERIC_READ | GENERIC_WRITE, 
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
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
                        TCHAR buffer2[MAX_PATH] = {0};
                        _stprintf_s(buffer2, MAX_PATH, PHYSICAL_DISK_NAME_FORMAT, disk_number.DeviceNumber);
                        disklist.push_back(buffer2);
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

    return disklist.size();
}

void QueryDiskAccessAlignment(tstring &diskname, STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR *desc)
{
    HANDLE device = CreateFile(diskname.c_str(),
        STANDARD_RIGHTS_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE != device)
    {
        DWORD ret_size = 0;
        TCHAR buffer[PAGE_SIZE] = {0};
        PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)buffer;
        query->QueryType = PropertyStandardQuery;
        query->PropertyId = StorageAccessAlignmentProperty;
        BOOL ok = DeviceIoControl(device,
            IOCTL_STORAGE_QUERY_PROPERTY,
            buffer,
            PAGE_SIZE,
            buffer,
            PAGE_SIZE,
            &ret_size,
            NULL);

        if (TRUE == ok)
        {
            //desc = (PSTORAGE_ACCESS_ALIGNMENT_DESCRIPTOR) buffer;
            RtlCopyMemory(desc, buffer, sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR));
        }
        CloseHandle(device);
    }
    else
        printf("CreateFile() failed, LastError=%d\n", GetLastError());
}

int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> disklist;
    _tprintf(_T("Enumerate Physical Disks:\n"));
    EnumPhysicalDisks(disklist);

    for(tstring &disk : disklist)
    {
        STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR desc = {0};
        QueryDiskAccessAlignment(disk, &desc);
        _tprintf(_T("Disk Access Alignemnt of [%s]:\n"), disk.c_str());
        _tprintf(_T("  BytesPerLogicalSector=%d, BytesPerPhysicalSector=%d\n"), desc.BytesPerLogicalSector, desc.BytesPerPhysicalSector);
        _tprintf(_T("  BytesPerCacheLine=%d, BytesOffsetForCacheAlignment=%d, BytesOffsetForSectorAlignment=%d\n"), desc.BytesPerCacheLine, desc.BytesOffsetForCacheAlignment, desc.BytesOffsetForSectorAlignment);
        _tprintf(_T("\n"));
    }

    _tprintf(_T("End...\n"));
}
