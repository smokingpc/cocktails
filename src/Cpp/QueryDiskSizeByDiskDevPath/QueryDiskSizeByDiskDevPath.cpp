// QueryDiskSizeByDiskDevPath.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <string>
#include <iostream>

#define SIZE_1MB    1048576

void Usage()
{
    _tprintf(_T("QueryDiskSizeByDiskDevPath.exe [devpath to disk]\n"));
    _tprintf(_T("example:\n"));
    _tprintf(_T("QueryDiskSizeByDiskDevPath.exe \"\\\\?\\scsi#disk&ven_nvme&prod_samsung_ssd_980#5&2e4e6c10&1&000000#{53f56307-b6bf-11d0-94f2-00a0c91efb8b}\"\n"));
    _tprintf(_T("\n"));
}

DWORD GetDiskSize(TCHAR *devpath, size_t &size)
{
    BOOL    ok = FALSE;
    ULONG   ret_size = 0;
    DWORD error = 0;
    GET_LENGTH_INFORMATION info = {0};

    HANDLE hDevice = CreateFile(devpath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hDevice == INVALID_HANDLE_VALUE) {
        error = GetLastError();
        _tprintf(_T("CreateFile to devpath failed. LastError=%d\n"), error);
        return error;
    }

    ok = DeviceIoControl(hDevice,
        IOCTL_DISK_GET_LENGTH_INFO,
        NULL,
        0,
        &info,
        sizeof(GET_LENGTH_INFORMATION),
        &ret_size,
        NULL
    );

    if (!ok) {
        error = GetLastError();
        _tprintf(_T("DeviceIoControl failed. LastError=%d\n"), error);
        return error;
    }

    size = info.Length.QuadPart;
    CloseHandle(hDevice);

    return ERROR_SUCCESS;
}

int _tmain(int argc, TCHAR* argv[])
{
    if(argc<2)
    {
        Usage();
        return -1;
    }
    size_t size = 0;
    DWORD error = GetDiskSize(argv[1], size);

    if(ERROR_SUCCESS == error)
        _tprintf(_T("size of specified device == %lld\n"), size);

    return 0;
}
