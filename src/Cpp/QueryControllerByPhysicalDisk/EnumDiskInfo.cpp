#include "Common.h"

const unsigned char upper_diff = (_T('z') - _T('Z'));
TCHAR tcsupper(TCHAR in)
{
    if (in <= _T('z') && in >= _T('a'))
        return in - upper_diff;
    return in;
}

//disk => dev interface of disk
static ULONG GetPhysicalDiskNumber(tstring &disk)
{
    DWORD return_size = 0;
    HANDLE device = CreateFile(disk.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE != device)
    {
        STORAGE_DEVICE_NUMBER disk_number = { 0 };
        return_size = 0;
        BOOL ok = DeviceIoControl(device,
            IOCTL_STORAGE_GET_DEVICE_NUMBER,
            NULL,
            0,
            &disk_number,
            sizeof(STORAGE_DEVICE_NUMBER),
            &return_size,
            NULL);

        CloseHandle(device);

        if (TRUE == ok)
            return disk_number.DeviceNumber;
    }

    return (ULONG)-1;
}
static void GetPhyDiskName(tstring &devpath, tstring &phydisk)
{
    ULONG disk_number = GetPhysicalDiskNumber(devpath);
    if(disk_number != (ULONG)-1)
    {
        TCHAR buffer[TINY_BUFFER_SIZE] = {0};
        _stprintf_s(buffer, TINY_BUFFER_SIZE, PHYSICAL_DISK_FORMAT, disk_number);
        phydisk = buffer;
    }
}
static size_t EnumDisks(list<tstring>& result)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id = 0;
    const GUID* class_guid = &GUID_DEVINTERFACE_DISK;

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

        result.push_back(detail->DevicePath);
        delete[] detail;
    }

    SetupDiDestroyDeviceInfoList(infoset);
    return result.size();
}

static void GetStorportDevPathByLocation(tstring& inst_path, tstring& devpath)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id;
    const GUID* class_guid = &GUID_DEVINTERFACE_STORAGEPORT;
    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (INVALID_HANDLE_VALUE == infoset)
        return;

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    id = 0;

    while (SetupDiEnumDeviceInfo(infoset, id++, &infodata))
    {
        const DEVPROPKEY* key = &DEVPKEY_Device_InstanceId;
        DEVPROPTYPE type = 0;
        BYTE buffer[BIG_BUFFER_SIZE] = { 0 };

        if (!SetupDiGetDeviceProperty(
            infoset, &infodata, key, &type,
            (BYTE*)buffer, BIG_BUFFER_SIZE, NULL, 0) || type != DEVPROP_TYPE_STRING) {
            goto end;
        }

        tstring path = (TCHAR*)buffer;
        std::transform(path.begin(), path.end(), path.begin(), tcsupper);
        std::transform(inst_path.begin(), inst_path.end(), inst_path.begin(), tcsupper);

        if(0 != path.compare(inst_path))
            continue;

        ZeroMemory(&ifdata, sizeof(SP_DEVICE_INTERFACE_DATA));
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        wprintf(L"[Enum Next DeviceInterface]\n");

        if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, class_guid, 0, &ifdata))
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

        //BYTE buffer[BIG_BUFFER_SIZE] = {0};
        RtlZeroMemory(buffer, BIG_BUFFER_SIZE);
        if (!SetupDiGetDeviceInterfaceDetail(
            infoset, &ifdata, detail, need_size,
            NULL, NULL))
        {
            continue;
        }

        devpath = detail->DevicePath;
        delete[] detail;
    }


end:
    if (infoset != NULL && infoset != INVALID_HANDLE_VALUE)
        SetupDiDestroyDeviceInfoList(infoset);
}

//ctrl will return data like this:  "PCI\VEN_1C5C&DEV_174A&SUBSYS_174A1C5C&REV_00\4&2ef10b15&0&0030"
static bool GetParentLocation(tstring &disk_devpath, tstring& ctrl)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA devif;

    DWORD need_size = 0;
    bool ret = false;
    DWORD error = 0;
    DEVPROPTYPE type = 0;
    WCHAR buffer[BIG_BUFFER_SIZE] = { 0 };

    infoset = SetupDiCreateDeviceInfoList(NULL, NULL);
    if (INVALID_HANDLE_VALUE == infoset)
    {
        error = GetLastError();
        goto out;
    }

    ZeroMemory(&devif, sizeof(SP_DEVICE_INTERFACE_DATA));
    devif.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (!SetupDiOpenDeviceInterface(infoset, disk_devpath.c_str(), 0, &devif))
    {
        error = GetLastError();
        goto out;
    }

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiGetDeviceInterfaceDetail(infoset, &devif, NULL, 0, &need_size, &infodata))
    {
        error = GetLastError();
        if (error != ERROR_INSUFFICIENT_BUFFER)
            goto out;
    }

    if (!SetupDiGetDeviceProperty(
        infoset,
        &infodata,
        &DEVPKEY_Device_Parent,
        &type,
        (BYTE*)buffer,
        sizeof(buffer),
        NULL, 0) || type != DEVPROP_TYPE_STRING) {

        error = GetLastError();
        goto out;
    }

    ctrl = buffer;

    SetupDiDeleteDeviceInterfaceData(infoset, &devif);
    error = 0;
    ret = true;

out:
    if (error != 0)
        _tprintf(L"error occurred, LastError=0x%08X\n", error);
    if (infoset != NULL && infoset != INVALID_HANDLE_VALUE)
        SetupDiDestroyDeviceInfoList(infoset);
    return ret;
}


//devpath example : "\\.\PhysicalDrive4"
size_t EnumPhysicalDisks(list<tstring>& devpath_list)
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

                    TCHAR temp[TINY_BUFFER_SIZE] = { 0 };
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

size_t EnumDiskInfo(list<DISK_INFO> &result)
{
    list<tstring> disklist;
    size_t count = EnumDisks(disklist);

    result.clear();
    for(auto &disk : disklist)
    {
        DISK_INFO info;
        info.DevPath = disk;
        
        GetPhyDiskName(disk, info.PhyDiskName);
        tstring parent;
        GetParentLocation(disk, parent);
        GetStorportDevPathByLocation(parent, info.CtrlDevPath);
        result.push_back(info);
    }

    return result.size();
}
