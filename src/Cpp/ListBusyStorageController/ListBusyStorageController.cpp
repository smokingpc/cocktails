// ListBusyStorageController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

void PrintInfo(OUT list<CONTROLLER_INFO>& busy_list, OUT list<CONTROLLER_INFO>& free_list,
                OUT list<PHYDISK_INFO>& disk_list, IN list<VOLUME_INFO>& vol_list)
{
    _tprintf(_T("======== Print Parsed Information ========\n"));
    _tprintf(_T("[Busy Controller List]\n"));
    for(auto &ctrl:busy_list)
    {
        _tprintf(_T("HwId=%s\n"), ctrl.HwId.c_str());
        _tprintf(_T("DevPath=%s\n"), ctrl.DevPath.c_str());
        _tprintf(_T("Has mounted volumes? (%s)\n"), ctrl.IsBusy? _T("TRUE") : _T("FALSE"));

        _tprintf(_T("\n"));
    }
    _tprintf(_T("\n"));

    _tprintf(_T("[Free Controller List]\n"));
    for (auto& ctrl : free_list)
    {
        _tprintf(_T("HwId=%s\n"), ctrl.HwId.c_str());
        _tprintf(_T("DevPath=%s\n"), ctrl.DevPath.c_str());
        _tprintf(_T("Has mounted volumes? (%s)\n"), ctrl.IsBusy ? _T("TRUE") : _T("FALSE"));

        _tprintf(_T("\n"));
    }
    _tprintf(_T("\n"));

    _tprintf(_T("[PhysicalDisk List]\n"));
    for (auto& disk : disk_list)
    {
        _tprintf(_T("DiskName=%s\n"), disk.PhyDisk.c_str());
        _tprintf(_T("Parent HwId=%s\n"), disk.ParentHwID.c_str());
        _tprintf(_T("Parent DevPath=%s\n"), disk.ParentDevPath.c_str());

        _tprintf(_T("\n"));
    }
    _tprintf(_T("\n"));

    _tprintf(_T("[Mounted Volumes List]\n"));
    for (auto& volume : vol_list)
    {
        _tprintf(_T("VolumeName=%s\n"), volume.VolumeName.c_str());
        for(auto &mount : volume.MountPointList)
        {
            _tprintf(_T("    MountPoint=%s\n"), mount.c_str());
            _tprintf(_T("\n"));
        }
        for (auto& disk : volume.PhyDisks)
        {
            _tprintf(_T("    PhyDisk=%s\n"), disk.c_str());
            _tprintf(_T("\n"));
        }

        _tprintf(_T("\n"));
    }

    _tprintf(_T("\n"));
    _tprintf(_T("Finished........\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    list<CONTROLLER_INFO> busy_list;
    list<CONTROLLER_INFO> free_list;
    list<PHYDISK_INFO> disk_list;
    list<VOLUME_INFO> vol_list;

    EnumVolumes(vol_list);
    EnumControllers(busy_list, free_list, disk_list, vol_list);

    //print all lists
    PrintInfo(busy_list, free_list, disk_list, vol_list);
    return 0;
}
