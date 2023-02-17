#pragma once

BOOL QueryDeviceProperty(tstring& devpath);
BOOL QueryAdapterProperty(tstring& devpath);
BOOL QueryDeviceIdProperty(tstring& devpath);
#if 0       //for Driver Use Only
BOOL QueryDeviceUniqueIdProperty(tstring& devpath);
#endif
BOOL QueryDeviceWriteCacheProperty(tstring& devpath);
