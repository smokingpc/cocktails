// ListBusyStorageController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

int _tmain(int argc, TCHAR* argv[])
{
    list<CONTROLLER_INFO> busy_list;
    list<CONTROLLER_INFO> free_list;
    list<PHYDISK_INFO> disk_list;
    list<VOLUME_INFO> vol_list;

    EnumVolumes(vol_list);
    EnumControllers(busy_list, free_list, disk_list, vol_list);

    //print all lists

    return 0;
}
