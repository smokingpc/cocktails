#include "Common.h"

static BOOL SendIoctlStorageQuery(
        tstring& devpath,
        LPVOID in_buffer, DWORD in_size,
        LPVOID out_buffer, DWORD out_size,
        LPDWORD ret_size)
{
    BOOL ret = FALSE;

    //open device handle for IOCTL_STORAGE_QUERY_PROPERTY need these attributes.
    //GENERIC_READ | GENERIC_WRITE,
    //FILE_SHARE_READ | FILE_SHARE_WRITE,
    //OPEN_EXISTING,
    //FILE_ATTRIBUTE_NORMAL,

    HANDLE device = CreateFile(devpath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("CreateFile failed. LastError=%d\n"), GetLastError());
        ret = FALSE;
        goto END;
    }

    ret = DeviceIoControl(
                device, IOCTL_STORAGE_QUERY_PROPERTY,
                in_buffer, in_size, 
                out_buffer, out_size,
                ret_size, NULL);

    if(!ret)
    {
        _tprintf(_T("DeviceIoControl failed. LastError=%d\n"), GetLastError());
    }

END:
    if (NULL != device && INVALID_HANDLE_VALUE != device)
        CloseHandle(device);
    return ret;
}

static __inline const TCHAR* ToStr(BOOL value)
{
    return (TRUE==value)? _T("TRUE") : _T("FALSE");
}

BOOL QueryDeviceProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceProperty;
    PSTORAGE_DEVICE_DESCRIPTOR result = (PSTORAGE_DEVICE_DESCRIPTOR) out_buffer;

    _tprintf(_T("[StorageDeviceProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
                    in_buffer, in_size, 
                    out_buffer, out_size, 
                    &ret_size);
    if(ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("DeviceType=%d, DeviceTypeModifier=%d\n"), result->DeviceType, result->DeviceTypeModifier);
        _tprintf(_T("IsRemovableMedia=%s, Support CmdQueue=%s\n"), ToStr(result->RemovableMedia), ToStr(result->CommandQueueing));
        _tprintf(_T("VenderIdOffset=%d, ProductIdOffset=%d\n"), result->VendorIdOffset, result->ProductIdOffset);
        _tprintf(_T("ProductRevOffset=%d, SerialNumberOffset=%d\n"), result->ProductRevisionOffset, result->SerialNumberOffset);
        _tprintf(_T("BusType=%d, RawPropertiesLength=%d\n"), result->BusType, result->RawPropertiesLength);
        _tprintf(_T("VendorId=%S\n"), (out_buffer + result->VendorIdOffset));
        _tprintf(_T("ProductId=%S\n"), (out_buffer + result->ProductIdOffset));
        _tprintf(_T("ProductRev=%S\n"), (out_buffer + result->ProductRevisionOffset));
        _tprintf(_T("SerialNumber=%S\n"), (out_buffer + result->SerialNumberOffset));
        _tprintf(_T("\n"));
    }

    if(NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAdapterProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterProperty;
    PSTORAGE_ADAPTER_DESCRIPTOR result = (PSTORAGE_ADAPTER_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAdapterProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("MaxTxLength=%d, MaxPhysicalPages=%d, AlignmentMask=%d\n"), result->MaximumTransferLength, result->MaximumPhysicalPages, result->AlignmentMask);
        _tprintf(_T("UsesPio=%s, ScanDown=%s, CmdQueue=%s, AcceleratedTx=%s\n"), ToStr(result->AdapterUsesPio), ToStr(result->AdapterScansDown), ToStr(result->CommandQueueing), ToStr(result->AcceleratedTransfer));
        _tprintf(_T("BusType=%d, BusMajorVer=%d, BusMinorVer=%d\n"), result->BusType, result->BusMajorVersion, result->BusMinorVersion);
        _tprintf(_T("SrbType=%d, AddressType=%d\n"), result->SrbType, result->AddressType);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceIdProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceIdProperty;
    PSTORAGE_DEVICE_ID_DESCRIPTOR result = (PSTORAGE_DEVICE_ID_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceIdProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);

        PSTORAGE_IDENTIFIER identifier = NULL;
        BYTE *cursor = result->Identifiers;
        DWORD offset = 0;
        for(DWORD i=0; i<result->NumberOfIdentifiers; i++)
        {
            cursor += offset;
            identifier = (PSTORAGE_IDENTIFIER)cursor;
            _tprintf(_T("Identifier[%d] => "), i);
            switch(identifier->CodeSet)
            {
                case StorageIdCodeSetBinary:
                {
                    for(DWORD j=0; j< identifier->IdentifierSize; j++)
                    {
                        _tprintf(_T("%02X"), identifier->Identifier[j]);
                    }
                    _tprintf(_T(" => "));
                    _tprintf(_T("%S"), identifier->Identifier);
                    _tprintf(_T("\n"));
                }
                    break;
                case StorageIdCodeSetAscii:
                case StorageIdCodeSetUtf8:
                {
                    _tprintf(_T("%S\n"), identifier->Identifier);
                }
                    break;
            }

            offset = identifier->NextOffset;
        }

    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}

