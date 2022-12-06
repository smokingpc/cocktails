#include "Common.h"
using namespace std;

static void FillScsiCDB_Inquiry(PCDB cdb, UCHAR payload_size)
{
    cdb->CDB6INQUIRY.OperationCode = SCSIOP_INQUIRY;
    cdb->CDB6INQUIRY.AllocationLength = payload_size;
}
static void FillScsiCDB_Inquiry3Vpd(PCDB cdb, UCHAR payload_size)
{
    cdb->CDB6INQUIRY3.OperationCode = SCSIOP_INQUIRY;
    cdb->CDB6INQUIRY3.EnableVitalProductData = 1;
    cdb->CDB6INQUIRY3.PageCode = VPD_SERIAL_NUMBER;
    cdb->CDB6INQUIRY3.AllocationLength = payload_size;
}

static void FillScsiCDB_SecurityIn(PCDB cdb, UCHAR protocol, UINT16 comid, ULONG payload_size )
{
    cdb->SECURITY_PROTOCOL_IN.OperationCode = SCSIOP_SECURITY_PROTOCOL_IN;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocol = protocol;

    //SecurityProtocolSpecific and AllocationLength field are BIG-ENDIAN
    SwapEndian(&comid, (UINT16*)cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific);
    DWORD temp = payload_size;
    SwapEndian(&temp, (ULONG*)cdb->SECURITY_PROTOCOL_IN.AllocationLength);
}

static void FillScsiCDB_SecurityOut(PCDB cdb, UCHAR protocol, UINT16 comid, ULONG payload_size)
{
    cdb->SECURITY_PROTOCOL_OUT.OperationCode = SCSIOP_SECURITY_PROTOCOL_OUT;
    cdb->SECURITY_PROTOCOL_OUT.SecurityProtocol = protocol;

    //SecurityProtocolSpecific and AllocationLength field are BIG-ENDIAN
    SwapEndian(&comid, (UINT16*)cdb->SECURITY_PROTOCOL_OUT.SecurityProtocolSpecific);
    DWORD temp = payload_size;
    SwapEndian(&temp, (ULONG*)cdb->SECURITY_PROTOCOL_OUT.AllocationLength);
}

static void FillScsiCmd(PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd, PVOID buffer, ULONG buf_size, UCHAR cdb_len, UCHAR data_inout)
{
    cmd->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    cmd->PathId = 0;
    cmd->TargetId = 1;
    cmd->Lun = 0;
    cmd->DataTransferLength = buf_size; //length of cmd->DataBuffer.
    cmd->TimeOutValue = 2;      //in seconds
    cmd->DataBuffer = buffer;
    cmd->SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE, SenseInfo);
    cmd->SenseInfoLength = sizeof(cmd->SenseInfo);
    cmd->CdbLength = cdb_len;
    cmd->DataIn = data_inout;
}


UINT32 volatile COpalDevice::HostSession = 0;

COpalDevice::COpalDevice(tstring devpath)
{
    DevPath = devpath;
}
COpalDevice::~COpalDevice(){}
UINT32 COpalDevice::GetHostSessionID()
{
    return InterlockedIncrement(&HostSession);
    //return 105; //??  copied from sedutils....
}

COpalNvme::COpalNvme(tstring devpath) : COpalDevice(devpath)
{
    Discovery0();
    Identify();

    DevInfo.Type = BusTypeNvme;
}
COpalNvme::~COpalNvme()
{
    if(INVALID_HANDLE_VALUE != DevHandle)
    {
        CloseHandle(DevHandle);
        DevHandle = INVALID_HANDLE_VALUE;
    }
}

