// SED_QueryByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

void Usage()
{
    _tprintf(_T("Usage: SED_QueryByDisk.exe <physical disk devpath>\n"));
    _tprintf(_T("example: SED_QueryByDisk.exe \\\\.\\PhysicalDrive3\n"));
    _tprintf(_T("\n"));
}




int _tmain(int argc, TCHAR* argv[])
{

    if(argc<2)
    {
        Usage();
        return -1;
    }


}
