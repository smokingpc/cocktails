// IdentifyStorNvmeController.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <string>
#include <list>
#include <SetupAPI.h>
#include <iostream>
#include <initguid.h>
#include <ntddstor.h>
#include <nvme.h>

#define PAGE_SIZE   4096
#define QUERY_BUF_SIZE  (PAGE_SIZE*2)
using std::list;
typedef std::basic_string<TCHAR> tstring;
#pragma comment(lib, "setupapi.lib")

size_t EnumStorageControllers(list<tstring> &result)
{
    HDEVINFO infoset;
    SP_DEVICE_INTERFACE_DATA ifdata;
    DWORD id = 0;
    const GUID* class_guid = &GUID_DEVINTERFACE_STORAGEPORT;

    infoset = SetupDiGetClassDevsW(
        class_guid,
        NULL,
        NULL,
        DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);

    if (infoset == INVALID_HANDLE_VALUE)
        goto END;

    ZeroMemory(&ifdata, sizeof(SP_DEVICE_INTERFACE_DATA));
    ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    while (SetupDiEnumDeviceInterfaces(infoset, NULL, class_guid, id++, &ifdata))
    {
        BYTE buffer[1024] = { 0 };
        PSP_DEVICE_INTERFACE_DETAIL_DATA detail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)buffer;

        detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetail(
            infoset, &ifdata, detail, 1024, NULL, NULL))
        {
            continue;
        }

        result.push_back(detail->DevicePath);
    }

END:
    if ((NULL != infoset) && (infoset != INVALID_HANDLE_VALUE))
        SetupDiDestroyDeviceInfoList(infoset);
    return result.size();
}

bool IdentifyNvmeController(tstring devif, PNVME_IDENTIFY_CONTROLLER_DATA ident)
{
    BOOL ok = FALSE;
    bool ret = false;
    DWORD ret_size = 0;
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR descriptor = NULL;
    PUCHAR buffer = NULL;
    PSTORAGE_PROPERTY_QUERY query = NULL;
    PSTORAGE_PROTOCOL_SPECIFIC_DATA data = NULL;
    HANDLE device = CreateFile(devif.c_str(), 
                    GENERIC_READ | GENERIC_WRITE, 
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    NULL, OPEN_EXISTING, 
                    FILE_ATTRIBUTE_NORMAL, NULL);

    if (device == INVALID_HANDLE_VALUE)
        goto END;

    //use larger buffer to prevent buffer overflow.
    buffer = new UCHAR[QUERY_BUF_SIZE];
    query = (PSTORAGE_PROPERTY_QUERY)buffer;
    RtlZeroMemory(buffer, QUERY_BUF_SIZE);

    query->PropertyId = STORAGE_PROPERTY_ID::StorageAdapterProtocolSpecificProperty;
    query->QueryType = STORAGE_QUERY_TYPE::PropertyStandardQuery;
    data = (PSTORAGE_PROTOCOL_SPECIFIC_DATA) query->AdditionalParameters;

    data->ProtocolType = STORAGE_PROTOCOL_TYPE::ProtocolTypeNvme;
    data->DataType = STORAGE_PROTOCOL_NVME_DATA_TYPE::NVMeDataTypeIdentify;
    data->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CODES::NVME_IDENTIFY_CNS_CONTROLLER;
    data->ProtocolDataRequestSubValue = 0;  //unused.
    data->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);  //return data followed after STORAGE_PROTOCOL_SPECIFIC_DATA structure
    data->ProtocolDataLength = sizeof(NVME_IDENTIFY_CONTROLLER_DATA);

    ok = DeviceIoControl(device, IOCTL_STORAGE_QUERY_PROPERTY, 
                    buffer, QUERY_BUF_SIZE,
                    buffer, QUERY_BUF_SIZE,
                    &ret_size, NULL);
    if(!ok)
    {
        _tprintf(_T("DeviceIoControl() failed. LastError=%d\n"), GetLastError());
        goto END;
    }

    //returned data overrite entire out buffer, in PSTORAGE_PROTOCOL_DATA_DESCRIPTOR type.
    descriptor = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
    if ((descriptor->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
        (descriptor->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR))) {
        _tprintf(_T("IdentifyController got invalid header of protocol data descriptor.\n"));
        goto END;
    }

    data = &descriptor->ProtocolSpecificData;
    if ((data->ProtocolDataOffset < sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA)) ||
        (data->ProtocolDataLength < sizeof(NVME_IDENTIFY_CONTROLLER_DATA))) {
        _tprintf(_T("ProtocolSpecificData got invalid identify controller data size and offset.\n"));
        goto END;
    }

    ret = true;
    //returned identify controller data is followed AFTER STORAGE_PROTOCOL_SPECIFIC_DATA structure.
    RtlCopyMemory(ident, ((PUCHAR)data+data->ProtocolDataOffset), sizeof(NVME_IDENTIFY_CONTROLLER_DATA));
END:
    if(device != INVALID_HANDLE_VALUE)
        CloseHandle(device);

    if(NULL != buffer)
        delete[] buffer;

    return ret;
}

int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> ctrl_list;
    EnumStorageControllers(ctrl_list);

    for(tstring &devif : ctrl_list)
    {
        _tprintf(_T("Found Controller=> %s\n"), devif.c_str());
        NVME_IDENTIFY_CONTROLLER_DATA ident = {0};
        bool ok = IdentifyNvmeController(devif, &ident);
        if(!ok)
        {
            _tprintf(_T("IdentifyNvmeController() failed....\n"));
            continue;
        }

        _tprintf(_T("[IdentifyController] VID=%X, Version=%X\n"), ident.VID, ident.VER);
    }
}