UINT16 COpalDevice::GetBaseComID()
{
    switch (DevFeature)
    {
    case FEATURE_CODE::ENTERPRISE:
        return DevInfo.Enterprise.GetBaseComID();
    case FEATURE_CODE::OPAL_V100:
        return DevInfo.OpalV100.GetBaseComID();
    case FEATURE_CODE::OPAL_V200:
        return DevInfo.OpalV200.GetBaseComID();
    }

    return 0;
}
DWORD COpalNvme::Discovery0()
{
    //discovery0() use SCSI_OP==SCSIOP_SECURITY_PROTOCOL_IN with empty payload to ask 
    //device discovery information.
    UCHAR protocol = 1;
    UINT16 comid = DISCOVERY0_COMID;
    DWORD buf_size = BIG_BUFFER_SIZE;
    BYTE *opal_buffer = new BYTE[buf_size];
    unique_ptr<BYTE> buf_ptr(opal_buffer);
    RtlZeroMemory(opal_buffer, buf_size);

    DWORD rc = DoScsiSecurityProtocolIn(protocol, comid, opal_buffer, buf_size);
    if(ERROR_SUCCESS != rc)
    {
        //todo: log....
        return rc;
    }

    ParseDiscovery0(opal_buffer);

    return ERROR_SUCCESS;
}
DWORD COpalNvme::Identify()
{
    DWORD rc = ERROR_SUCCESS;

    //Identify() use SCSI_OP==SCSIOP_INQUIRY to get device identify information.
    UCHAR protocol = 1;
    UINT16 comid = 1;
    DWORD buf_size = SMALL_BUFFER_SIZE;
    BYTE* buffer = new BYTE[buf_size];
    unique_ptr<BYTE> buf_ptr(buffer);
    RtlZeroMemory(buffer, buf_size);

    rc = DoScsiInquiry(protocol, comid, buffer, buf_size);
    if (ERROR_SUCCESS != rc)
    {
        //todo: log....
        return rc;
    }
    ParseIndentify((PINQUIRYDATA)buffer);
    
    RtlZeroMemory(buffer, buf_size);
    rc = DoScsiInquiry3Vpd(protocol, comid, buffer, buf_size);
    if (ERROR_SUCCESS != rc)
    {
        //todo: log....
        return rc;
    }
    ParseIndentify((PVPD_SERIAL_NUMBER_PAGE)buffer);

    return ERROR_SUCCESS;
}

bool COpalNvme::LockGlobalRange(const char *pwd)
{
    UINT32 host_sid = GetHostSessionID();
    UINT32 tper_sid = StartSession(host_sid, OPAL_UID_TAG::LOCKINGSP, TRUE, (char*) pwd);
    DWORD error = ERROR_SUCCESS;
    BYTE* buffer = new BYTE[PAGE_SIZE];
    unique_ptr<BYTE> buf_ptr(buffer);

    if(0 == tper_sid)
        return false;

    CLockGlobalRange lock(host_sid, tper_sid, GetBaseComID());
    lock.PrepareCmd(TRUE, TRUE);

    memset(buffer, 0, PAGE_SIZE);
    lock.BuildOpalBuffer(buffer, PAGE_SIZE);
    error = DoScsiSecurityProtocolOut(1, GetBaseComID(), buffer, PAGE_SIZE);

    if (ERROR_SUCCESS != error)
        return 0;

    Sleep(50);  //wait 50ms for NVMe device complete last request.

    memset(buffer, 0, PAGE_SIZE);
    error = DoScsiSecurityProtocolIn(1, GetBaseComID(), buffer, PAGE_SIZE);
    if (ERROR_SUCCESS != error)
    {
        //todo: log....

    }

    //set Locking Range
    EndSession(host_sid, tper_sid);
    return true;
}

bool COpalNvme::LockGlobalRange(const wchar_t* pwd)
{
    std::wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> converter;
    string temp = converter.to_bytes((char16_t*)pwd);
    return LockGlobalRange(temp.c_str());
}

bool COpalNvme::UnlockGlobalRange(const char* pwd)
{
    UINT32 host_sid = GetHostSessionID();
    host_sid = 0;
    UINT32 tper_sid = StartSession(host_sid, OPAL_UID_TAG::LOCKINGSP, TRUE, (char*)pwd);
    DWORD error = ERROR_SUCCESS;
    BYTE* buffer = new BYTE[PAGE_SIZE];
    unique_ptr<BYTE> buf_ptr(buffer);

    if (0 == tper_sid)
        return false;

    CLockGlobalRange lock(host_sid, tper_sid, GetBaseComID());
    lock.PrepareCmd(FALSE, FALSE);
    memset(buffer, 0, PAGE_SIZE);
    lock.BuildOpalBuffer(buffer, PAGE_SIZE);
    error = DoScsiSecurityProtocolOut(1, GetBaseComID(), buffer, PAGE_SIZE);

    if (ERROR_SUCCESS != error)
        return 0;

    Sleep(50);  //wait 50ms for NVMe device complete last request.

    memset(buffer, 0, PAGE_SIZE);
    error = DoScsiSecurityProtocolIn(1, GetBaseComID(), buffer, PAGE_SIZE);
    if (ERROR_SUCCESS != error)
    {
        //todo: log....

    }

    //set Locking Range
    EndSession(host_sid, tper_sid);
    return true;
}
bool COpalNvme::UnlockGlobalRange(const wchar_t* pwd)
{
    std::wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> converter;
    string temp = converter.to_bytes((char16_t*)pwd);
    return UnlockGlobalRange(temp.c_str());
}

