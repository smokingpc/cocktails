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

void ParseIdentifyResult(IN OUT OPAL_DISKINFO& diskinfo, IN PINQUIRYDATA data)
{
    //INQUIRYDATA
    RtlCopyMemory(diskinfo.ProductRev, data->ProductRevisionLevel, 4);
    RtlCopyMemory(diskinfo.ProductName, data->ProductId, 16);

}
void ParseIdentifyResult(IN OUT OPAL_DISKINFO& diskinfo, IN PVPD_SERIAL_NUMBER_PAGE data)
{
    size_t size = data->PageLength - sizeof(VPD_SERIAL_NUMBER_PAGE);
    RtlCopyMemory(diskinfo.SN, data->SerialNumber, size);
}

void ParseIdentifyResult(IN OUT OPAL_DISKINFO& diskinfo, IN PATA_IDENTIFY_RESP data)
{}

void ParseDiscoveryResult(IN OUT OPAL_DISKINFO& diskinfo, IN BYTE buffer[])
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
        switch (desc->Header.GetCode())
        {
        case TPer:
            RtlCopyMemory(&diskinfo.TPer, &desc->TPer, sizeof(FEATURE_DESC_TPer));
            break;
        case LOCKING:
            RtlCopyMemory(&diskinfo.Locking, &desc->Locking, sizeof(FEATURE_DESC_LOCKING));
            break;
        case GEOMETRY:
            RtlCopyMemory(&diskinfo.Geometry, &desc->Geometry, sizeof(FEATURE_DESC_GEOMETRY));
            break;
        case ENTERPRISE:
            RtlCopyMemory(&diskinfo.Enterprise, &desc->Enterprise, sizeof(FEATURE_DESC_ENTERPRISE_SSC));
            break;
        case OPAL_V100:
            RtlCopyMemory(&diskinfo.OpalV100, &desc->OpalV100, sizeof(FEATURE_DESC_OPAL_V100));
            break;
        case SINGLE_USER:
            RtlCopyMemory(&diskinfo.SingleUserMode, &desc->SingleUserMode, sizeof(FEATURE_DESC_SINGLE_USER_MODE));
            break;
        case DATASTORE:
            RtlCopyMemory(&diskinfo.Datastore, &desc->Datastore, sizeof(FEATURE_DESC_DATASTORE));
            break;
        case OPAL_V200:
            RtlCopyMemory(&diskinfo.OpalV200, &desc->OpalV200, sizeof(FEATURE_DESC_OPAL_V200));
            break;
        }

        //each block are splitted by a DWORD...
        cursor = cursor + desc->Header.Length + sizeof(DWORD);
        desc = (PFEATURE_DESCRIPTOR)cursor;
    }
}


//Discovery0 is TCG defined "device discovery layer0" behavior.
//bool Discovery0_NVMe(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
bool Discovery0_NVMe(IN OUT OPAL_DISKINFO &info, IN tstring& diskname)
{
    UCHAR protocol = 1;
    USHORT comid = 1;
    bool ok = false;
    DWORD ret_size = 0;
    DWORD buf_size = BIG_BUFFER_SIZE;
    UINT8* buffer = NULL;
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

    buffer = new UINT8[buf_size];
    RtlZeroMemory(buffer, buf_size);
    PSCSI_PASS_THROUGH_DIRECT cmd = (PSCSI_PASS_THROUGH_DIRECT) new BYTE[SMALL_BUFFER_SIZE];
    RtlZeroMemory(cmd, SMALL_BUFFER_SIZE);
    cmd->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    cmd->PathId = 0;
    cmd->TargetId = 1;
    cmd->Lun = 0;
    cmd->DataTransferLength = buf_size;
    cmd->TimeOutValue = 3;      //in seconds
    cmd->DataBuffer = buffer;
    cmd->SenseInfoOffset = ((cmd->Length + sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);   //align to DWORD
    cmd->SenseInfoLength = SMALL_BUFFER_SIZE - cmd->SenseInfoOffset;

    PCDB cdb = (PCDB)cmd->Cdb;
    cdb->SECURITY_PROTOCOL_IN.OperationCode = SCSIOP_SECURITY_PROTOCOL_IN;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocol = protocol;

    //SecurityProtocolSpecific and AllocationLength field are BIG-ENDIAN
    SwapEndian(&comid, (USHORT*)cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific);
    SwapEndian(&buf_size, (ULONG*)cdb->SECURITY_PROTOCOL_IN.AllocationLength);
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

    if (ok)
        ParseDiscoveryResult(info, buffer);
    else
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);

    delete[] cmd;
    delete[] buffer;
    return ok;
}

