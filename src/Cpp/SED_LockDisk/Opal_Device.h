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
    
    inline bool IsOpal2(){return (DevFeature == FEATURE_CODE::OPAL_V200);}
    inline bool IsOpal1() { return (DevFeature == FEATURE_CODE::OPAL_V100); }
    inline bool IsEnterprise() { return (DevFeature == FEATURE_CODE::ENTERPRISE); }

    inline void GetDeviceInfo(OPAL_DEVICE_INFO &info){RtlCopyMemory(&info, &DevInfo, sizeof(OPAL_DEVICE_INFO));}
    UINT16 GetBaseComID();
    virtual bool QueryTPerProperties(BYTE *buffer, size_t buf_size) = 0;
    virtual bool LockGlobalRange(const char* pwd) = 0;
    virtual bool UnlockGlobalRange(const char* pwd) = 0;
    virtual bool LockGlobalRange(const wchar_t* pwd) = 0;
    virtual bool UnlockGlobalRange(const wchar_t* pwd) = 0;

    static UINT32 GetHostSessionID();

protected:
    OPAL_DEVICE_INFO DevInfo;
    FEATURE_CODE DevFeature = FEATURE_CODE::NO_FEATURE;
    
    //following fields comes from TCG_Storage_Opal_SSC_Application_Note.pdf
    //it explains communication packets and blocks structure.
    static volatile UINT32 HostSession;
    virtual UINT32 StartSession(UINT32 host_sid, OPAL_UID_TAG provider, BOOLEAN is_write, char* pwd) = 0;
    virtual void EndSession(UINT32 host_sid, UINT32 tper_sid) = 0;
};


class COpalNvme : public COpalDevice{
public:
    COpalNvme(tstring devpath);
    ~COpalNvme();

    DWORD Discovery0();
    DWORD Identify();
    bool QueryTPerProperties(BYTE* resp, size_t resp_size);
    bool LockGlobalRange(const char* pwd);
    bool LockGlobalRange(const wchar_t* pwd);
    bool UnlockGlobalRange(const char* pwd);
    bool UnlockGlobalRange(const wchar_t* pwd);

protected:
    void ParseDiscovery0(IN BYTE* buffer);
    void ParseIndentify(PINQUIRYDATA data);
    void ParseIndentify(PVPD_SERIAL_NUMBER_PAGE data);
    UINT32 StartSession(UINT32 host_sid, OPAL_UID_TAG provider, BOOLEAN is_write, char *pwd);
    void EndSession(UINT32 host_sid, UINT32 tper_sid);

private:
    HANDLE DevHandle = INVALID_HANDLE_VALUE;

    DWORD SendCommand(DWORD ioctl, PVOID cmd_buf, size_t cmd_size);

    DWORD DoScsiSecurityProtocolIn(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiSecurityProtocolOut(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiInquiry(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
    DWORD DoScsiInquiry3Vpd(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size);
};