void COpalNvme::ParseDiscovery0(IN BYTE* buffer)
{
    PDISCOVERY0_HEADER header = (PDISCOVERY0_HEADER)buffer;
    PFEATURE_DESCRIPTOR desc = NULL;
    UCHAR* cursor = buffer;
    UCHAR* body = NULL;
    UINT32 total_size = header->GetParamLength();
    cursor += sizeof(DISCOVERY0_HEADER);
    body = cursor;
    desc = (PFEATURE_DESCRIPTOR)cursor;

    while (desc->Header.Length > 0 && (cursor - buffer) < total_size)
    {
//all data in OPAL response are BIG-ENDIAN.
//if responsed desc->Header.GetCode() can't be parsed, this device DEFINITELY not support this feature.
        switch (desc->Header.GetCode())
        {
        case TPer:
            RtlCopyMemory(&DevInfo.TPer, &desc->TPer, sizeof(FEATURE_DESC_TPer));
            break;
        case LOCKING:
            RtlCopyMemory(&DevInfo.Locking, &desc->Locking, sizeof(FEATURE_DESC_LOCKING));
            break;
        case GEOMETRY:
            RtlCopyMemory(&DevInfo.Geometry, &desc->Geometry, sizeof(FEATURE_DESC_GEOMETRY));
            break;
        case ENTERPRISE:
            if (desc->Header.GetCode() > DevFeature)
                DevFeature = (FEATURE_CODE)desc->Header.GetCode();
            RtlCopyMemory(&DevInfo.Enterprise, &desc->Enterprise, sizeof(FEATURE_DESC_ENTERPRISE_SSC));
            break;
        case OPAL_V100:
            if (desc->Header.GetCode() > DevFeature)
                DevFeature = (FEATURE_CODE)desc->Header.GetCode();
            RtlCopyMemory(&DevInfo.OpalV100, &desc->OpalV100, sizeof(FEATURE_DESC_OPAL_V100));
            break;
        case SINGLE_USER:
            RtlCopyMemory(&DevInfo.SingleUserMode, &desc->SingleUserMode, sizeof(FEATURE_DESC_SINGLE_USER_MODE));
            break;
        case DATASTORE:
            RtlCopyMemory(&DevInfo.Datastore, &desc->Datastore, sizeof(FEATURE_DESC_DATASTORE));
            break;
        case OPAL_V200:
            if (desc->Header.GetCode() > DevFeature)
                DevFeature = (FEATURE_CODE)desc->Header.GetCode();
            RtlCopyMemory(&DevInfo.OpalV200, &desc->OpalV200, sizeof(FEATURE_DESC_OPAL_V200));
            break;
        }

        //each block are splitted by a DWORD...
        cursor = cursor + desc->Header.Length + sizeof(DWORD);
        desc = (PFEATURE_DESCRIPTOR)cursor;
    }
}
void COpalNvme::ParseIndentify(PINQUIRYDATA data)
{
    RtlCopyMemory(DevInfo.ModelName, data->ProductId, sizeof(data->ProductId));
    RtlCopyMemory(DevInfo.FirmwareRev, data->ProductRevisionLevel, sizeof(data->ProductRevisionLevel));
}
void COpalNvme::ParseIndentify(PVPD_SERIAL_NUMBER_PAGE data)
{
    RtlCopyMemory(DevInfo.SerialNo, data->SerialNumber, data->PageLength);
}
UINT32 COpalNvme::StartSession(UINT32 host_sid, OPAL_UID_TAG provider, BOOLEAN is_write, char* pwd)
{
    //open session with password
    //sp stands for ServiceProvider
    //OPAL_UID_TAG sp = IsEnterprise()? OPAL_UID_TAG::ENT_LOCKINGSP : OPAL_UID_TAG::LOCKINGSP;
    UINT32 tper_sid = 0;
    DWORD error = ERROR_SUCCESS;
    CCmdStartSession start(host_sid, GetBaseComID());

    BYTE* cmd_buf = new BYTE[PAGE_SIZE];
    memset(cmd_buf, 0, PAGE_SIZE);
    start.PrepareCmd(provider, OPAL_UID_TAG::ADMIN1, is_write, pwd);
    start.BuildOpalBuffer(cmd_buf, PAGE_SIZE);
    error = DoScsiSecurityProtocolOut(1, GetBaseComID(), cmd_buf, PAGE_SIZE);
    delete[] cmd_buf;

    if (ERROR_SUCCESS != error)
        return 0;

    Sleep(50);  //wait 50ms for NVMe device complete last request.
    //sometimes buffer to read data from device SHOULD be allocated on heap, not stack....
    //but in sedutils, it requires aligned to 1024 ? wierd....
    BYTE* resp_buf = new BYTE[PAGE_SIZE];
    memset(resp_buf, 0, PAGE_SIZE);
    error = DoScsiSecurityProtocolIn(1, GetBaseComID(), resp_buf, PAGE_SIZE);
    COpalResponse resp(resp_buf, PAGE_SIZE);
    
    //In StartSession, HSN and TSN are replied from device.
    //replied ArgList of Response only have 2 data: first is "echo of HostSession ID", 
    //second is device's TPer Session ID
    list<COpalData *> list;
    resp.GetPayload(list);
    for(COpalData* item:list)
    {
        UINT32 data = 0;
        ((COpalDataAtom*)item)->GetUint(data);
        if(data != host_sid)
            tper_sid = data;
        delete item;
    }
    //

    delete[] resp_buf;

    if (ERROR_SUCCESS != error)
        return 0;

    //todo: check response data

    //return true;
    return tper_sid;
}
void COpalNvme::EndSession(UINT32 host_sid, UINT32 tper_sid)
{
    //end session
    CCmdEndSession end(host_sid, tper_sid, GetBaseComID());
    BYTE* cmd_buf = new BYTE[PAGE_SIZE];
    memset(cmd_buf, 0, PAGE_SIZE);

    end.BuildOpalBuffer(cmd_buf, PAGE_SIZE);
    DWORD error = DoScsiSecurityProtocolOut(1, GetBaseComID(), cmd_buf, PAGE_SIZE);
    delete[] cmd_buf;

    if (ERROR_SUCCESS != error)
        return;

    Sleep(50);  //wait 50ms for NVMe device complete last request.
    //sometimes buffer to read data from device SHOULD be allocated on heap, not stack....
    //but in sedutils, it requires aligned to 1024 ? wierd....
    BYTE* resp_buf = new BYTE[PAGE_SIZE];
    memset(resp_buf, 0, PAGE_SIZE);
    error = DoScsiSecurityProtocolIn(1, GetBaseComID(), resp_buf, PAGE_SIZE);
    delete[] resp_buf;

    if (ERROR_SUCCESS != error)
        return;

    //todo: check response data

    //return true;
}

