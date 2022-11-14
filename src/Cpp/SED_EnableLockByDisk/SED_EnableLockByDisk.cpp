// SED_EnableLockByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"


int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> disklist;
    EnumPhysicalDisks(disklist);
    for (auto& diskname : disklist)
    {
        STORAGE_BUS_TYPE type = BusTypeUnknown;
        if (!IdentifyStorageType(type, diskname))
        {
            _tprintf(_T("Identify storage type of specified disk [%s] failed.\n"), diskname.c_str());
            return -1;
        }

    
    }
}
