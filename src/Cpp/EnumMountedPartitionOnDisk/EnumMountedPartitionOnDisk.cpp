// EnumMountedPartitionOnDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Common.h"

int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> list;
    size_t count = EnumPhysicalDisks(list);

    if(0 == count)
    {
        _tprintf(_T("EnumPhysicalDisks() has no results...\n"));
        return -1;
    }

    for(auto &devpath : list)
    {
        _tprintf(_T("Found Disk => [%s]\n"), devpath.c_str());

        unique_ptr<DRIVE_LAYOUT_INFORMATION_EX> bufptr((DRIVE_LAYOUT_INFORMATION_EX*)new BYTE[BIG_BUFFER_SIZE]);
        ZeroMemory(bufptr.get(), BIG_BUFFER_SIZE);

        if(!GetPartitionList((BYTE*)bufptr.get(), BIG_BUFFER_SIZE, devpath))
            continue;

        
    }

    return 0;
}

