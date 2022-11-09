// SED_QueryByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

void Usage()
{
    _tprintf(_T("Usage: SED_QueryByDisk.exe <physical disk devpath>\n"));
    _tprintf(_T("example: SED_QueryByDisk.exe \\\\.\\PhysicalDrive3\n"));
    _tprintf(_T("\n"));
}

void ShowStructureSizes()
{
    _tprintf(_T("sizeof(FEATURE_DESCRIPTOR) = %lld\n"), sizeof(FEATURE_DESCRIPTOR));
    _tprintf(_T("sizeof(FEATURE_DESC_HEADER) = %lld\n"), sizeof(FEATURE_DESC_HEADER));
    _tprintf(_T("\n"));
    _tprintf(_T("sizeof(FEATURE_DESC_TPer) = %lld\n"), sizeof(FEATURE_DESC_TPer));
    _tprintf(_T("sizeof(FEATURE_DESC_LOCKING) = %lld\n"), sizeof(FEATURE_DESC_LOCKING));
    _tprintf(_T("sizeof(FEATURE_DESC_GEOMETRY) = %lld\n"), sizeof(FEATURE_DESC_GEOMETRY));
    _tprintf(_T("sizeof(FEATURE_DESC_ENTERPRISE_SSC) = %lld\n"), sizeof(FEATURE_DESC_ENTERPRISE_SSC));
    _tprintf(_T("sizeof(FEATURE_DESC_SINGLE_USER_MODE) = %lld\n"), sizeof(FEATURE_DESC_SINGLE_USER_MODE));
    _tprintf(_T("sizeof(FEATURE_DESC_OPAL_V100) = %lld\n"), sizeof(FEATURE_DESC_OPAL_V100));
    _tprintf(_T("sizeof(FEATURE_DESC_OPAL_V200) = %lld\n"), sizeof(FEATURE_DESC_OPAL_V200));
    _tprintf(_T("sizeof(FEATURE_DESC_DATASTORE) = %lld\n"), sizeof(FEATURE_DESC_DATASTORE));
}

void PrintDiscoveryResult(BYTE buffer[])
{}

//Discovery0 is TCG defined "device discovery layer0" behavior.
bool Discovery0(OUT BYTE buffer[], IN tstring &diskname)
{
    HANDLE device = CreateFile(diskname.c_str(),
                        GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE,
                        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

    

    if(INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return false;
}

int _tmain(int argc, TCHAR* argv[])
{

    if(argc<2)
    {
        Usage();
        return -1;
    }

    tstring diskname = argv[1];
    ShowStructureSizes();

    BYTE *buffer = new BYTE[BIG_BUFFER_SIZE];
    RtlZeroMemory(buffer, BIG_BUFFER_SIZE);

    if(Discovery0(buffer, diskname))
    {
        PrintDiscoveryResult(buffer);
    }

    delete[] buffer;
}