//bool Discovery0_SATA(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
bool Discovery0_SATA(IN OUT OPAL_DISKINFO& info, IN tstring& diskname)
{
    USHORT comid = 0x0001;
    UINT8 cmd_id = 1;
    UINT8 protocol = 1;
    bool ok = false;
    DWORD buf_size = BIG_BUFFER_SIZE;
    UINT8* buffer = NULL;
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

    buffer = new UINT8[buf_size];
    RtlZeroMemory(buffer, buf_size);

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

    if (ok)
        ParseDiscoveryResult(info, buffer);
    else
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);

    delete[] cmd;
    delete[] buffer;
    return ok;
}

//bool Identify_NVMe(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
bool Identify_NVMe(IN OUT OPAL_DISKINFO& info, IN tstring& diskname)
{
    UCHAR protocol = 1;
    USHORT comid = 1;
    bool ok = false;
    DWORD buf_size = TINY_BUFFER_SIZE;
    UINT8* buffer = NULL;
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

    buffer = new UINT8[buf_size];
    RtlZeroMemory(buffer, buf_size);

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
    cdb->CDB6INQUIRY.OperationCode = SCSIOP_INQUIRY;
    cdb->CDB6INQUIRY.AllocationLength = (UCHAR)buf_size;
    cmd->CdbLength = 6;
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

    ok = DeviceIoControl(device,
        IOCTL_SCSI_PASS_THROUGH_DIRECT,
        cmd,
        SMALL_BUFFER_SIZE,
        cmd,
        SMALL_BUFFER_SIZE,
        &ret_size,
        FALSE);

    if (ok)
        ParseIdentifyResult(info, (PINQUIRYDATA)buffer);
    else
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

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

    cdb->CDB6INQUIRY3.OperationCode = SCSIOP_INQUIRY;
    cdb->CDB6INQUIRY3.EnableVitalProductData = 1;
    cdb->CDB6INQUIRY3.PageCode = VPD_SERIAL_NUMBER;
    cdb->CDB6INQUIRY3.AllocationLength = (UCHAR)buf_size;
    cmd->CdbLength = 6;
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

    ok = DeviceIoControl(device,
        IOCTL_SCSI_PASS_THROUGH_DIRECT,
        cmd,
        SMALL_BUFFER_SIZE,
        cmd,
        SMALL_BUFFER_SIZE,
        &ret_size,
        FALSE);

    if (ok)
        ParseIdentifyResult(info, (PVPD_SERIAL_NUMBER_PAGE)buffer);
    else
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);

    delete[] cmd;
    delete[] buffer;
    return ok;
}

//bool Identify_SATA(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
bool Identify_SATA(IN OUT OPAL_DISKINFO& info, IN tstring& diskname)
{
    USHORT comid = 0x0001;
    UINT8 cmd_id = 1;
    UINT8 protocol = 1;
    bool ok = false;
    DWORD buf_size = BIG_BUFFER_SIZE;
    UINT8* buffer = NULL;
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

    buffer = new UINT8[buf_size];
    RtlZeroMemory(buffer, buf_size);

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
    if (ok)
        ParseIdentifyResult(info, (PATA_IDENTIFY_RESP)buffer);
    else
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);

    delete[] buffer;
    return ok;
}