bool COpalNvme::QueryTPerProperties(BYTE* resp, size_t resp_size)
{
    //send command directly
    //QueryTPerProperties use HSN==TSN==0,
    //use other values will fail this command.
    CCmdQueryProperties cmd(0, GetBaseComID());
#if 0
    COpalCommand cmd(SMUID, PROPERTIES, GetHostSessionID(), 0, GetBaseComID());
    COpalNamePair hostprop;
    COpalDataAtom name;
    COpalDataAtom value;
    COpalList value_list;
    COpalNamePair value_pair;
    UINT16 data_size = BIG_BUFFER_SIZE;

    UINT32 temp = 1;
    value.PutUint(temp);
    {
        name.PutString((char*)"MaxComPacketSize", (int) strlen("MaxComPacketSize"));
        value.PutUint(data_size);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }
    {
        name.PutString((char*)"MaxResponseComPacketSize", strlen("MaxResponseComPacketSize"));
        value.PutUint(data_size);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }
    {
        name.PutString((char*)"MaxPacketSize", strlen("MaxPacketSize"));
        data_size = BIG_BUFFER_SIZE - sizeof(COpalComPacket);
        value.PutUint(data_size);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }
    {
        name.PutString((char*)"MaxIndTokenSize", strlen("MaxIndTokenSize"));
        data_size = BIG_BUFFER_SIZE - sizeof(COpalComPacket) - sizeof(COpalPacket) - sizeof(COpalSubPacket);
        value.PutUint(data_size);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }
    {
        name.PutString((char*)"MaxPackets", strlen("MaxPackets"));
        value.PutUint((UINT8)1);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }
    {
        name.PutString((char*)"MaxSubPackets", strlen("MaxSubPackets"));
        value.PutUint((UINT8)1);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }
    {
        name.PutString((char*)"MaxMethods", strlen("MaxMethods"));
        value.PutUint((UINT8)1);
        value_pair.Set(name, &value);
        value_list.PushOpalItem(value_pair);
    }

    name.PutUint((UINT8) HOSTPROPERTIES);
    hostprop.Set(name, &value_list);

    cmd.PushCmdArg(hostprop);
    //cmd.CompleteCmd();

    //to read Properties, we should use queried BaseComID to replace ExtComID in ComPacket;
    cmd.SetBaseComID(GetBaseComID());
#endif
    BYTE cmd_buf[PAGE_SIZE] = {0};
    //BYTE *cmd_buf = new BYTE[PAGE_SIZE];
    //RtlZeroMemory(cmd_buf, PAGE_SIZE);
    //BYTE* temp2 = (BYTE*)ROUND_UP_ALIGN_2N((size_t)cmd_buf, PAGE_SIZE);

    cmd.PrepareCmd(PAGE_SIZE);

    size_t cmd_size = cmd.BuildOpalBuffer(cmd_buf, PAGE_SIZE);
    DWORD error = DoScsiSecurityProtocolOut(1, GetBaseComID(), cmd_buf, PAGE_SIZE);

    if(ERROR_SUCCESS != error)
        return false;

    Sleep(50);  //wait 50ms for NVMe device complete last request.
    //sometimes buffer to read data from device SHOULD be allocated on heap, not stack....
    //but in sedutils, it requires aligned to 1024 ? wierd....
    error = DoScsiSecurityProtocolIn(1, GetBaseComID(), resp, resp_size);

    return (error == ERROR_SUCCESS);
}
DWORD COpalNvme::SendCommand(DWORD ioctl, PVOID cmd_buf, size_t cmd_size)
{
    DWORD rc = ERROR_SUCCESS;
    DWORD return_len = 0;

    if (INVALID_HANDLE_VALUE == DevHandle)
    {

        DevHandle = CreateFile(DevPath.c_str(),
            GENERIC_WRITE | GENERIC_READ,
            FILE_SHARE_WRITE | FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
    }

    if (INVALID_HANDLE_VALUE == DevHandle)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), DevPath.c_str(), GetLastError());
        return ERROR_DEVICE_NOT_AVAILABLE;
    }

    //send cmd
    BOOL ok = DeviceIoControl(DevHandle,
        //IOCTL_SCSI_PASS_THROUGH_DIRECT,
        ioctl,
        cmd_buf,
        (DWORD)cmd_size,
        cmd_buf,
        (DWORD)cmd_size,
        &return_len,
        FALSE);

    if (!ok)
        rc = GetLastError();

    return rc;
}

DWORD COpalNvme::DoScsiSecurityProtocolIn(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size, sizeof(cdb->SECURITY_PROTOCOL_IN), SCSI_IOCTL_DATA_IN);
    FillScsiCDB_SecurityIn(cdb, protocol, comid, (ULONG)buf_size);

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
    //return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE));
}
DWORD COpalNvme::DoScsiSecurityProtocolOut(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size, (UCHAR)sizeof(cdb->SECURITY_PROTOCOL_OUT), SCSI_IOCTL_DATA_OUT);
    FillScsiCDB_SecurityOut(cdb, protocol, comid, (ULONG)buf_size);

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
//    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE));
}
DWORD COpalNvme::DoScsiInquiry(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size, (UCHAR)sizeof(cdb->CDB6INQUIRY), SCSI_IOCTL_DATA_IN);
    FillScsiCDB_Inquiry(cdb, (UCHAR)buf_size);

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
}
DWORD COpalNvme::DoScsiInquiry3Vpd(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size, (UCHAR)sizeof(cdb->CDB6INQUIRY3), SCSI_IOCTL_DATA_IN);
    FillScsiCDB_Inquiry3Vpd(cdb, (UCHAR)buf_size);

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
}
