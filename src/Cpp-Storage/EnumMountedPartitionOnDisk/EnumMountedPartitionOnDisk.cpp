// EnumMountedPartitionOnDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Common.h"

int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> disk_list;
    size_t count = EnumPhysicalDisks(disk_list);

    if(0 == count)
    {
        _tprintf(_T("EnumPhysicalDisks() has no results...\n"));
        return -1;
    }

    for(auto &devpath : disk_list)
    {
        _tprintf(_T("Found Disk => [%s]\n"), devpath.c_str());

        unique_ptr<DRIVE_LAYOUT_INFORMATION_EX> bufptr((DRIVE_LAYOUT_INFORMATION_EX*)new BYTE[BIG_BUFFER_SIZE]);
        ZeroMemory(bufptr.get(), BIG_BUFFER_SIZE);

        if(!GetPartitionList((BYTE*)bufptr.get(), BIG_BUFFER_SIZE, devpath))
            continue;
    }

    list<VOLUME_INFO> volume_list;
    
    //count = EnumVolumesByDriveLetter(volume_list);
    count = EnumVolumes(volume_list);

    if(0 == count)
        _tprintf(_T("Volume Enumeration didn't get any mounted volumes...\n"));
    else
    {
        _tprintf(_T("[Volume Enumeration]\n"));

        for (auto& volume : volume_list)
        {
            _tprintf(_T("Volume (%s)"), volume.VolumeName.c_str());
            if (!IsVolumeReady(volume.VolumeName))
                _tprintf(_T("** It is NOT READY! Should be an empty drive! \n"));
            else
                _tprintf(_T("\n"));

            if(IsVolumeMounted(volume.VolumeName))
                _tprintf(_T("\t => Volume is already mounted. \n"));

            if (volume.PhyDisks.size() > 0)
            {
                _tprintf(_T("\tPhysical disk span list: \n"));

                for (tstring& disk : volume.PhyDisks)
                    _tprintf(_T("\t\t%s\n"), disk.c_str());
            }

            if(volume.MountPointList.size() > 0)
            {
                _tprintf(_T("\tMounted point list:\n"));

                for(auto& mount: volume.MountPointList)
                    _tprintf(_T("\t\t%s\n"), mount.c_str());
            }
        }
    }
    return 0;
}

