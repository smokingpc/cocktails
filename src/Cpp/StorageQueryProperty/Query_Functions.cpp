#include "Common.h"

static BOOL SendIoctlStorageQuery(
        tstring& devpath,
        LPVOID in_buffer, DWORD in_size,
        LPVOID out_buffer, DWORD out_size,
        LPDWORD ret_size)
{
    BOOL ret = FALSE;
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

BOOL QueryDeviceProperty(tstring& devpath)
{
    LPVOID in_buffer = NULL, out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    out_size = sizeof(STORAGE_DEVICE_DESCRIPTOR);
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceProperty;
    PSTORAGE_DEVICE_DESCRIPTOR result = (PSTORAGE_DEVICE_DESCRIPTOR) out_buffer;

    BOOL ok = SendIoctlStorageQuery(devpath, 
                    in_buffer, in_size, 
                    out_buffer, out_size, 
                    &ret_size);
    if(ok)
    {
        //todo: print result
        _tprintf(_T("print result still not implemented yet!\n"));
    }

    if(NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}

BOOL QueryAdapterProperty(tstring& devpath)
{
    LPVOID in_buffer = NULL, out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    out_size = sizeof(STORAGE_ADAPTER_DESCRIPTOR);
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageAdapterProperty;
    PSTORAGE_ADAPTER_DESCRIPTOR result = (PSTORAGE_ADAPTER_DESCRIPTOR)out_buffer;

    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        //todo: print result
        _tprintf(_T("print result still not implemented yet!\n"));
    }

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}

BOOL QueryDeviceIdProperty(tstring& devpath)
{
    LPVOID in_buffer = NULL, out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    out_size = sizeof(STORAGE_DEVICE_ID_DESCRIPTOR);
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceIdProperty;
    PSTORAGE_DEVICE_ID_DESCRIPTOR result = (PSTORAGE_DEVICE_ID_DESCRIPTOR)out_buffer;

    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        //todo: print result
        _tprintf(_T("print result still not implemented yet!\n"));
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
    LPVOID in_buffer = NULL, out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    out_size = sizeof(STORAGE_ADAPTER_DESCRIPTOR);
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
        //todo: print result
        _tprintf(_T("print result still not implemented yet!\n"));
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
    LPVOID in_buffer = NULL, out_buffer = NULL;
    DWORD in_size = 0, out_size = 0, ret_size = 0;

    in_size = sizeof(STORAGE_PROPERTY_QUERY);
    in_buffer = new BYTE[in_size];
    memset(in_buffer, 0, in_size);

    out_size = sizeof(STORAGE_ADAPTER_DESCRIPTOR);
    out_buffer = new BYTE[out_size];
    memset(out_buffer, 0, out_size);

    PSTORAGE_PROPERTY_QUERY query = (PSTORAGE_PROPERTY_QUERY)in_buffer;
    query->QueryType = PropertyStandardQuery;
    query->PropertyId = StorageDeviceWriteCacheProperty;
    PSTORAGE_WRITE_CACHE_PROPERTY result = (PSTORAGE_WRITE_CACHE_PROPERTY)out_buffer;

    BOOL ok = SendIoctlStorageQuery(devpath,
        in_buffer, in_size,
        out_buffer, out_size,
        &ret_size);
    if (ok)
    {
        //todo: print result
        _tprintf(_T("print result still not implemented yet!\n"));
    }

    if (NULL != in_buffer)
        delete[] in_buffer;

    if (NULL != out_buffer)
        delete[] out_buffer;
    return ok;
}

