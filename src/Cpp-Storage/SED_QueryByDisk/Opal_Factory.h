#pragma once

void EnumOpalDevices(IN OUT std::list<class COpalDevice*>& result);
DWORD QueryOpalDevices(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result);
//size_t EnumOpalDevice(IN tstring& diskname, IN OUT list< COpalDevice*> devlist);
//OPAL_DEVICE_INFO* CreateOpalDevice(IN tstring& diskname);

STORAGE_BUS_TYPE QueryStorageType(IN tstring& diskname);
DWORD EnumWin32Disks(IN OUT std::list<tstring> &result);
