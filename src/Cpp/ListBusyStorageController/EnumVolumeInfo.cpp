#include "Common.h"

const TCHAR* DriveLetters[] = {
    _T("A:"), _T("B:"), _T("C:"), _T("D:"), _T("E:"),
    _T("F:"), _T("G:"), _T("H:"), _T("I:"), _T("J:"),
    _T("K:"), _T("L:"), _T("M:"), _T("N:"), _T("O:"),
    _T("P:"), _T("Q:"), _T("R:"), _T("S:"), _T("T:"),
    _T("U:"), _T("V:"), _T("W:"), _T("X:"), _T("Y:"),
    _T("Z:")
};

//query the Mounted Folder Path of specified volume name.
//e.g.  "C:\abc\temp" mapped from "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
//**Note: the path use normal Win32 path format.
static void QueryVolumeMountFolders(tstring volume, list<tstring>& result)
{
    TCHAR buffer[BIG_BUFFER_SIZE] = { 0 };
    tstring temp = volume;
    if (temp.back() != _T('\\'))
        temp += _T('\\');

    HANDLE find = FindFirstVolumeMountPoint(temp.c_str(), buffer, BIG_BUFFER_SIZE);
    BOOL find_ok = TRUE;

    if (INVALID_HANDLE_VALUE != find)
    {
        while (find_ok)
        {
            result.push_back(buffer);
            RtlZeroMemory(buffer, sizeof(buffer));
            find_ok = FindNextVolumeMountPoint(find, buffer, BIG_BUFFER_SIZE);
        }
        FindVolumeMountPointClose(find);
    }

}

//query  volume name(DOSNAME) to drive name mapping list
//e.g.  "C:" mapped from "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
static void EnumDriveMountInfo(list< DRIVE_MOUNT_INFO> result)
{
    for (const TCHAR* drive : DriveLetters)
    {
        TCHAR buffer[SMALL_BUFFER_SIZE] = { 0 };
        tstring drive_name = drive;
        tstring volume_name = _T("");
        drive_name += _T("\\");
        BOOL ok = GetVolumeNameForVolumeMountPoint(drive_name.c_str(), buffer, SMALL_BUFFER_SIZE);
        if (ok)
        {
            TCHAR buffer2[TINY_BUFFER_SIZE] = { 0 };
            _stprintf_s(buffer2, TINY_BUFFER_SIZE, DOSDRIVE_FORMAT, drive);
            volume_name = buffer2;
            DRIVE_MOUNT_INFO info;
            info.DriveName = drive_name;
            info.VolumeName = volume_name;
            result.push_back(info);
        }
    }
}

//query the Mounted Drive Name by specified volume name.
//volume name example:"\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
//drive name example: "\\?\C:\"
static void QueryDriveMountOfVolume(tstring volume, list<tstring> &result)
{
    list<DRIVE_MOUNT_INFO> drive_list;
    EnumDriveMountInfo(drive_list);

    for(auto &drive : drive_list)
    {
        if(0 == StrCompare(drive.VolumeName, volume))
            result.push_back(drive.DriveName);
    }
}

//DiskExtent means "Where is the volume placed?"
//One Volume could split into multiple physical disks.
static void QueryDiskExtent(tstring &volume, list<tstring> &disklist)
{
    //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
    //to open volume, back-slash on tail should be removed. or the IOCTL will always return lasterror 0x57
    tstring temp = volume;
    if (temp.back() == _T('\\'))
        temp.pop_back();        //remove back-slash on string tail. 

    HANDLE vol_handle = CreateFile(temp.c_str(), 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (INVALID_HANDLE_VALUE != vol_handle)
    {
        DWORD ret_size = 0;
        BYTE buffer[BIG_BUFFER_SIZE] = { 0 };

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
                disklist.push_back(buffer2);
            }
        }
        else
        {
            _tprintf(_T("(%s) IOCTL failed. last error = 0x%08X\n"), temp.c_str(), GetLastError());
        }
        CloseHandle(vol_handle);
    }
    else
    {
        _tprintf(_T("open (%s) failed. last error = 0x%08X\n"), temp.c_str(), GetLastError());
    }
}

