// QueryControllerByPhysicalDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

static bool QueryParentController(tstring phydisk, tstring &ctrl)
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
    if (!SetupDiOpenDeviceInterface(infoset, phydisk.c_str(), 0, &devif))
    {
        error = GetLastError();
        goto out;
    }

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiGetDeviceInterfaceDetail(infoset, &devif, NULL, 0, &need_size, &infodata)) 
    {
        error = GetLastError();
        //if (error != ERROR_INSUFFICIENT_BUFFER) 
        //{
            goto out;
        //}
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

static size_t EnumVolumes(list<tstring> &vol_list)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id = 0;
    const GUID* class_guid = GUID_DEVINTERFACE_VOLUME;

    DWORD flag = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT;
    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE);

    
    return vol_list.size();
}

//devpath example : "\\.\PhysicalDrive4"
static size_t EnumPhysicalDisks(list<tstring>& devpath_list)
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


int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> disklist;
    EnumPhysicalDisks(disklist);
    for(auto &disk : disklist)
    {
        tstring ctrl;
        QueryParentController(disk, ctrl);
        _tprintf(_T("disk (%s) => parent (%s)\n"), disk.c_str(), ctrl.c_str());
    }

    return 0;
}

