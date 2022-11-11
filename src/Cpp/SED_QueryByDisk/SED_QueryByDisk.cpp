// SED_QueryByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

void Usage()
{
    _tprintf(_T("Usage: SED_QueryByDisk.exe <physical disk devpath>\n"));
    _tprintf(_T("example: SED_QueryByDisk.exe \\\\.\\PhysicalDrive3\n"));
    _tprintf(_T("\n"));
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
    PDISCOVERY0_HEADER header = (PDISCOVERY0_HEADER) buffer;
    PFEATURE_DESCRIPTOR desc = NULL;
    UCHAR*cursor = buffer;
    UCHAR* body = NULL;
    UINT32 total_size = header->GetParamLength();
    cursor += sizeof(DISCOVERY0_HEADER);
    body = cursor;
    desc = (PFEATURE_DESCRIPTOR) cursor;

    while(desc->Header.Length > 0 && (cursor-buffer)< total_size)
    {
        switch (desc->Header.Code)
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
                PrintFeatureData(&desc->OpalV100);
                break;
            case SINGLE_USER:
                PrintFeatureData(&desc->SingleUserMode);
                break;
            case DATASTORE:
                PrintFeatureData(&desc->Datastore);
                break;
            case OPAL_V200:
                PrintFeatureData(&desc->OpalV200);
                break;
        }

        //each block are splitted by a DWORD...
        cursor = cursor + desc->Header.Length + sizeof(DWORD);
        desc = (PFEATURE_DESCRIPTOR)cursor;
    }
}

int _tmain(int argc, TCHAR* argv[])
{
    list<tstring> disklist;
    EnumPhysicalDisks(disklist);
    for(auto &diskname : disklist)
    { 
        BYTE *buffer = NULL;
        STORAGE_BUS_TYPE type = BusTypeUnknown;
        if(!IdentifyStorageType(type, diskname))
        {
            _tprintf(_T("Identify storage type of specified disk [%s] failed.\n"), diskname.c_str());
            return -1;
        }

        //Identify storage type by "BusType of PhysicalDrive"
        switch(type)
        {
            case BusTypeAta:
            case BusTypeSata:
            //case BusTypeRAID:   //don't use this cmd in RAID controller, even it is SATA raid. RAID will reply NO DATA.
            {
                buffer = new BYTE[BIG_BUFFER_SIZE];
                RtlZeroMemory(buffer, BIG_BUFFER_SIZE);
                if (Discovery0_SATA(buffer, BIG_BUFFER_SIZE, diskname))
                {
                    PrintDiscoveryResult(buffer);
                }
                delete[] buffer;
                break;
            }
            case BusTypeNvme:
            {
                buffer = new BYTE[BIG_BUFFER_SIZE];
                RtlZeroMemory(buffer, BIG_BUFFER_SIZE);
                if (Discovery0_NVMe(buffer, BIG_BUFFER_SIZE, diskname))
                {
                    PrintDiscoveryResult(buffer);
                }
                delete[] buffer;
                break;
            }
        }
    }
}
