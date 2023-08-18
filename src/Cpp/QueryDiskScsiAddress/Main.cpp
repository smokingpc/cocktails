// QueryDiskScsiAddress.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <Setupapi.h>
#include <initguid.h>
#include <devpkey.h>
#include <devpropdef.h>
#include <Ntddscsi.h>
#include <stdio.h>
#include <string>
#include <list>

#define BIG_BUFFER_SIZE 4096
typedef std::basic_string<TCHAR> tstring;
using namespace std;
#pragma comment( lib, "setupapi.lib" )

typedef struct _DISK_BTL8_INFO {
    tstring InstanceId;
    tstring DevInf;
    SCSI_ADDRESS Btl8Addr;
}DISK_BTL8_INFO, *PDISK_BTL8_INFO;

size_t EnumDisksByInstanceId(list<DISK_BTL8_INFO*>& result)
{
    HDEVINFO infoset = NULL;
    GUID disk_class_guid = GUID_DEVINTERFACE_DISK;
    infoset = SetupDiGetClassDevs(&disk_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (INVALID_HANDLE_VALUE != infoset)
    {
        SP_DEVINFO_DATA infodata = { 0 };
        DWORD devid = 0;
        infodata.cbSize = sizeof(SP_DEVINFO_DATA);
        while (TRUE == SetupDiEnumDeviceInfo(infoset, devid++, &infodata))
        {
            BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
            BOOL ok = FALSE;
            DEVPROPTYPE type = 0;
            CONST DEVPROPKEY* key = NULL;
            DISK_BTL8_INFO* found = new DISK_BTL8_INFO;

            //In old windows, we should use CfgMgr API to retrieve these information from registry.
            //Since VISTA, all information of device and driver are unified to [Unified Device Property Model].
            //https://docs.microsoft.com/en-us/windows-hardware/drivers/install/unified-device-property-model--windows-vista-and-later-
            key = &DEVPKEY_Device_InstanceId;
            type = DEVPROP_TYPE_STRING;
            ok = SetupDiGetDeviceProperty(infoset, &infodata, key, &type, buffer, BIG_BUFFER_SIZE, NULL, 0);
            if(ok)
            {
                RtlZeroMemory(found, sizeof(DISK_BTL8_INFO));
                found->InstanceId = (TCHAR*)buffer;
            }
            else
                _tprintf(_T("***get InstanceId failed, LastError=%d!!\n"), GetLastError());

            SP_DEVICE_INTERFACE_DATA  ifdata = {0};
            ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            ok = SetupDiEnumDeviceInterfaces(infoset, &infodata, &disk_class_guid, 0, &ifdata);
            if(ok)
            {
                DWORD need_size = 0;
                RtlZeroMemory(buffer, BIG_BUFFER_SIZE);
                ((PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer)->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
                ok = SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, 
                            (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer, BIG_BUFFER_SIZE, &need_size, &infodata);
                if (ok)
                {
                    found->DevInf = (TCHAR*) ((PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer)->DevicePath;
                }
                else
                    _tprintf(_T("***SetupDiGetDeviceInterfaceDetail() failed, LastError=%d!!\n"), GetLastError());
            }
            else
                _tprintf(_T("***SetupDiEnumDeviceInterfaces() failed, LastError=%d!!\n"), GetLastError());

            if(!(found->InstanceId.empty() && found->DevInf.empty()))
                result.push_back(found);
        }
        SetupDiDestroyDeviceInfoList(infoset);
    }
    else
        _tprintf(_T("***SetupDiGetClassDevs() failed, LastError=%d!!\n"), GetLastError());

    return result.size();
}

void QueryDiskBtl8Address(DISK_BTL8_INFO* info)
{
    HANDLE device = CreateFile(info->DevInf.c_str(),
                        GENERIC_ALL, 0, NULL, OPEN_EXISTING, 0, NULL);
    if(INVALID_HANDLE_VALUE != device)
    {
        BOOL ok = FALSE;
        DWORD ret_size = 0;
        ok = DeviceIoControl(device, IOCTL_SCSI_GET_ADDRESS, NULL, 0, &info->Btl8Addr, 
                                sizeof(info->Btl8Addr), &ret_size, NULL);
        if(!ok)
            _tprintf(_T("***Failed to query address of %s!!\n"), info->InstanceId.c_str());
        CloseHandle(device);
    }
    else
        _tprintf(_T("***CreateFile() failed, LastError=%d!!\n"), GetLastError());
}

void PrintDiskInfo(DISK_BTL8_INFO* info)
{
    _tprintf(_T("Disk BTL8[%d.%d.%d]=>\n\t%s\n\t%s\n"), 
            info->Btl8Addr.PathId, info->Btl8Addr.TargetId, 
            info->Btl8Addr.Lun, info->DevInf.c_str(), info->InstanceId.c_str());
}

int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> disklist;
    list<DISK_BTL8_INFO*>infolist;
    size_t count = 0;

    count = EnumDisksByInstanceId(infolist);
    if(0 == count)
        _tprintf(_T("No disks found....\n"));

    for (PDISK_BTL8_INFO& item : infolist)
    {
        QueryDiskBtl8Address(item);
        PrintDiskInfo(item);
        delete item;
    }
}