#if 0       //for Driver Use Only
BOOL QueryDeviceUniqueIdProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceUniqueIdProperty;
    PSTORAGE_DEVICE_UNIQUE_IDENTIFIER result = (PSTORAGE_DEVICE_UNIQUE_IDENTIFIER)out_buffer;

    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("[StorageDeviceProperty]\n"));
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
#endif

BOOL QueryDeviceWriteCacheProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceWriteCacheProperty;
    PSTORAGE_WRITE_CACHE_PROPERTY result = (PSTORAGE_WRITE_CACHE_PROPERTY)out_buffer;

    _tprintf(_T("[StorageDeviceWriteCacheProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("WriteCacheType=%d, WriteCacheEnabled=%d\n"), result->WriteCacheType, result->WriteCacheEnabled);
        _tprintf(_T("WriteCacheChangeable=%d, WriteThroughSupported=%d\n"), result->WriteCacheChangeable, result->WriteThroughSupported);
        _tprintf(_T("FlushCacheSupported=%s, UserDefinedPowerProtection=%s\n"), ToStr(result->FlushCacheSupported), ToStr(result->UserDefinedPowerProtection));
        _tprintf(_T("NVCacheEnabled=%s\n"), ToStr(result->NVCacheEnabled));
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAccessAlignmentProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAccessAlignmentProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_ACCESS_ALIGNMENT_DESCRIPTOR result = (PSTORAGE_ACCESS_ALIGNMENT_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAccessAlignmentProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("BytesPerCacheLine=%d\n"), result->BytesPerCacheLine);
        _tprintf(_T("BytesOffsetForCacheAlignment=%d\n"), result->BytesOffsetForCacheAlignment);
        _tprintf(_T("BytesPerLogicalSector=%d\n"), result->BytesPerLogicalSector);
        _tprintf(_T("BytesPerPhysicalSector=%d\n"), result->BytesPerPhysicalSector);
        _tprintf(_T("BytesOffsetForSectorAlignment=%d\n"), result->BytesOffsetForSectorAlignment);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceSeekPenaltyProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceSeekPenaltyProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PDEVICE_SEEK_PENALTY_DESCRIPTOR result = (PDEVICE_SEEK_PENALTY_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceSeekPenaltyProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("IncursSeekPenalty=%s\n"), ToStr(result->IncursSeekPenalty));
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceTrimProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceTrimProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PDEVICE_TRIM_DESCRIPTOR result = (PDEVICE_TRIM_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceTrimProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("TrimEnabled=%s\n"), ToStr(result->TrimEnabled));
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceLBProvisioningProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceLBProvisioningProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PDEVICE_LB_PROVISIONING_DESCRIPTOR result = (PDEVICE_LB_PROVISIONING_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceLBProvisioningProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("ThinProvisioningEnabled=%d\n"), result->ThinProvisioningEnabled);
        _tprintf(_T("ThinProvisioningReadZeros=%d\n"), result->ThinProvisioningReadZeros);
        _tprintf(_T("AnchorSupported=%d\n"), result->AnchorSupported);
        _tprintf(_T("UnmapGranularityAlignmentValid=%d\n"), result->UnmapGranularityAlignmentValid);
        _tprintf(_T("GetFreeSpaceSupported=%d\n"), result->GetFreeSpaceSupported);
        _tprintf(_T("MapSupported=%d\n"), result->MapSupported);

        _tprintf(_T("OptimalUnmapGranularity=%lld\n"), result->OptimalUnmapGranularity);
        _tprintf(_T("UnmapGranularityAlignment=%lld\n"), result->UnmapGranularityAlignment);
        _tprintf(_T("MaxUnmapLbaCount=%d\n"), result->MaxUnmapLbaCount);
        _tprintf(_T("MaxUnmapBlockDescriptorCount=%d\n"), result->MaxUnmapBlockDescriptorCount);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDevicePowerProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDevicePowerProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PDEVICE_POWER_DESCRIPTOR result = (PDEVICE_POWER_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDevicePowerProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("DeviceAttentionSupported=%s\n"), ToStr(result->DeviceAttentionSupported));
        _tprintf(_T("AsynchronousNotificationSupported=%s\n"), ToStr(result->AsynchronousNotificationSupported));
        _tprintf(_T("IdlePowerManagementEnabled=%s\n"), ToStr(result->IdlePowerManagementEnabled));
        _tprintf(_T("D3ColdEnabled=%s\n"), ToStr(result->D3ColdEnabled));
        _tprintf(_T("D3ColdSupported=%s\n"), ToStr(result->D3ColdSupported));
        _tprintf(_T("NoVerifyDuringIdlePower=%s\n"), ToStr(result->NoVerifyDuringIdlePower));
        _tprintf(_T("IdleTimeoutInMS=%d\n"), result->IdleTimeoutInMS);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceCopyOffloadProperty(tstring& devpath)
{
    BYTE *in_buffer = NULL, *out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceCopyOffloadProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PDEVICE_COPY_OFFLOAD_DESCRIPTOR result = (PDEVICE_COPY_OFFLOAD_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceCopyOffloadProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("MaximumTokenLifetime=%d\n"), result->MaximumTokenLifetime);
        _tprintf(_T("DefaultTokenLifetime=%d\n"), result->DefaultTokenLifetime);
        _tprintf(_T("MaximumTransferSize=%lld\n"), result->MaximumTransferSize);
        _tprintf(_T("OptimalTransferCount=%lld\n"), result->OptimalTransferCount);
        _tprintf(_T("MaximumDataDescriptors=%d\n"), result->MaximumDataDescriptors);
        _tprintf(_T("MaximumTransferLengthPerDescriptor=%d\n"), result->MaximumTransferLengthPerDescriptor);
        _tprintf(_T("OptimalTransferLengthPerDescriptor=%d\n"), result->OptimalTransferLengthPerDescriptor);
        _tprintf(_T("OptimalTransferLengthGranularity=%d\n"), result->OptimalTransferLengthGranularity);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}

BOOL QueryDeviceMediumProductType(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceMediumProductType;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_MEDIUM_PRODUCT_TYPE_DESCRIPTOR result = (PSTORAGE_MEDIUM_PRODUCT_TYPE_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceMediumProductType]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("MediumProductType=%d\n"), result->MediumProductType);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAdapterRpmbProperty(tstring& devpath)
{
//Replay Protected Memory Block (RPMB)
//what the hell is it?
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterRpmbProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_RPMB_DESCRIPTOR result = (PSTORAGE_RPMB_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAdapterRpmbProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("RPMB SizeInBytes=%d\n"), result->SizeInBytes);
        _tprintf(_T("MaxReliableWriteSizeInBytes=%d\n"), result->MaxReliableWriteSizeInBytes);
        _tprintf(_T("FrameFormat=%d\n"), result->FrameFormat);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAdapterCryptoProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterCryptoProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_CRYPTO_DESCRIPTOR result = (PSTORAGE_CRYPTO_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAdapterCryptoProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        //result->Version == sizeof(struct of result)
        //result->Size == sizeof(struct) + sizeof(following extra data)
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("NumKeysSupported=%d, NumCryptoCapabilities=%d\n"), result->NumKeysSupported, result->NumCryptoCapabilities);
        
        PSTORAGE_CRYPTO_CAPABILITY cap = result->CryptoCapabilities;

        for(DWORD i=0;i< result->NumCryptoCapabilities; i++)
        {
            _tprintf(_T("CryptoCap[%d]\n"), i);
            _tprintf(_T("  Version=%d, Size=%d\n"), cap->Version, cap->Size);
            _tprintf(_T("  CryptoCapabilityIndex=%d\n"), cap->CryptoCapabilityIndex);
            _tprintf(_T("  AlgorithmId=%d\n"), cap->AlgorithmId);
            _tprintf(_T("  KeySize=%d\n"), cap->KeySize);
            _tprintf(_T("  DataUnitSizeBitmask=%d\n"), cap->DataUnitSizeBitmask);
            cap++;
        }
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceIoCapabilityProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceIoCapabilityProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_DEVICE_IO_CAPABILITY_DESCRIPTOR result = (PSTORAGE_DEVICE_IO_CAPABILITY_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceIoCapabilityProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("LunMaxIoCount=%d, AdapterMaxIoCount=%d\n"), result->LunMaxIoCount, result->AdapterMaxIoCount);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAdapterProtocolSpecificProperty(tstring& devpath)
{
    _tprintf(_T("[StorageAdapterProtocolSpecificProperty] ==> not implemented!\n"));
    return FALSE;
#if 0
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterProtocolSpecificProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_PROTOCOL_DATA_DESCRIPTOR result = (PSTORAGE_PROTOCOL_DATA_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAdapterProtocolSpecificProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("LunMaxIoCount=%d, AdapterMaxIoCount=%d\n"), result->LunMaxIoCount, result->AdapterMaxIoCount);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
#endif
}
BOOL QueryDeviceProtocolSpecificPropert(tstring& devpath)
{
    _tprintf(_T("[StorageDeviceProtocolSpecificProperty] ==> not implemented!\n"));
    return FALSE;
#if 0
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceProtocolSpecificProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_DEVICE_IO_CAPABILITY_DESCRIPTOR result = (PSTORAGE_DEVICE_IO_CAPABILITY_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceProtocolSpecificProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("LunMaxIoCount=%d, AdapterMaxIoCount=%d\n"), result->LunMaxIoCount, result->AdapterMaxIoCount);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
#endif
}
BOOL QueryAdapterTemperatureProperty(tstring& devpath) 
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterTemperatureProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_TEMPERATURE_DATA_DESCRIPTOR result = (PSTORAGE_TEMPERATURE_DATA_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAdapterTemperatureProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("CriticalTemperature=%d, WarningTemperature=%d\n"), result->CriticalTemperature, result->WarningTemperature);
        _tprintf(_T("InfoCount=%d\n"), result->InfoCount);

        PSTORAGE_TEMPERATURE_INFO info = result->TemperatureInfo;

        for (DWORD i = 0; i < result->InfoCount; i++)
        {
            _tprintf(_T("TemperatureInfo[%d]\n"), i);
            //cap->Version is real version number. currently it use STORAGE_CRYPTO_CAPABILITY_VERSION_1.
            //cap->size == sizeof(STORAGE_CRYPTO_CAPABILITY)
            _tprintf(_T("  Index=%d, Temperature=%d\n"), info->Index, info->Temperature);
            _tprintf(_T("  OverThreshold=%d, UnderThreshold=%d\n"), info->OverThreshold, info->UnderThreshold);
            _tprintf(_T("  OverThresholdChangable=%s\n"), ToStr(info->OverThresholdChangable));
            _tprintf(_T("  UnderThresholdChangable=%s\n"), ToStr(info->UnderThresholdChangable));
            _tprintf(_T("  EventGenerated=%s\n"), ToStr(info->EventGenerated));

            info++;
        }
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceTemperatureProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceTemperatureProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_TEMPERATURE_DATA_DESCRIPTOR result = (PSTORAGE_TEMPERATURE_DATA_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceTemperatureProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("CriticalTemperature=%d, WarningTemperature=%d\n"), result->CriticalTemperature, result->WarningTemperature);
        _tprintf(_T("InfoCount=%d\n"), result->InfoCount);

        PSTORAGE_TEMPERATURE_INFO info = result->TemperatureInfo;

        for (DWORD i = 0; i < result->InfoCount; i++)
        {
            _tprintf(_T("TemperatureInfo[%d]\n"), i);
            _tprintf(_T("  Index=%d, Temperature=%d\n"), info->Index, info->Temperature);
            _tprintf(_T("  OverThreshold=%d, UnderThreshold=%d\n"), info->OverThreshold, info->UnderThreshold);
            _tprintf(_T("  OverThresholdChangable=%s\n"), ToStr(info->OverThresholdChangable));
            _tprintf(_T("  UnderThresholdChangable=%s\n"), ToStr(info->UnderThresholdChangable));
            _tprintf(_T("  EventGenerated=%s\n"), ToStr(info->EventGenerated));

            info++;
        }
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAdapterPhysicalTopologyProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterPhysicalTopologyProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_PHYSICAL_TOPOLOGY_DESCRIPTOR result = (PSTORAGE_PHYSICAL_TOPOLOGY_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageAdapterPhysicalTopologyProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("NodeCount=%d\n"), result->NodeCount);

        PSTORAGE_PHYSICAL_NODE_DATA  data = result->Node;

        for (DWORD i = 0; i < result->NodeCount; i++)
        {
            _tprintf(_T("Node[%d]\n"), i);
            _tprintf(_T("  NodeId=%d, AdapterCount=%d\n"), data->NodeId, data->AdapterCount);
            _tprintf(_T("  AdapterDataLength=%d, AdapterDataOffset=%d\n"), data->AdapterDataLength, data->AdapterDataOffset);
            _tprintf(_T("  DeviceCount=%d, DeviceDataLength=%d, DeviceDataOffset=%d\n"), data->DeviceCount, data->DeviceDataLength, data->DeviceDataOffset);
            _tprintf(_T(" **Skip parsing AdapterData and DeviceData**\n"));
            data++;
        }
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDevicePhysicalTopologyProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDevicePhysicalTopologyProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_PHYSICAL_TOPOLOGY_DESCRIPTOR result = (PSTORAGE_PHYSICAL_TOPOLOGY_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDevicePhysicalTopologyProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("NodeCount=%d\n"), result->NodeCount);

        PSTORAGE_PHYSICAL_NODE_DATA  data = result->Node;

        for (DWORD i = 0; i < result->NodeCount; i++)
        {
            _tprintf(_T("Node[%d]\n"), i);
            _tprintf(_T("  NodeId=%d, AdapterCount=%d\n"), data->NodeId, data->AdapterCount);
            _tprintf(_T("  AdapterDataLength=%d, AdapterDataOffset=%d\n"), data->AdapterDataLength, data->AdapterDataOffset);
            _tprintf(_T("  DeviceCount=%d, DeviceDataLength=%d, DeviceDataOffset=%d\n"), data->DeviceCount, data->DeviceDataLength, data->DeviceDataOffset);
            _tprintf(_T(" **Skip parsing AdapterData and DeviceData**\n"));
            data++;
        }
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceManagementStatus(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceManagementStatus;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_DEVICE_MANAGEMENT_STATUS result = (PSTORAGE_DEVICE_MANAGEMENT_STATUS)out_buffer;

    _tprintf(_T("[StorageDeviceManagementStatus]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("Health=%d\n"), result->Health);
        _tprintf(_T("NumberOfOperationalStatus=%d, NumberOfAdditionalReasons=%d\n"), result->NumberOfOperationalStatus, result->NumberOfAdditionalReasons);


        for(DWORD i=0;i<result->NumberOfOperationalStatus; i++)
        {
            _tprintf(_T("  OperationalStatus[%d]=%d\n"), i, result->OperationalStatus[i]);
        }
        for (DWORD i = 0; i < result->NumberOfAdditionalReasons; i++)
        {
            PSTORAGE_OPERATIONAL_REASON reason = &result->AdditionalReasons[i];
            _tprintf(_T("  ==>AdditionalReason[%d]\n"), i);
            _tprintf(_T("    Reason=%d\n"), reason->Reason);
            if(DiskOpReasonScsiSenseCode == reason->Reason)
            {
                _tprintf(_T("    SenseKey=%d, ASC=%d, ASCQ=%d\n"), reason->RawBytes.ScsiSenseKey.SenseKey, reason->RawBytes.ScsiSenseKey.ASC, reason->RawBytes.ScsiSenseKey.ASCQ);
            }
            else if(DiskOpReasonScsiSenseCode == reason->Reason)
            { 
                _tprintf(_T("    CriticalHealth=%d, ErrorThresholdStatus=%d\n"), reason->RawBytes.NVDIMM_N.CriticalHealth, reason->RawBytes.NVDIMM_N.ErrorThresholdStatus);
                _tprintf(_T("    ModuleHealth=%02X %02X\n"), reason->RawBytes.NVDIMM_N.ModuleHealth[0], reason->RawBytes.NVDIMM_N.ModuleHealth[1]);
            }
            else
            {
                _tprintf(_T("    RawBytes.AsUlong=%08X\n"), reason->RawBytes.AsUlong);
            }
        }
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryAdapterSerialNumberProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterSerialNumberProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_ADAPTER_SERIAL_NUMBER result = (PSTORAGE_ADAPTER_SERIAL_NUMBER)out_buffer;

    _tprintf(_T("[StorageAdapterSerialNumberProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("SerialNumber=%s\n"), result->SerialNumber);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceNumaProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceNumaProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_DEVICE_NUMA_PROPERTY result = (PSTORAGE_DEVICE_NUMA_PROPERTY)out_buffer;

    _tprintf(_T("[StorageDeviceNumaProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("NumaNode=%d\n"), result->NumaNode);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceUnsafeShutdownCount(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceUnsafeShutdownCount;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_DEVICE_UNSAFE_SHUTDOWN_COUNT result = (PSTORAGE_DEVICE_UNSAFE_SHUTDOWN_COUNT)out_buffer;

    _tprintf(_T("[StorageDeviceUnsafeShutdownCount]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("UnsafeShutdownCount=%d\n"), result->UnsafeShutdownCount);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceEnduranceProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceEnduranceProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_HW_ENDURANCE_DATA_DESCRIPTOR result = (PSTORAGE_HW_ENDURANCE_DATA_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceEnduranceProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("ValidFields="));

        if((result->EnduranceInfo.ValidFields & 0x00000001) == 0x00000001)
            _tprintf(_T("GroupId, "));
        if ((result->EnduranceInfo.ValidFields & 0x00000002) == 0x00000002)
            _tprintf(_T("Flags, "));
        if ((result->EnduranceInfo.ValidFields & 0x00000004) == 0x00000004)
            _tprintf(_T("LifePercentage, "));
        if ((result->EnduranceInfo.ValidFields & 0x00000008) == 0x00000008)
            _tprintf(_T("BytesReadCount, "));
        if ((result->EnduranceInfo.ValidFields & 0x00000010) == 0x00000010)
            _tprintf(_T("ByteWriteCount"));

        _tprintf(_T("\n"));
        if ((result->EnduranceInfo.ValidFields & 0x00000001) == 0x00000001)
            _tprintf(_T("  GroupId=%d\n"), result->EnduranceInfo.GroupId);
        if ((result->EnduranceInfo.ValidFields & 0x00000002) == 0x00000002)
            _tprintf(_T("  Flags.Shared=%d\n"), result->EnduranceInfo.Flags.Shared);
        if ((result->EnduranceInfo.ValidFields & 0x00000004) == 0x00000004)
            _tprintf(_T("  LifePercentage=%d\n"), result->EnduranceInfo.LifePercentage);
        if ((result->EnduranceInfo.ValidFields & 0x00000008) == 0x00000008)
        { 
            UINT64 *value = (UINT64*)result->EnduranceInfo.BytesReadCount;
            _tprintf(_T("  BytesReadCount lowbytes=%016llX, highbytes=%016llX\n"), value[0], value[1]);
        }
        if ((result->EnduranceInfo.ValidFields & 0x00000010) == 0x00000010)
        {
            UINT64* value = (UINT64*)result->EnduranceInfo.ByteWriteCount;
            _tprintf(_T("  ByteWriteCount lowbytes=%016llX, highbytes=%016llX\n"), value[0], value[1]);
        }

    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryDeviceLedStateProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceLedStateProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_DEVICE_LED_STATE_DESCRIPTOR result = (PSTORAGE_DEVICE_LED_STATE_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageDeviceLedStateProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("State=%016llX\n"), result->State);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
BOOL QueryFruIdProperty(tstring& devpath)
{
    BYTE* in_buffer = NULL, * out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);
    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageFruIdProperty;

    //most of returned structure contains extra data followed after structure.
    //You can query "required return size" by setting no out_buffer in DeviceIoControl.
    //I am lazy so just use 1 page to retrieve returned data.
    out_size = PAGE_SIZE;
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);
    PSTORAGE_FRU_ID_DESCRIPTOR result = (PSTORAGE_FRU_ID_DESCRIPTOR)out_buffer;

    _tprintf(_T("[StorageFruIdProperty]\n"));
    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("IdentifierSize=%d\n"), result->IdentifierSize);
    }
    _tprintf(_T("\n"));

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
