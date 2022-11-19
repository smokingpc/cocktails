// SED_QueryByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"

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
    _tprintf(_T("Algined(%d), LogicalBlockSize(%d), AlignmentGranularity(%lld), LowestAlignedLBA(%lld)\n"),
                data->Aligned, data->GetLogicalBlockSize(), 
                data->GetAlignmentGranularity(), data->GetLowestAlignedLBA());
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_ENTERPRISE_SSC data)
{
    _tprintf(_T("[Enterprise]\n"));
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"),data->GetBaseComID(), data->GetNumberComIDs());
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_SINGLE_USER_MODE data)
{
    _tprintf(_T("[SingleUser Mode]\n"));
    _tprintf(_T("NumberLockingObjects(%d), Any(%d), All(%d), Policy(%d)\n"),
                data->GetNumberLockingObjects(), data->Any, data->All, data->Policy);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_OPAL_V100 data)
{
    _tprintf(_T("[Opal V100]\n"));
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"), data->GetBaseComID(), data->GetNumberComIDs());
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_OPAL_V200 data)
{
    _tprintf(_T("[Opal V200]\n"));
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"), data->GetBaseComID(), data->GetNumberComIDs());
    _tprintf(_T("NumlockingAdminAuth(%d), NumlockingUserAuth(%d)\n"), data->GetNumlockingAdminAuth(), data->GetNumlockingUserAuth());
    _tprintf(_T("InitialPIN(%d), RevertedPIN(%d)\n"), data->InitialPIN, data->RevertedPIN);
    _tprintf(_T("\n"));
}
void PrintFeatureData(PFEATURE_DESC_DATASTORE data)
{
    _tprintf(_T("[Datastore]\n"));
    _tprintf(_T("MaxTables(%d), MaxSizeTables(%d), TableSizeAlignment(%d)\n"),
                data->GetMaxTables(), data->GetMaxSizeTables(), data->GetTableSizeAlignment());
    _tprintf(_T("\n"));
}


void PrintDiskInfo(tstring &diskname, OPAL_DISKINFO& diskinfo)
{
    _tprintf(_T("===> %s\n"), diskname.c_str());
    _tprintf(_T("Model [%S], Rev [%S]\n"), diskinfo.ProductName, diskinfo.ProductRev);
    _tprintf(_T("SN [%S]\n"), diskinfo.SN);
    PrintFeatureData(&diskinfo.TPer);
    PrintFeatureData(&diskinfo.Locking);
    PrintFeatureData(&diskinfo.Geometry);
    PrintFeatureData(&diskinfo.Enterprise);
    PrintFeatureData(&diskinfo.OpalV100);
    PrintFeatureData(&diskinfo.SingleUserMode);
    PrintFeatureData(&diskinfo.Datastore);
    PrintFeatureData(&diskinfo.OpalV200);
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
                OPAL_DISKINFO info = { 0 };
                Identify_SATA(info, diskname);
                Discovery0_SATA(info, diskname);
                PrintDiskInfo(diskname, info);
                break;
            }
            case BusTypeNvme:
            {
                OPAL_DISKINFO info = { 0 };
                Identify_NVMe(info, diskname);
                Discovery0_NVMe(info, diskname);
                PrintDiskInfo(diskname, info);
                break;
            }
        }
    }
}
