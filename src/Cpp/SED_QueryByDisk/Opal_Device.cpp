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

static void FillScsiCmd(PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd, PVOID buffer, ULONG buf_size)
{
    cmd->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    cmd->PathId = 0;
    cmd->TargetId = 1;
    cmd->Lun = 0;
    cmd->DataTransferLength = buf_size; //length of cmd->DataBuffer.
    cmd->TimeOutValue = 3;      //in seconds
    cmd->DataBuffer = buffer;
    cmd->SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE, SenseInfo);
    cmd->SenseInfoLength = sizeof(cmd->SenseInfo);
}

COpalDevice::COpalDevice(tstring devpath)
{
    DevPath = devpath;
}
COpalDevice::~COpalDevice(){}

DWORD COpalDevice::OpenSession()
{
    return ERROR_NOT_SUPPORTED; 
}
void COpalDevice::CloseSession(){}

COpalNvme::COpalNvme(tstring devpath) : COpalDevice(devpath)
{
    Discovery0();
    Identify();

    DevInfo.Type = BusTypeNvme;
}
COpalNvme::~COpalNvme(){}

DWORD COpalNvme::Discovery0()
{
    //discovery0() use SCSI_OP==SCSIOP_SECURITY_PROTOCOL_IN with empty payload to ask 
    //device discovery information.
    UCHAR protocol = 1;
    UINT16 comid = 1;
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
void COpalNvme::ParseIndentify(PINQUIRYDATA data)
{
    RtlCopyMemory(DevInfo.ModelName, data->ProductId, sizeof(data->ProductId));
    RtlCopyMemory(DevInfo.FirmwareRev, data->ProductRevisionLevel, sizeof(data->ProductRevisionLevel));
}
void COpalNvme::ParseIndentify(PVPD_SERIAL_NUMBER_PAGE data)
{
    RtlCopyMemory(DevInfo.SerialNo, data->SerialNumber, data->PageLength);
}

DWORD COpalNvme::SendCommand(DWORD ioctl, PVOID cmd_buf, size_t cmd_size)
{
    DWORD rc = ERROR_SUCCESS;
    DWORD return_len = 0;
    HANDLE device = CreateFile(DevPath.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), DevPath.c_str(), GetLastError());
        return ERROR_DEVICE_NOT_AVAILABLE;
    }

    //send cmd
    BOOL ok = DeviceIoControl(device,
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
    CloseHandle(device);

    return rc;
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
            if (desc->Header.Code > UseFeature)
                UseFeature = (FEATURE_CODE)desc->Header.Code;
            RtlCopyMemory(&DevInfo.Enterprise, &desc->Enterprise, sizeof(FEATURE_DESC_ENTERPRISE_SSC));
            break;
        case OPAL_V100:
            if (desc->Header.Code > UseFeature)
                UseFeature = (FEATURE_CODE)desc->Header.Code;
            RtlCopyMemory(&DevInfo.OpalV100, &desc->OpalV100, sizeof(FEATURE_DESC_OPAL_V100));
            break;
        case SINGLE_USER:
            RtlCopyMemory(&DevInfo.SingleUserMode, &desc->SingleUserMode, sizeof(FEATURE_DESC_SINGLE_USER_MODE));
            break;
        case DATASTORE:
            RtlCopyMemory(&DevInfo.Datastore, &desc->Datastore, sizeof(FEATURE_DESC_DATASTORE));
            break;
        case OPAL_V200:
            if (desc->Header.Code > UseFeature)
                UseFeature = (FEATURE_CODE)desc->Header.Code;
            RtlCopyMemory(&DevInfo.OpalV200, &desc->OpalV200, sizeof(FEATURE_DESC_OPAL_V200));
            break;
        }

        //each block are splitted by a DWORD...
        cursor = cursor + desc->Header.Length + sizeof(DWORD);
        desc = (PFEATURE_DESCRIPTOR)cursor;
    }
}

DWORD COpalNvme::DoScsiSecurityProtocolIn(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size);
    FillScsiCDB_SecurityIn(cdb, protocol, comid, (ULONG)buf_size);
    cmd->CdbLength = sizeof(cdb->SECURITY_PROTOCOL_IN);
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
}
DWORD COpalNvme::DoScsiSecurityProtocolOut(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size);
    FillScsiCDB_SecurityOut(cdb, protocol, comid, (ULONG)buf_size);
    cmd->CdbLength = sizeof(cdb->SECURITY_PROTOCOL_OUT);
    cmd->DataIn = SCSI_IOCTL_DATA_OUT;

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
}
DWORD COpalNvme::DoScsiInquiry(IN UCHAR protocol, IN UINT16 comid, IN BYTE* opal_buf, IN size_t buf_size)
{
    DWORD cmd_size = SMALL_BUFFER_SIZE;
    unique_ptr<BYTE> cmd_ptr(new BYTE[cmd_size]);
    RtlZeroMemory(cmd_ptr.get(), cmd_size);

    PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd =
        (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    PCDB cdb = (PCDB)cmd->Cdb;

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size);
    FillScsiCDB_Inquiry(cdb, (UCHAR)buf_size);
    cmd->CdbLength = sizeof(cdb->CDB6INQUIRY);
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

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

    FillScsiCmd(cmd, opal_buf, (ULONG)buf_size);
    FillScsiCDB_Inquiry3Vpd(cdb, (UCHAR)buf_size);
    cmd->CdbLength = sizeof(cdb->CDB6INQUIRY3);
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

    return SendCommand(IOCTL_SCSI_PASS_THROUGH_DIRECT, cmd_ptr.get(), cmd_size);
}
