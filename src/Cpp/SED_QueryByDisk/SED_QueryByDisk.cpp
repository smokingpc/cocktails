// SED_QueryByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

void Usage()
{
    _tprintf(_T("Usage: SED_QueryByDisk.exe <physical disk devpath>\n"));
    _tprintf(_T("example: SED_QueryByDisk.exe \\\\.\\PhysicalDrive3\n"));
    _tprintf(_T("\n"));
}

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

void PrintFeatureData(PFEATURE_DESC_TPer data)
{
    _tprintf(_T("[TPer]\n"));
    _tprintf(_T("SyncSupport(%d), AsyncSupport(%d), AckNackSupport(%d)\n"),
                data->SyncSupport, data->AsyncSupport, data->AckNackSupport);
    _tprintf(_T("BufferMgmtSupport(%d), StreamingSupport(%d), ComIdMgmtSupport(%d)\n"), 
                data->BufferMgmtSupport, data->StreamingSupport, data->ComIdMgmtSupport);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_LOCKING data)
{
    _tprintf(_T("[Locking]\n"));
    _tprintf(_T("LockingSupport(%d), LockingEnabled(%d), IsLocked(%d)\n"), 
                data->LockingSupport, data->LockingEnabled, data->IsLocked);
    _tprintf(_T("MediaEncryption(%d), MBREnabled(%d), MBRDone(%d)\n"), 
                data->MediaEncryption, data->MBREnabled, data->MBRDone);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_GEOMETRY data) 
{
    _tprintf(_T("[Geometry]\n"));
    _tprintf(_T("LogicalBlockSize(%d), AlignmentGranularity(%lld), LowestAlignedLBA(%lld)\n"),
                data->LogicalBlockSize, data->AlignmentGranularity, data->LowestAlignedLBA);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_ENTERPRISE_SSC data)
{
    _tprintf(_T("[Enterprise]\n"));
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"),data->BaseComID, data->NumberComIDs);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_SINGLE_USER_MODE data)
{
    _tprintf(_T("[SingleUser Mode]\n"));
    _tprintf(_T("NumberLockingObjects(%d), Any(%d), All(%d), Policy(%d)\n"),
                data->NumberLockingObjects, data->Any, data->All, data->Policy);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_OPAL_V100 data)
{
    _tprintf(_T("[Opal V100]\n"));
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"), data->BaseComID, data->NumberComIDs);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_OPAL_V200 data)
{
    _tprintf(_T("[Opal V200]\n"));
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"), data->BaseComID, data->NumberComIDs);
    _tprintf(_T("NumlockingAdminAuth(%d), NumlockingUserAuth(%d)\n"), data->NumlockingAdminAuth, data->NumlockingUserAuth);
    _tprintf(_T("InitialPIN(%d), RevertedPIN(%d)\n"), data->InitialPIN, data->RevertedPIN);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_DATASTORE data)
{
    _tprintf(_T("[Datastore]\n"));
    _tprintf(_T("MaxTables(%d), MaxSizeTables(%d), TableSizeAlignment(%d)\n"),
                data->MaxTables, data->MaxSizeTables, data->TableSizeAlignment);
    _tprintf(_T("\n"));
}
void PrintDiscoveryResult(BYTE buffer[])
{
    UCHAR*cursor = buffer;
    PFEATURE_DESCRIPTOR desc = (PFEATURE_DESCRIPTOR) cursor;
    while(desc->Header.Length > 0)
    {
        switch (desc->Header.GetFeatureCode())
        {
            case TPer:
                PrintFeatureData(&desc->TPer);
            break;
            case LOCKING:
                PrintFeatureData(&desc->Locking);
                break;
            case GEOMETRY:
                PrintFeatureData(&desc->Geometry);
                break;
            case ENTERPRISE:
                PrintFeatureData(&desc->Enterprise);
                break;
            case OPAL_V100:
                PrintFeatureData(&desc->SingleUserMode);
                break;
            case SINGLE_USER:
                PrintFeatureData(&desc->OpalV100);
                break;
            case DATASTORE:
                PrintFeatureData(&desc->OpalV200);
                break;
            case SSC_V2:
                PrintFeatureData(&desc->Datastore);
                break;
        }

        //each block are splitted by a DWORD...
        cursor = cursor + desc->Header.Length + sizeof(DWORD);
        desc = (PFEATURE_DESCRIPTOR)cursor;
    }
}

//Discovery0 is TCG defined "device discovery layer0" behavior.
bool Discovery0(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring &diskname)
{
    UCHAR protocol = 1;
    USHORT comid = 1;
    bool ok = false;
    DWORD ret_size = 0;
    HANDLE device = CreateFile(diskname.c_str(),
                        GENERIC_WRITE | GENERIC_READ | GENERIC_EXECUTE,
                        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                        NULL,
                        OPEN_EXISTING,
                        0,
                        NULL);

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
    cmd->SenseInfoOffset = (cmd->Length+(sizeof(DWORD)-1)/ sizeof(DWORD))* sizeof(DWORD);   //align to DWORD

    PCDB cdb = (PCDB)cmd->Cdb;
    cdb->SECURITY_PROTOCOL_IN.OperationCode = SCSIOP_SECURITY_PROTOCOL_IN;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocol = protocol;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific[0] = (comid&0xFF00)>>8;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific[1] = (comid & 0x00FF);
    cdb->SECURITY_PROTOCOL_IN.AllocationLength[0] = (UCHAR)((buf_size & 0xFF000000) >> 24);
    cdb->SECURITY_PROTOCOL_IN.AllocationLength[1] = (UCHAR)((buf_size & 0x00FF0000) >> 16);
    cdb->SECURITY_PROTOCOL_IN.AllocationLength[2] = (UCHAR)((buf_size & 0x0000FF00) >> 8);
    cdb->SECURITY_PROTOCOL_IN.AllocationLength[3] = (UCHAR)((buf_size & 0x000000FF));
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

    if(INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return ok;
}

int _tmain(int argc, TCHAR* argv[])
{

    if(argc<2)
    {
        Usage();
        return -1;
    }

    tstring diskname = argv[1];
    ShowStructureSizes();

    BYTE *buffer = new BYTE[BIG_BUFFER_SIZE];
    RtlZeroMemory(buffer, BIG_BUFFER_SIZE);

    if(Discovery0(buffer, BIG_BUFFER_SIZE, diskname))
    {
        PrintDiscoveryResult(buffer);
    }

    delete[] buffer;
}
