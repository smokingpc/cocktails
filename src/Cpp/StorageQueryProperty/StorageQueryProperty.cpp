// StorageQueryProperty.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

void Usage()
{
    printf("Usage: StorageQueryProperty.exe [physical_drive_devpath]\n");
    printf("e.g.: StorageQueryProperty.exe \\\\.\\PhysicalDrive1 \n\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
    if(argc<2)
    {
        Usage();
        return -1;
    }


}

