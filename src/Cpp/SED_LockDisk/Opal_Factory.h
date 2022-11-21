#pragma once
namespace OpalFactory 
{

    DWORD EnumOpalDevices(IN OUT std::list<OPAL_DEVICE_INFO>& result);
    DWORD QueryOpalDevices(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result);
    //static DWORD OpalDiscovery0(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result){}
    DWORD EnumWin32Disks(IN OUT std::list<tstring> &result);
}
