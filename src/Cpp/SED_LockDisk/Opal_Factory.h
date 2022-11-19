#pragma once
static class COpalFactory {
public:
    static DWORD EnumOpalDevices(IN OUT list<OPAL_DEVICE_INFO>& result);
    static DWORD QueryOpalDevices(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result);

private:
    static DWORD OpalDiscovery0(IN tstring& diskname, IN OUT OPAL_DEVICE_INFO& result);
    static DWORD EnumDisks(IN OUT list<tstring> &result);
};
