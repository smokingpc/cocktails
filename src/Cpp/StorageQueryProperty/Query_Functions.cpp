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
        //result->Version == sizeof(struct)
        //result->Size == sizeof(struct) + sizeof(following extra data)
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
    //result->Version == sizeof(struct)
    //result->Size == sizeof(struct) + sizeof(following extra data)
        _tprintf(_T("Version=%d, Size=%d\n"), result->Version, result->Size);
        _tprintf(_T("MaxTxLength=%d, MaxPhysicalPages=%d, AlignmentMask=%d\n"), result->MaximumTransferLength, result->MaximumPhysicalPages, result->AlignmentMask);
        _tprintf(_T("UsesPio=%s, ScanDown=%s, CmdQueue=%s, AcceleratedTx=%s\n"), ToStr(result->AdapterUsesPio), ToStr(result->AdapterScansDown), ToStr(result->CommandQueueing), ToStr(result->AcceleratedTransfer));
        _tprintf(_T("BusType=%d, BusMajorVer=%d, BusMinorVer=%d\n"), result->BusType, result->BusMajorVersion, result->BusMinorVersion);
        _tprintf(_T("SrbType=%d, AddressType=%d\n"), result->SrbType, result->AddressType);
        _tprintf(_T("\n"));
    }

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
        //result->Version == sizeof(struct)
        //result->Size == sizeof(struct) + sizeof(following extra data)
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

        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

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
        _tprintf(_T("\n"));
    }

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}
