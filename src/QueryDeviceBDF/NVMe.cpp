#include "Common.h"

static inline void string_trim(string& str)
{
    std::size_t found = str.find_last_not_of(" ");
    if (found != string::npos)
        str.erase(found + 1);
    else
        str.clear();            // str is all whitespace
}

void BuildNqn(OUT tstring &result, PNVME_IDENTIFY_CONTROLLER_DATA14 identify)
{
    string subnqn = "";
    string model = "";
    string serial = "";
    string fw_rev = "";

    subnqn.assign((char*)identify->SUBNQN);
    if (subnqn == "" || identify->VER == 0)
    {
        int offset = snprintf((char*)identify->SUBNQN, 256,
            "nqn.2014.08.org.nvmexpress:%04x%04x",
            identify->VID, identify->SSVID);

        memcpy((char*)identify->SUBNQN + offset, identify->SN, sizeof(identify->SN));
        offset += sizeof(identify->SN);
        memcpy((char*)identify->SUBNQN + offset, identify->MN, sizeof(identify->MN));
        offset += sizeof(identify->MN);
        memset((char*)identify->SUBNQN + offset, 0, sizeof(identify->SUBNQN) - offset);
        subnqn = (char*)identify->SUBNQN;
    }

    string_trim(subnqn);

#ifdef _UNICODE
    StrToWStr(result, subnqn);
#else
    result = subnqn;
#endif
}

void QueryNVMeDeviceNqn(tstring &result, tstring &devpath)
{
    result = _T("");
    HANDLE hDevice = CreateFile(devpath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hDevice != INVALID_HANDLE_VALUE)
    {
        PSTORAGE_PROPERTY_QUERY query = NULL;
        PSTORAGE_PROTOCOL_SPECIFIC_DATA data = NULL;
        PSTORAGE_PROTOCOL_DATA_DESCRIPTOR desc = NULL;

        DWORD size = FIELD_OFFSET(STORAGE_PROPERTY_QUERY, AdditionalParameters) +
            sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA) +
            sizeof(NVME_IDENTIFY_CONTROLLER_DATA14);
        DWORD ret_size = 0;

        PBYTE buffer = new BYTE[size];
        ZeroMemory(buffer, size);

        query = (PSTORAGE_PROPERTY_QUERY)buffer;
        desc = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)buffer;
        data = (PSTORAGE_PROTOCOL_SPECIFIC_DATA)query->AdditionalParameters;

        query->PropertyId = StorageAdapterProtocolSpecificProperty;
        query->QueryType = PropertyStandardQuery;

        data->ProtocolType = ProtocolTypeNvme;
        data->DataType = NVMeDataTypeIdentify;
        data->ProtocolDataRequestValue = NVME_IDENTIFY_CNS_CONTROLLER;
        data->ProtocolDataRequestSubValue = 0;
        data->ProtocolDataOffset = sizeof(STORAGE_PROTOCOL_SPECIFIC_DATA);
        data->ProtocolDataLength = sizeof(NVME_IDENTIFY_CONTROLLER_DATA14);

        BOOL ok = DeviceIoControl(hDevice,
            IOCTL_STORAGE_QUERY_PROPERTY,
            buffer,
            size,
            buffer,
            size,
            &ret_size,
            NULL);


        if (!ok)
            _tprintf(_T("DeviceIoControl failed, LastError=%d\n"), GetLastError());

        if ((desc->Version != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)) ||
            (desc->Size != sizeof(STORAGE_PROTOCOL_DATA_DESCRIPTOR)))
        {
            _tprintf(_T("Invalid return data from DeviceIoControl\n"));
            ok = FALSE;
        }

        data = &desc->ProtocolSpecificData;

        PNVME_IDENTIFY_CONTROLLER_DATA14 identify = (PNVME_IDENTIFY_CONTROLLER_DATA14)((PCHAR)data + data->ProtocolDataOffset);

        if ((identify->VID == 0) || (identify->NN == 0))
        {
            _tprintf(_T("Invalid NVME_IDENTIFY_CONTROLLER_DATA\n"));
            ok = FALSE;
        }

        if (ok)
            BuildNqn(result, identify);

        delete[] buffer;
        CloseHandle(hDevice);
    }
    else
        wprintf(L"failed to open %s, skip...\n", devpath.c_str());
}