size_t EnumVolumes(list<VOLUME_INFO> &result)
{
    TCHAR buffer[BIG_BUFFER_SIZE] = { 0 };
    //FindFirstVolume()/FindNextVolume() can enum all volumes in this system.
    //even the volume is empty.....
    HANDLE find = FindFirstVolume(buffer, BIG_BUFFER_SIZE);
    BOOL find_ok = TRUE;

    if (INVALID_HANDLE_VALUE != find)
    {
        while (find_ok)
        {
            VOLUME_INFO info;
            //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
            info.VolumeName = buffer;
            QueryDriveMountOfVolume(info.VolumeName, info.MountPointList);
            QueryVolumeMountFolders(info.VolumeName, info.MountPointList);
            QueryDiskExtent(info.VolumeName, info.PhyDisks);
            result.push_back(info);

            RtlZeroMemory(buffer, BIG_BUFFER_SIZE * sizeof(TCHAR));
            find_ok = FindNextVolume(find, buffer, BIG_BUFFER_SIZE);
        }
        FindVolumeClose(find);
    }

    return result.size();
}


//size_t EnumPhysicalDisks(list<PHYDISK_INFO> &result)
//{}
//
//size_t EnumMountedPhysicalDisks(list<tstring> disk_list)
//{
//    HDEVINFO infoset = NULL;
//    GUID disk_class_guid = GUID_DEVINTERFACE_DISK;
//    GUID* debug = &disk_class_guid;
//    infoset = SetupDiGetClassDevs(&disk_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
//
//    if (INVALID_HANDLE_VALUE != infoset)
//    {
//        SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
//        SP_DEVINFO_DATA infodata = { 0 };
//        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
//        DWORD devid = 0;
//        while (TRUE == SetupDiEnumDeviceInterfaces(infoset, &infodata, &disk_class_guid, devid, &ifdata))
//        {
//            DWORD need_size = 0;
//            DWORD return_size = 0;
//            BOOL ok = FALSE;
//            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
//            devid++;
//            SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, NULL, 0, &need_size, NULL);
//            need_size = need_size * 2;
//            BYTE* buffer = new BYTE[need_size];
//            unique_ptr<BYTE> bufptr(buffer);
//            ZeroMemory(buffer, need_size);
//
//            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
//            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
//            ok = SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, ifdetail, need_size, &need_size, NULL);
//            if (TRUE == ok)
//            {
//                HANDLE device = CreateFile(ifdetail->DevicePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
//                    NULL, OPEN_EXISTING, 0, NULL);
//                if (INVALID_HANDLE_VALUE != device)
//                {
//                    STORAGE_DEVICE_NUMBER disk_number = { 0 };
//                    return_size = 0;
//                    ok = DeviceIoControl(device,
//                        IOCTL_STORAGE_GET_DEVICE_NUMBER,
//                        NULL,
//                        0,
//                        &disk_number,
//                        sizeof(STORAGE_DEVICE_NUMBER),
//                        &return_size,
//                        NULL);
//                    CloseHandle(device);
//
//                    PHYDISK_INFO diskinfo;
//                    TCHAR temp[TINY_BUFFER_SIZE] = { 0 };
//                    {
//                        _stprintf_s(temp, TINY_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_number.DeviceNumber);
//                        diskinfo.PhyDisk = temp;
//                    }
//                    if (QueryParentController(diskinfo, infoset, &infodata))
//                        disk_list.push_back(diskinfo);
//                    else
//                        _tprintf(_T("query [%s] parent controller failed. error=%d\n"), diskinfo.PhyDisk.c_str(), GetLastError());
//                }
//                else
//                    printf("CreateFile() failed, LastError=%d\n", GetLastError());
//            }
//            else
//                printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
//        }
//    }
//
//    printf("EnumPhysicalDisk() Finished....\n");
//
//    return disk_list.size();
//}
