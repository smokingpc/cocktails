#pragma once

//Opal Device defines the storage device which support SED features.
//it should support at least NVMe and ATA devices.
//COpalDevice should define common behavior of SED. child class defines implementation.

class COpalDevice {
public:
    COpalDevice(tstring devpath);
    virtual ~COpalDevice();

protected:
    DWORD OpenSession();
    void CloseSession();

    virtual DWORD Discovery0() = 0;
    virtual DWORD Identify() = 0;
    virtual void ParseDiscovery0(IN BYTE* buffer) = 0;

    OPAL_DEVICE_INFO DevInfo;
    tstring DevPath = _T("");
};


class COpalNvme : public COpalDevice{
public:
    COpalNvme(tstring devpath);
    ~COpalNvme();

protected:
    DWORD Discovery0();
    DWORD Identify();
    void ParseDiscovery0(IN BYTE* buffer);

private:
    void ParseIndentify(PINQUIRYDATA data);
    void ParseIndentify(PVPD_SERIAL_NUMBER_PAGE data);
    DWORD SendCommand(DWORD ioctl, PVOID cmd_buf, size_t cmd_size);

    //
    DWORD DoScsiSecurityProtocolIn(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiSecurityProtocolOut(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiInquiry(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiInquiry3Vpd(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
};

