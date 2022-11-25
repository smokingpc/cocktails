#pragma once

//Opal Device defines the storage device which support SED features.
//it should support at least NVMe and ATA devices.
//COpalDevice should define common behavior of SED. child class defines implementation.

class COpalDevice {
public:
    COpalDevice(tstring devpath);
    virtual ~COpalDevice();
    virtual DWORD Discovery0() = 0;
    virtual DWORD Identify() = 0;
    tstring DevPath = _T("");
    
    inline void GetDeviceInfo(OPAL_DEVICE_INFO &info){RtlCopyMemory(&info, &DevInfo, sizeof(OPAL_DEVICE_INFO));}
    UINT16 GetBaseComID();
    virtual bool QueryTPerProperties(BYTE *buffer, size_t buf_size) = 0;
protected:
    OPAL_DEVICE_INFO DevInfo;
    FEATURE_CODE DevFeature = FEATURE_CODE::NO_FEATURE;
    
    //following fields comes from TCG_Storage_Opal_SSC_Application_Note.pdf
    //it explains communication packets and blocks structure.
    UINT32 HostSession = 0;
};


class COpalNvme : public COpalDevice{
public:
    COpalNvme(tstring devpath);
    ~COpalNvme();

    DWORD Discovery0();
    DWORD Identify();
    bool QueryTPerProperties(BYTE* resp, size_t resp_size);

protected:
    void ParseDiscovery0(IN BYTE* buffer);
    void ParseIndentify(PINQUIRYDATA data);
    void ParseIndentify(PVPD_SERIAL_NUMBER_PAGE data);

private:
    DWORD SendCommand(DWORD ioctl, PVOID cmd_buf, size_t cmd_size);

    DWORD DoScsiSecurityProtocolIn(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiSecurityProtocolOut(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiInquiry(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiInquiry3Vpd(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
};

