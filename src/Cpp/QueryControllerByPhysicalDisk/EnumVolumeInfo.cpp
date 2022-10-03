#include "Common.h"

const TCHAR* DriveLetters[] = {
    _T("A:"), _T("B:"), _T("C:"), _T("D:"), _T("E:"),
    _T("F:"), _T("G:"), _T("H:"), _T("I:"), _T("J:"),
    _T("K:"), _T("L:"), _T("M:"), _T("N:"), _T("O:"),
    _T("P:"), _T("Q:"), _T("R:"), _T("S:"), _T("T:"),
    _T("U:"), _T("V:"), _T("W:"), _T("X:"), _T("Y:"),
    _T("Z:")
};

//found volume interface name in result example =>
//  "\\?\storage#volume#{2904e7e5-d0c4-11eb-8357-806e6f6e6963}#00000076fda00000#{53f5630d-b6bf-11d0-94f2-00a0c91efb8b}"
static size_t EnumVolumeDevIfNames(list<tstring>& result)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id = 0;
    const GUID* class_guid = &GUID_DEVINTERFACE_VOLUME;

    DWORD flag = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT;
    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE == infoset)
        return 0;

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    id = 0;
    while (SetupDiEnumDeviceInfo(infoset, id++, &infodata))
    {
        ZeroMemory(&ifdata, sizeof(SP_DEVICE_INTERFACE_DATA));
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, class_guid, 0, &ifdata))
            continue;

        if ((ifdata.Flags & SPINT_ACTIVE) == 0)
            continue;

        PSP_DEVICE_INTERFACE_DETAIL_DATA detail;
        DWORD need_size = 0;

        SetupDiGetDeviceInterfaceDetail(
            infoset,
            &ifdata,
            NULL,
            0,
            &need_size,
            NULL);

        detail = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new BYTE[need_size];
        if (NULL == detail)
            continue;

        ZeroMemory(detail, need_size);
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(
            infoset, &ifdata, detail, need_size,
            NULL, NULL))
            continue;

        tstring temp = detail->DevicePath;
        std::transform(temp.begin(), temp.end(), temp.begin(), tcsupper);

        result.push_back(temp);
        delete[] detail;
    }

    SetupDiDestroyDeviceInfoList(infoset);
    return result.size();
}
static BOOL IsVolumeReady(tstring vol_name)
{
    BOOL ret = FALSE;

    HANDLE device = CreateFile(vol_name.c_str(), 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (device != INVALID_HANDLE_VALUE)
    {
        ret = TRUE;
        if (GetLastError() == ERROR_NOT_READY)
            ret = FALSE;
        CloseHandle(device);
    }

    return ret;
}
//iterate all drives (C:, D:, E: .....etc) and check their mount volumes
static void EnumDriveMountInfo(list<DRIVE_MOUNT_INFO>& mount_list)
{
    for (const TCHAR* drive : DriveLetters)
    {
        TCHAR buffer[SMALL_BUFFER_SIZE] = { 0 };
        tstring drive_name = drive;
        drive_name += _T("\\");
        BOOL ok = GetVolumeNameForVolumeMountPoint(drive_name.c_str(), buffer, SMALL_BUFFER_SIZE);
        if (ok)
        {
            TCHAR buffer2[TINY_BUFFER_SIZE] = { 0 };
            _stprintf_s(buffer2, TINY_BUFFER_SIZE, DOSDRIVE_FORMAT, drive);

            DRIVE_MOUNT_INFO info;
            info.DriveName = buffer2;
            info.MountVolume = buffer;
            std::transform(info.DriveName.begin(), info.DriveName.end(), info.DriveName.begin(), tcsupper);
            std::transform(info.MountVolume.begin(), info.MountVolume.end(), info.MountVolume.begin(), tcsupper);

            mount_list.push_back(info);
        }
    }
}

static void QueryVolumeMountFolders(tstring vol_name, list<tstring>& mount_list)
{
    TCHAR buffer[BIG_BUFFER_SIZE] = { 0 };
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

static BOOL BuildVolumeInfo(VOLUME_INFO& info, list<DRIVE_MOUNT_INFO>& drive_list)
{
    //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
    //to open volume, back-slash on tail should be removed. or the IOCTL will always return lasterror 0x57
    tstring temp = info.VolumeName;
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
        //TODO: CDROM and other readonly device could get error in this IOCTL.
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

            //Ask this volume : "where is your mount points ?" and get all mount dir.
            QueryVolumeMountFolders(info.VolumeName, info.MountPointList);
            for (auto& drive : drive_list)
            {
                if (info.VolumeName.compare(drive.MountVolume) == 0)
                    info.MountPointList.push_back(drive.DriveName);
            }
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

//volume name example:  "\\?\Volume{8b035f6c-d8c1-4671-b2c2-69d4f75f6787}\"
size_t EnumVolumeInfo(list<VOLUME_INFO>& result)
{
    result.clear();

    TCHAR buffer[BIG_BUFFER_SIZE] = { 0 };
    HANDLE find = FindFirstVolume(buffer, BIG_BUFFER_SIZE);
    BOOL find_ok = TRUE;
    list<DRIVE_MOUNT_INFO> drive_list;
    EnumDriveMountInfo(drive_list);

    if (INVALID_HANDLE_VALUE != find)
    {
        while (find_ok)
        {
            VOLUME_INFO info;
            //volume name example:  "\\?\Volume{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}\"
            info.VolumeName = buffer;
            std::transform(info.VolumeName.begin(), info.VolumeName.end(), info.VolumeName.begin(), tcsupper);

            //parse extents and mount path
            BOOL ok = BuildVolumeInfo(info, drive_list);
            if (ok)
            {
                info.IsReady = IsVolumeReady(info.VolumeName);
                result.push_back(info);
            }

            RtlZeroMemory(buffer, BIG_BUFFER_SIZE * sizeof(TCHAR));
            find_ok = FindNextVolume(find, buffer, BIG_BUFFER_SIZE);
        }
        FindVolumeClose(find);
    }

    return result.size();
}

#if 0
//vol_name is DeviceInterface name of specified volume, not regular volume name in disk manager.
BOOL IsVolumeMounted(tstring vol_name)
{
    DWORD ret_size = 0;
    HANDLE device = CreateFile(vol_name.c_str(), 0, 
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if(INVALID_HANDLE_VALUE == device)
        return FALSE;

    BOOL ok = DeviceIoControl(
        device,             // handle to device
        FSCTL_IS_VOLUME_MOUNTED,      // dwIoControlCode
        NULL,                         // lpInBuffer
        0,                            // nInBufferSize
        NULL,                         // lpOutBuffer
        0,                            // nOutBufferSize
        &ret_size,    // number of bytes returned
        NULL   // OVERLAPPED structure
    );
    CloseHandle(device);

    return !ok; 
}
#endif 

size_t EnumVolume(list<tstring> &result)
{
    HDEVINFO devinfo = NULL;
    GUID disk_class_guid = GUID_DEVINTERFACE_VOLUME;
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
                tstring temp = ifdetail->DevicePath;
                result.push_back(temp);
            }
        }
    }

    return result.size();
}

//
//size_t EnumVolumeInfo(list<VOLUME_INFO> &result)
//{
//    list<tstring> volume_list;
//    list<DRIVE_MOUNT_INFO> mount_list;
//
//    EnumDriveMountInfo(mount_list);
//    EnumVolumes(volume_list);   //here get volume names, not device interface name of volumes.
//    for(auto &volume : volume_list)
//    {
//        
//    }
//}