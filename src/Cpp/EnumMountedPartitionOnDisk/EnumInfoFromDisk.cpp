#include "Common.h"

const TCHAR *DriveLetters[] = {
    _T("A:"), _T("B:"), _T("C:"), _T("D:"), _T("E:"),
    _T("F:"), _T("G:"), _T("H:"), _T("I:"), _T("J:"),
    _T("K:"), _T("L:"), _T("M:"), _T("N:"), _T("O:"),
    _T("P:"), _T("Q:"), _T("R:"), _T("S:"), _T("T:"),
    _T("U:"), _T("V:"), _T("W:"), _T("X:"), _T("Y:"),
    _T("Z:")
};

BOOL IsVolumeReady(tstring vol_name)
{
    tstring temp = vol_name;
    //if (temp.back() == _T('\\'))
    //    temp.pop_back();

    HANDLE device = CreateFile(temp.c_str(), 0,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, 
                            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if(device != INVALID_HANDLE_VALUE)
    {
        CloseHandle(device);

        if (GetLastError() == ERROR_NOT_READY)
            return FALSE;

        return TRUE;
    }

    return FALSE;
}

static void QueryVolumeMountFolders(tstring vol_name, list<tstring> &mount_list)
{
    TCHAR buffer[BIG_BUFFER_SIZE] = {0};
    tstring temp = vol_name;
    if (temp.back() != _T('\\'))
        temp += _T('\\');

    HANDLE find = FindFirstVolumeMountPoint(temp.c_str(), buffer, BIG_BUFFER_SIZE);
    BOOL find_ok = TRUE;

    if (INVALID_HANDLE_VALUE != find)
    {
        while (find_ok)
        {
            mount_list.push_back(buffer);
            RtlZeroMemory(buffer, sizeof(buffer));
            find_ok = FindNextVolumeMountPoint(find, buffer, BIG_BUFFER_SIZE);
        }
        FindVolumeMountPointClose(find);
    }

}

static void EnumDriveMountInfo(list<DRIVE_MOUNT_INFO>& mount_list)
{
    for (const TCHAR* drive : DriveLetters)
    {
        TCHAR buffer[SMALL_BUFFER_SIZE] = {0};
        tstring drive_name = drive;
        drive_name += _T("\\");
        BOOL ok = GetVolumeNameForVolumeMountPoint(drive_name.c_str(), buffer, SMALL_BUFFER_SIZE);
        if (ok)
        {
            TCHAR buffer2[TINY_BUFFER_SIZE] = {0};
            _stprintf_s(buffer2, TINY_BUFFER_SIZE, DOSDRIVE_FORMAT, drive);

            DRIVE_MOUNT_INFO info;
            info.DriveName = buffer2;
            info.MountVolume = buffer;
            mount_list.push_back(info);
        }
    }
}

static BOOL BuildVolumeInfo(VOLUME_INFO &info)
{
    //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
    //to open volume, back-slash on tail should be removed. or the IOCTL will always return lasterror 0x57
    tstring temp = info.VolumeName;
    if(temp.back() == _T('\\'))
        temp.pop_back();        //remove back-slash on string tail. 

    HANDLE vol_handle = CreateFile(temp.c_str(), 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (INVALID_HANDLE_VALUE != vol_handle)
    {
        DWORD ret_size = 0;
        BYTE buffer[BIG_BUFFER_SIZE] = {0};

        //Ask this volume : "where is your physical disk?".
        //TODO: an Volume could span into multiple physical disks.
        BOOL ok = DeviceIoControl(vol_handle, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
            NULL, 0,
            buffer, sizeof(buffer),
            &ret_size, NULL);
        if (ok)
        {
            PVOLUME_DISK_EXTENTS exts = (PVOLUME_DISK_EXTENTS)buffer;
            for (DWORD i = 0; i < exts->NumberOfDiskExtents; i++)
            {
                DWORD disk_id = exts->Extents[i].DiskNumber;
                TCHAR buffer2[SMALL_BUFFER_SIZE] = { 0 };
                _stprintf_s(buffer2, SMALL_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_id);
                info.PhyDisks.push_back(buffer2);
            }

            //Ask this volume : "where is your mount points ?"
            QueryVolumeMountFolders(info.VolumeName, info.MountPointList);
        }
        else
        {
            _tprintf(_T("(%s) IOCTL failed. last error = 0x%08X\n"), temp.c_str(), GetLastError());
            return FALSE;
        }
        CloseHandle(vol_handle);
    }
    else
    {
        _tprintf(_T("open (%s) failed. last error = 0x%08X\n"), temp.c_str(), GetLastError());
        return FALSE;
    }

    return TRUE;
}

//devpath example : "\\.\PhysicalDrive4"
size_t EnumPhysicalDisks(list<tstring> &devpath_list)
{
    HDEVINFO devinfo = NULL;
    GUID disk_class_guid = GUID_DEVINTERFACE_DISK;
    GUID* debug = &disk_class_guid;
    devinfo = SetupDiGetClassDevs(&disk_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE != devinfo)
    {
        SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        DWORD devid = 0;
        while (TRUE == SetupDiEnumDeviceInterfaces(devinfo, NULL, &disk_class_guid, devid, &ifdata))
        {
            DWORD need_size = 0;
            DWORD return_size = 0;
            BOOL ok = FALSE;
            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
            devid++;
            SetupDiGetDeviceInterfaceDetail(devinfo, &ifdata, NULL, 0, &need_size, NULL);
            need_size = need_size * 2;
            BYTE* buffer = new BYTE[need_size];
            unique_ptr<BYTE> bufptr(buffer);
            ZeroMemory(buffer, need_size);

            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            ok = SetupDiGetDeviceInterfaceDetail(devinfo, &ifdata, ifdetail, need_size, &need_size, NULL);
            if (TRUE == ok)
            {
                HANDLE device = CreateFile(ifdetail->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, 0, NULL);
                if (INVALID_HANDLE_VALUE != device)
                {
                    STORAGE_DEVICE_NUMBER disk_number = { 0 };
                    return_size = 0;
                    ok = DeviceIoControl(device,
                        IOCTL_STORAGE_GET_DEVICE_NUMBER,
                        NULL,
                        0,
                        &disk_number,
                        sizeof(STORAGE_DEVICE_NUMBER),
                        &return_size,
                        NULL);
                    CloseHandle(device);

                    TCHAR temp[TINY_BUFFER_SIZE] = {0};
                    _stprintf_s(temp, TINY_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_number.DeviceNumber);
                    devpath_list.push_back(temp);
                }
                else
                    printf("CreateFile() failed, LastError=%d\n", GetLastError());
            }
            else
                printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
        }
    }

    printf("EnumPhysicalDisk() Finished....\n");

    return devpath_list.size();
}

//size_t EnumVolumesByDriveLetter(list<VOLUME_INFO> &volume_list)
//{
//    volume_list.clear();
//
//    //windows only can mount volumes from A: to Z:.
//    //So we only have to enum 26 drives.
//    //Note: mount points to check need a tail back-slash.
//    for(const TCHAR* item : DriveLetters)
//    {
//        TCHAR buffer[BIG_BUFFER_SIZE] = {0};
//        //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
//        BOOL ok = GetVolumeNameForVolumeMountPoint(item, buffer, BIG_BUFFER_SIZE);
//        if(ok)
//        {
//            VOLUME_INFO info;
//            info.MountPointList.push_back(item);
//            info.VolumeName = buffer;
//
//            //to open volume, back-slash on tail should be removed. or the IOCTL will always return lasterror 0x57
//            tstring temp = info.VolumeName;
//            temp.pop_back();        //remove back-slash on string tail. 
//
//            //FILE_FLAG_BACKUP_SEMANTICS => override to skip file security checks
//            //open volume don't need specific DesiredAccess
//            HANDLE device = CreateFile(temp.c_str(), 0,
//                                    FILE_SHARE_READ | FILE_SHARE_WRITE, 
//                                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
//            if(INVALID_HANDLE_VALUE != device)
//            {
//                DWORD ret_size = 0;
//                RtlZeroMemory(buffer, BIG_BUFFER_SIZE * sizeof(TCHAR));
//                ok = DeviceIoControl(device, IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS,
//                                        NULL, 0, 
//                                        buffer, BIG_BUFFER_SIZE * sizeof(TCHAR),
//                                        &ret_size, NULL);
//                if(ok)
//                {
//                    PVOLUME_DISK_EXTENTS exts = (PVOLUME_DISK_EXTENTS)buffer;
//                    for(DWORD i=0; i<exts->NumberOfDiskExtents; i++)
//                    {
//                        DWORD disk_id = exts->Extents[i].DiskNumber;
//                        TCHAR buffer2[SMALL_BUFFER_SIZE] = {0};
//                        _stprintf_s(buffer2, SMALL_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_id);
//                        info.PhyDisks.push_back(buffer2);
//                    }
//                    volume_list.push_back(info);
//                }
//                else
//                    _tprintf(_T("(%s) IOCTL failed. last error = 0x%08X\n"), temp.c_str(), GetLastError());
//
//                CloseHandle(device);
//            }
//            else
//                _tprintf(_T("open (%s) failed. last error = 0x%08X\n"), temp.c_str(), GetLastError());
//        }
//    }
//
//    return volume_list.size();
//}

size_t EnumVolumes(list<VOLUME_INFO>& volume_list)
{
    volume_list.clear();

    TCHAR buffer[BIG_BUFFER_SIZE] = { 0 };
    HANDLE find = FindFirstVolume(buffer, BIG_BUFFER_SIZE);
    BOOL find_ok = TRUE;

    if(INVALID_HANDLE_VALUE != find)
    {
        list<DRIVE_MOUNT_INFO> drive_list;
        EnumDriveMountInfo(drive_list);

        while(find_ok)
        {
            VOLUME_INFO info;
            //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
            info.VolumeName = buffer;

            //parse extents and mount path
            BOOL ok = BuildVolumeInfo(info);
            if(ok)
            {
                for(auto &drive : drive_list)
                {
                    if(info.VolumeName.compare(drive.MountVolume) == 0)
                        info.MountPointList.push_back(drive.DriveName);
                }

                volume_list.push_back(info);
            }

            RtlZeroMemory(buffer, BIG_BUFFER_SIZE * sizeof(TCHAR));
            find_ok = FindNextVolume(find, buffer, BIG_BUFFER_SIZE);
        }
        FindVolumeClose(find);
    }

    return volume_list.size();
}

//devpath => physical disk devpath
// e.g.  "\\.\PhysicalDrive4"
BOOL GetPartitionList(IN OUT BYTE* buffer, DWORD buf_size, tstring& devpath)
{
    DWORD ret_size = 0;
    BOOL ok = FALSE;
    HANDLE disk = CreateFile(devpath.c_str(), GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (INVALID_HANDLE_VALUE != disk)
    {
        //IOCTL_DISK_GET_DRIVE_LAYOUT_EX won't tell you "how big of buffer it need."
        // so we have to allocate a huge buffer which big enough to query...
        ok = DeviceIoControl(disk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, buffer, buf_size, &ret_size, NULL);

        if (!ok)
        {
            _tprintf(_T("IOCTL_DISK_GET_DRIVE_LAYOUT_EX failed, last error=%d\n"), GetLastError());
        }

        CloseHandle(disk);
    }
    else
        _tprintf(_T("Open physical disk [%s] failed. LastError=%d\n"), devpath.c_str(), GetLastError());

    return ok;
}