#include "Common.h"

static BOOL IsPhyDiskBusy(tstring phydisk, list<VOLUME_INFO>& vol_list)
{
    for (auto& volume : vol_list)
    {
        //If a controller has physical disk which contains a volume, this controller is busy.
        if (volume.IsDiskHasThisVolume(phydisk))// && volume.IsVolumeMounted())
            return TRUE;
    }
    return FALSE;
}
static BOOL QueryDevInterfaceByInstanceId(OUT tstring& devpath, IN tstring& instance_id)
{
    HDEVINFO infoset;
    tstring enum_str = _T("");
    DWORD flag = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT;
    DEVPROPTYPE type = DEVPROP_TYPE_STRING;
    BOOL found = FALSE;
    infoset = SetupDiGetClassDevs(&GUID_DEVINTERFACE_STORAGEPORT, NULL, NULL, flag);

    if (INVALID_HANDLE_VALUE != infoset && NULL != infoset)
    {
        DWORD id = 0;
        tstring found_devif = _T("");
        SP_DEVINFO_DATA infodata = { 0 };
        infodata.cbSize = sizeof(SP_DEVINFO_DATA);
        while (SetupDiEnumDeviceInfo(infoset, id, &infodata))
        {
            id++;
            DEVPROPTYPE type = 0;
            tstring found_hwid = _T("");

            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
            SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
            BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
            ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, &GUID_DEVINTERFACE_STORAGEPORT, 0, &ifdata))
                continue;

            BOOL ok = SetupDiGetDeviceProperty(infoset, &infodata, &DEVPKEY_Device_InstanceId,
                &type, buffer, BIG_BUFFER_SIZE, NULL, 0);

            if (!ok)
                continue;
            found_hwid = (wchar_t*)buffer;

            std::transform(instance_id.begin(), instance_id.end(), instance_id.begin(), tcsupper);
            std::transform(found_hwid.begin(), found_hwid.end(), found_hwid.begin(), tcsupper);
            if (0 != instance_id.compare(found_hwid))
                continue;

            RtlZeroMemory(buffer, BIG_BUFFER_SIZE);
            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            if (!SetupDiGetDeviceInterfaceDetail(
                infoset, &ifdata, ifdetail, BIG_BUFFER_SIZE, NULL, NULL))
                continue;

            devpath = ifdetail->DevicePath;
            found = TRUE;
            break;
        }
        SetupDiDestroyDeviceInfoList(infoset);
    }

    return found;
}

static BOOL QueryParentController(OUT PHYDISK_INFO& info, IN HDEVINFO infoset, IN PSP_DEVINFO_DATA infodata)
{
    DEVPROPTYPE type = DEVPROP_TYPE_STRING;
    BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
    BOOL ok = SetupDiGetDeviceProperty(infoset, infodata, &DEVPKEY_Device_Parent,
        &type, buffer, BIG_BUFFER_SIZE, NULL, 0);
    if (!ok)
    {
        DWORD error = GetLastError();
        //result = _T("");
        return FALSE;
    }

    info.ParentInstanceID = (wchar_t*)buffer;
    tstring devpath;
    if (QueryDevInterfaceByInstanceId(info.ParentDevPath, info.ParentInstanceID))
        return TRUE;

    return FALSE;
}

BOOL EnumControllers(OUT list<CONTROLLER_INFO>& busy_list, OUT list<CONTROLLER_INFO>& free_list,
    OUT list<PHYDISK_INFO>& disk_list, IN list<VOLUME_INFO>& vol_list)
{
    HDEVINFO infoset = NULL;
    GUID disk_class_guid = GUID_DEVINTERFACE_DISK;
    GUID* debug = &disk_class_guid;
    infoset = SetupDiGetClassDevs(&disk_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    
    if (INVALID_HANDLE_VALUE != infoset)
    {
        SP_DEVICE_INTERFACE_DATA ifdata = { 0 };
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        DWORD devid = 0;
        while (TRUE == SetupDiEnumDeviceInterfaces(infoset, NULL, &disk_class_guid, devid, &ifdata))
        //while (TRUE == SetupDiEnumDeviceInfo(infoset, devid, &infodata))
        {
            DWORD need_size = 0;
            DWORD return_size = 0;
            BOOL ok = FALSE;
            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
            devid++;

            //if(!SetupDiEnumDeviceInterfaces(infoset, &infodata, &disk_class_guid, 0, &ifdata))
            //    continue;

            SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, NULL, 0, &need_size, NULL);
            need_size = need_size * 2;
            BYTE* buffer = new BYTE[need_size];
            unique_ptr<BYTE> bufptr(buffer);
            ZeroMemory(buffer, need_size);

            SP_DEVINFO_DATA infodata = { 0 };
            infodata.cbSize = sizeof(SP_DEVINFO_DATA);

            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            //ok = SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, ifdetail, need_size, &need_size, NULL);
            ok = SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, ifdetail, need_size, &need_size, &infodata);
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
    
                    PHYDISK_INFO diskinfo;
                    TCHAR temp[TINY_BUFFER_SIZE] = { 0 };
                    {
                        _stprintf_s(temp, TINY_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_number.DeviceNumber);
                        diskinfo.PhyDisk = temp;
                    }
                    if (QueryParentController(diskinfo, infoset, &infodata))
                    {
                        disk_list.push_back(diskinfo);

                        CONTROLLER_INFO ctrlinfo;
                        ctrlinfo.DevPath = diskinfo.ParentDevPath;
                        ctrlinfo.InstanceId = diskinfo.ParentInstanceID;
                        
                        //If a controller has physical disk which contains a volume, this controller is busy.
                        if (IsPhyDiskBusy(diskinfo.PhyDisk, vol_list))
                        {
                            ctrlinfo.IsBusy = true;
                            busy_list.push_back(ctrlinfo);
                        }
                        else
                        {
                            ctrlinfo.IsBusy = false;
                            free_list.push_back(ctrlinfo);
                        }
                    }
                    else
                        _tprintf(_T("query [%s] parent controller failed. error=%d\n"), diskinfo.PhyDisk.c_str(), GetLastError());
                }
                else
                    printf("CreateFile() failed, LastError=%d\n", GetLastError());
            }
            else
                printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
        }
    }
    
    printf("EnumPhysicalDisk() Finished....\n");
    if(busy_list.size() == 0 && free_list.size() == 0)
        return FALSE;
    return TRUE;
}

