
#include <windows.h>
#include <SetupAPI.h>
#include <iostream>
#include <initguid.h>

#include <string>
#include <list>
using namespace std;
//it can be device interface class guid of WdfDeviceCreateDeviceInterface()
DEFINE_GUID(GUID_TEST_GUID,
    0x952826e2, 0xf467, 0x4f78, 0xbb, 0x22, 0xa3, 0xef, 0xc8, 0x6c, 0x4a, 0x1d);
// {952826e2-f467-4f78-bb22-a3efc86c4a1d}

size_t EnumerateDeviceInterface(list<wstring> &result, const GUID* class_guid)
{
    HDEVINFO infoset;
    SP_DEVINFO_DATA infodata;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id;

    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE);

    ZeroMemory(&infodata, sizeof(SP_DEVINFO_DATA));
    infodata.cbSize = sizeof(SP_DEVINFO_DATA);
    id = 0;

    while (SetupDiEnumDeviceInfo(infoset, id++, &infodata))
    {
        ZeroMemory(&ifdata, sizeof(SP_DEVICE_INTERFACE_DATA));
        ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        wprintf(L"[Enum Next DeviceInterface]\n");

        if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, class_guid, 0, &ifdata)) 
        {
            wprintf(L"Enum DeviceInterface failed(error=%d), try next device...\n", GetLastError());
            continue;
        }

        if ((ifdata.Flags & SPINT_ACTIVE) == 0) 
        {
            wprintf(L"Got interface[0] but inactive => Flag=0x%08X", ifdata.Flags);
            //continue;
        }

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
        {
            wprintf(L"Allocate Buffer Failed, try next device...\n");
            continue;
        }
        //wprintf(L"required buffer size = %d\n", need_size);

        ZeroMemory(detail, need_size);
        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(
                infoset, &ifdata, detail, need_size,
                NULL, NULL)) 
        {
            wprintf(L"SetupDiGetDeviceInterfaceDetail Failed, error=%d...\n", GetLastError());
            continue;
        }

        wprintf(L"found path => %s\n", detail->DevicePath);
        result.push_back(detail->DevicePath);
        delete[] detail;
    }

    return result.size();
}
size_t EnumerateDeviceInterface2(list<wstring>& result, const GUID* class_guid)
{
    HDEVINFO infoset;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id = 0;

    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    wprintf(L"==> SetupDiGetClassDevsW\n");
    if(infoset == INVALID_HANDLE_VALUE)
        goto END;

    wprintf(L"==> SetupDiEnumDeviceInterfaces\n");
    ZeroMemory(&ifdata, sizeof(SP_DEVICE_INTERFACE_DATA));
    ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    while (SetupDiEnumDeviceInterfaces(infoset, NULL, class_guid, id++, &ifdata))
    {
        BYTE buffer[1024] = {0};
        PSP_DEVICE_INTERFACE_DETAIL_DATA detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;

        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetail(
            infoset, &ifdata, detail, 1024, NULL, NULL))
        {
            wprintf(L"SetupDiGetDeviceInterfaceDetail Failed, error=%d...\n", GetLastError());
            continue;
        }

        wprintf(L"Got interface{%s} Flag=0x%08X\n", detail->DevicePath, ifdata.Flags);
        result.push_back(detail->DevicePath);
    }
    wprintf(L"==> SetupDiEnumDeviceInterfaces loop done, lasterror=%d\n", GetLastError());

END:
    if((NULL != infoset) && (infoset != INVALID_HANDLE_VALUE))
        SetupDiDestroyDeviceInfoList(infoset);
    return result.size();
}

int main()
{
    list<wstring> list;

    size_t count = EnumerateDeviceInterface2(list, &GUID_DEVINTERFACE_STORAGEPORT);
    if (0 == count)
    {
        wprintf(L"no interface found, exit...\n\n");
        return -1;
    }

#if 0
    //size_t count = EnumerateDeviceInterface(list, &GUID_TEST_GUID);
    //size_t count = EnumerateDeviceInterface(list, &GUID_DEVINTERFACE_DISK);
    size_t count = EnumerateDeviceInterface(list, &GUID_DEVINTERFACE_STORAGEPORT);
    if(0==count)
    {
        wprintf(L"no interface found, exit...\n\n");
        return -1;
    }

    HANDLE hdevice = INVALID_HANDLE_VALUE;
    for (const auto& path : list) 
    {
        hdevice = CreateFile(path.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL);

        bool ok = (hdevice != INVALID_HANDLE_VALUE);
        wprintf(L"open device [%s][%d] for [%s]\n", (ok==true?L"ok":L"failed"), GetLastError(), path.c_str());
    }
#endif
    wprintf(L"exit.....\n");
}
