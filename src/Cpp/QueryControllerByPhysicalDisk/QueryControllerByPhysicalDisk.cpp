// QueryControllerByPhysicalDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

static bool FindCtrl(PCONTROLLER_INFO *found, list<CONTROLLER_INFO>& ctrllist, tstring& devpath)
{
    for(auto &item : ctrllist)
    {
        if(0==devpath.compare(item.DevPath))
        {
            *found = &item;
            return true;
        }
    }

    return false;
}

//find volumes which build on specified physical disk.
//result stores "volume names" of search result.
static size_t FindVolumes(list<tstring> &result, list<VOLUME_INFO>& vollist, tstring phydisk)
{
    for(auto & volume : vollist)
    {
        list<tstring>::iterator found = std::find(volume.PhyDisks.begin(), volume.PhyDisks.end(), phydisk);
        if(found != volume.PhyDisks.end())
            result.push_back(volume.VolumeName);
    }

    return result.size();
}

static bool IsDiskUnderController(PCONTROLLER_INFO ctrl, tstring phydisk)
{
    for(auto &disk:ctrl->Disks)
    {
        if(0==disk.PhyDiskName.compare(phydisk))
            return true;
    }

    return false;
}

static size_t BuildControllerInfoList(list<CONTROLLER_INFO>& result, list<DISK_INFO>& disklist, list<VOLUME_INFO>& vollist)
{
    for(auto &disk : disklist)
    {
        if(disk.CtrlDevPath.size() == 0)
            continue;
    //first search if this controller exist? if exist then search current disk.
    //if current not exist , build a new disk volume info and push it.
        //list<CONTROLLER_INFO>::iterator iter = result.end();
        PCONTROLLER_INFO found;
        if(FindCtrl(&found, result, disk.CtrlDevPath))
        {
            if(false == IsDiskUnderController(found, disk.PhyDiskName))
            {
                DISK_VOLUME_INFO diskinfo;
                diskinfo.DevPath = disk.DevPath;
                diskinfo.PhyDiskName = disk.PhyDiskName;
                FindVolumes(diskinfo.Volumes, vollist, disk.PhyDiskName);
                found->Disks.push_back(diskinfo);
            }
        }
        else
        {
            CONTROLLER_INFO ctrlinfo;
            ctrlinfo.DevPath = disk.CtrlDevPath;
            DISK_VOLUME_INFO diskinfo;
            diskinfo.DevPath = disk.DevPath;
            diskinfo.PhyDiskName = disk.PhyDiskName;
            
            FindVolumes(diskinfo.Volumes, vollist, disk.PhyDiskName);
            ctrlinfo.Disks.push_back(diskinfo);

            result.push_back(ctrlinfo);
        }
    }

    return 0;
}

int _tmain(int argc, TCHAR* argv[])
{
    list<DISK_INFO> disklist;
    EnumDiskInfo(disklist);

    list<VOLUME_INFO>vollist;
    EnumVolumeInfo(vollist);

    list<CONTROLLER_INFO> ctrllist;
    BuildControllerInfoList(ctrllist, disklist, vollist);

    _tprintf(_T("======================== Volume list ========================\n"));
    for(auto &info : vollist)
    {
        _tprintf(_T("Volume [%s]\n"), info.VolumeName.c_str());
        _tprintf(_T("\tBuilt on Phydisk : "));
        for(auto &disk : info.PhyDisks)
            _tprintf(_T("%s, "), disk.c_str());
        _tprintf(_T("\n"));

        _tprintf(_T("\tVolume mount points : "));
        for (auto& mount : info.MountPointList)
            _tprintf(_T("%s, "), mount.c_str());
        _tprintf(_T("\n"));
    }

    _tprintf(_T("======================== Controller list ========================\n"));
    for(auto &ctrl : ctrllist)
    {
        _tprintf(_T("Controller [%s]\n"), ctrl.DevPath.c_str());
        _tprintf(_T("\tHas Phydisks : \n"));
        for (auto& disk : ctrl.Disks)
        {
            _tprintf(_T("\tPhysicalDisk (%s)\n"), disk.PhyDiskName.c_str());
            _tprintf(_T("\tDevPath (%s)\n"), disk.DevPath.c_str());
            _tprintf(_T("\tHas Volumes:\n"));
            for(auto &vol : disk.Volumes)
                _tprintf(_T("\t\t%s\n"), vol.c_str());
        }
        _tprintf(_T("\n"));
    }
    //_tprintf(_T("========================  ========================\n"));

    return 0;
}

