#include "Common.h"

void ShowStructureSizes()
{
    _tprintf(_T("sizeof(FEATURE_DESCRIPTOR) = %lld\n"), sizeof(FEATURE_DESCRIPTOR));
    _tprintf(_T("sizeof(FEATURE_DESC_HEADER) = %lld\n"), sizeof(FEATURE_DESC_HEADER));
    _tprintf(_T("\n"));
    _tprintf(_T("sizeof(FEATURE_DESC_TPer) = %lld\n"), sizeof(FEATURE_DESC_TPer));
    _tprintf(_T("sizeof(FEATURE_DESC_LOCKING) = %lld\n"), sizeof(FEATURE_DESC_LOCKING));
    _tprintf(_T("sizeof(FEATURE_DESC_GEOMETRY) = %lld\n"), sizeof(FEATURE_DESC_GEOMETRY));
    _tprintf(_T("sizeof(FEATURE_DESC_ENTERPRISE_SSC) = %lld\n"), sizeof(FEATURE_DESC_ENTERPRISE_SSC));
    _tprintf(_T("sizeof(FEATURE_DESC_SINGLE_USER_MODE) = %lld\n"), sizeof(FEATURE_DESC_SINGLE_USER_MODE));
    _tprintf(_T("sizeof(FEATURE_DESC_OPAL_V100) = %lld\n"), sizeof(FEATURE_DESC_OPAL_V100));
    _tprintf(_T("sizeof(FEATURE_DESC_OPAL_V200) = %lld\n"), sizeof(FEATURE_DESC_OPAL_V200));
    _tprintf(_T("sizeof(FEATURE_DESC_DATASTORE) = %lld\n"), sizeof(FEATURE_DESC_DATASTORE));
}

//Discovery0 is TCG defined "device discovery layer0" behavior.
bool Discovery0_NVMe(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
{
    UCHAR protocol = 1;
    USHORT comid = 1;
    bool ok = false;
    DWORD ret_size = 0;
    HANDLE device = CreateFile(diskname.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), diskname.c_str(), GetLastError());
        return false;
    }

    PSCSI_PASS_THROUGH_DIRECT cmd = (PSCSI_PASS_THROUGH_DIRECT) new BYTE[SMALL_BUFFER_SIZE];
    RtlZeroMemory(cmd, SMALL_BUFFER_SIZE);
    cmd->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    cmd->PathId = 0;
    cmd->TargetId = 1;
    cmd->Lun = 0;
    cmd->SenseInfoLength = 32;
    cmd->DataTransferLength = buf_size;
    cmd->TimeOutValue = 5;      //in seconds
    cmd->DataBuffer = buffer;
    cmd->SenseInfoOffset = (cmd->Length + (sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);   //align to DWORD

    PCDB cdb = (PCDB)cmd->Cdb;
    cdb->SECURITY_PROTOCOL_IN.OperationCode = SCSIOP_SECURITY_PROTOCOL_IN;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocol = protocol;

    //SecurityProtocolSpecific and AllocationLength field are BIG-ENDIAN
    SwapEndian(&comid, (USHORT*)cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific);
    SwapEndian(&buf_size, (ULONG*)cdb->SECURITY_PROTOCOL_IN.AllocationLength);
    //cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific[0] = (comid&0xFF00)>>8;
    //cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific[1] = (comid & 0x00FF);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[0] = (UCHAR)((buf_size & 0xFF000000) >> 24);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[1] = (UCHAR)((buf_size & 0x00FF0000) >> 16);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[2] = (UCHAR)((buf_size & 0x0000FF00) >> 8);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[3] = (UCHAR)((buf_size & 0x000000FF));
    cmd->CdbLength = sizeof(cdb->SECURITY_PROTOCOL_IN);
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

    ok = DeviceIoControl(device,
        IOCTL_SCSI_PASS_THROUGH_DIRECT,
        cmd,
        SMALL_BUFFER_SIZE,
        cmd,
        SMALL_BUFFER_SIZE,
        &ret_size,
        FALSE);

    delete[] cmd;
    if (!ok)
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return ok;
}
bool Discovery0_SATA(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
{
    //UCHAR protocol = 1;
    USHORT comid = 0x0001;
    UINT8 cmd_id = 1;
    UINT8 protocol = 1;
    bool ok = false;
    DWORD ret_size = 0;
    HANDLE device = CreateFile(diskname.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), diskname.c_str(), GetLastError());
        return false;
    }

    PATA_PASS_THROUGH_DIRECT cmd = new ATA_PASS_THROUGH_DIRECT();
    RtlZeroMemory(cmd, sizeof(ATA_PASS_THROUGH_DIRECT));
    cmd->Length = sizeof(ATA_PASS_THROUGH_DIRECT);

    //info query and discovery0 are ATA_FLAGS_DATA_IN
    cmd->AtaFlags = ATA_FLAGS_DATA_IN;
    cmd->DataTransferLength = buf_size;
    cmd->DataBuffer = buffer;
    cmd->TimeOutValue = 5;      //in seconds
    //Roy : I don't understand why fill such values here...
    //copied from SEDUTIL src
    cmd->CurrentTaskFile[0] = protocol; // Protocol
    cmd->CurrentTaskFile[1] = (UINT8)(buf_size / 512); // how many "sectors" does Payload have?
    SwapEndian(&comid, (UINT16*)&cmd->CurrentTaskFile[3]);
    cmd->CurrentTaskFile[6] = (UCHAR)ATACOMMAND::IF_RECV;

    ok = DeviceIoControl(device,
        IOCTL_ATA_PASS_THROUGH_DIRECT,
        cmd,
        sizeof(ATA_PASS_THROUGH_DIRECT),
        cmd,
        sizeof(ATA_PASS_THROUGH_DIRECT),
        &ret_size,
        FALSE);

    delete[] cmd;
    if (!ok)
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return ok;
}
