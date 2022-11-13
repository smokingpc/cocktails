#include "Common.h"
#pragma comment( lib, "setupapi.lib" )

size_t EnumStorageControllers(list<tstring> &result)
{
    HDEVINFO infoset = NULL;
    GUID disk_class_guid = GUID_DEVINTERFACE_STORAGEPORT;
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
            ZeroMemory(buffer, need_size);

            SP_DEVINFO_DATA infodata = { 0 };
            infodata.cbSize = sizeof(SP_DEVINFO_DATA);

            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            //ok = SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, ifdetail, need_size, &need_size, NULL);
            ok = SetupDiGetDeviceInterfaceDetail(infoset, &ifdata, ifdetail, need_size, &need_size, &infodata);
            if (TRUE == ok)
            {
                result.push_back(ifdetail->DevicePath);
            }
            else
                printf("SetupDiGetDeviceInterfaceDetail() failed, LastError=%d\n", GetLastError());
            delete[] buffer;
        }
    }
       return result.size();
}