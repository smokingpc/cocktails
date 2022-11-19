#include "Common.h"

static void FillDiscovery0ScsiCmd(PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd, 
                            PVOID buffer, ULONG buffer_size, UCHAR protocol, UINT16 base_comid)
{
    cmd->Length = sizeof(PSCSI_PASS_THROUGH_DIRECT);
    cmd->PathId = 0;
    cmd->TargetId = 1;
    cmd->Lun = 0;
    cmd->DataTransferLength = buffer_size; //length of cmd->DataBuffer.
    cmd->TimeOutValue = 3;      //in seconds
    cmd->DataBuffer = buffer;
    //cmd->SenseInfoOffset = ((cmd->Length + sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);   //align to DWORD
    cmd->SenseInfoOffset = offsetof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE, SenseInfo);
    cmd->SenseInfoLength = sizeof(cmd->SenseInfo);

    PCDB cdb = (PCDB)cmd->Cdb;
    cdb->SECURITY_PROTOCOL_IN.OperationCode = SCSIOP_SECURITY_PROTOCOL_IN;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocol = protocol;

    //SecurityProtocolSpecific and AllocationLength field are BIG-ENDIAN
    SwapEndian(&base_comid, (UINT16*)cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific);
    SwapEndian(&buffer_size, (ULONG*)cdb->SECURITY_PROTOCOL_IN.AllocationLength);
    cmd->CdbLength = sizeof(cdb->SECURITY_PROTOCOL_IN);
    cmd->DataIn = SCSI_IOCTL_DATA_IN;
}




COpalDevice::COpalDevice(tstring devpath)
{
    DevPath = devpath;
}
COpalDevice::~COpalDevice(){}

DWORD COpalDevice::OpenSession(){}
void COpalDevice::CloseSession(){}

DWORD COpalDevice::SendCommand(){}




COpalNvme::COpalNvme(tstring devpath) : COpalDevice(devpath)
{
    Discovery0();
    Identify();
}
COpalNvme::~COpalNvme(){}

DWORD COpalNvme::ReadOpalData(){}
//Write config and properties to device, via OPAL commands.
DWORD COpalNvme::WriteOpalData(){}

DWORD COpalNvme::Discovery0()
{
    UCHAR security_protocol = 1;
    UINT16 comid = 1;
    BYTE *buffer = NULL;
    DWORD buf_size = 0;
    DWORD ret_size = 0;
    DWORD rc = ERROR_SUCCESS;
    unique_ptr<BYTE> buf_ptr;
    unique_ptr<BYTE> cmd_ptr;

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

    buf_ptr.reset(new BYTE[BIG_BUFFER_SIZE]);
    //buffer = new BYTE[buf_size];
    RtlZeroMemory(buf_ptr.get(), BIG_BUFFER_SIZE);

    cmd_ptr.reset(new BYTE[SMALL_BUFFER_SIZE]);
    RtlZeroMemory(cmd_ptr.get(), SMALL_BUFFER_SIZE);
    //PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE cmd = (PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get();
    FillDiscovery0ScsiCmd((PSCSI_PASS_THROUGH_DIRECT_WITH_SENSE)cmd_ptr.get(), 
                        buf_ptr.get(), BIG_BUFFER_SIZE, security_protocol, comid);

    //send cmd
    BOOL ok = DeviceIoControl(device,
        IOCTL_SCSI_PASS_THROUGH_DIRECT,
        cmd_ptr.get(),
        sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE),
        cmd_ptr.get(),
        sizeof(SCSI_PASS_THROUGH_DIRECT_WITH_SENSE),
        &ret_size,
        FALSE);

    if(!ok)
        rc = GetLastError();
    CloseHandle(device);

    return rc;
}

DWORD COpalNvme::Identify(){}

