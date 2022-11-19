#pragma once

class COpalDevice {
public:
    COpalDevice(tstring devpath);
    virtual ~COpalDevice();

protected:
    DWORD OpenSession();
    void CloseSession();

    virtual DWORD SendCommand();
    //Read properties and informations from device, via OPAL commands.
    virtual DWORD ReadOpalData() = 0;

    //Write config and properties to device, via OPAL commands.
    virtual DWORD WriteOpalData() = 0;

    virtual DWORD Discovery0() = 0;
    virtual DWORD Identify() = 0;

    OPAL_DEVICE_INFO DevInfo;
    tstring DevPath = _T("");
};


class COpalNvme : public COpalDevice{
public:
    COpalNvme(tstring devpath);
    ~COpalNvme();

protected:
    virtual DWORD ReadOpalData();
    //Write config and properties to device, via OPAL commands.
    virtual DWORD WriteOpalData();

    virtual DWORD Discovery0();
    virtual DWORD Identify();

//private:
//    void FillDiscovery0Cmd(PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd, PVOID payload, ULONG payload_size, UCHAR protocol, UINT16 base_comid);
};
