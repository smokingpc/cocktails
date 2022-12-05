// SED_LockDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"
using namespace std;

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
    _tprintf(_T("BaseComID(%d), NumberComIDs(%d)\n"), data->GetBaseComID(), data->GetNumberComIDs());
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


void PrintOpalDeviceInfo(tstring& diskname, OPAL_DEVICE_INFO& info)
{
    _tprintf(_T("===> %s\n"), diskname.c_str());
    _tprintf(_T("Model [%S], Rev [%S]\n"), info.ModelName, info.FirmwareRev);
    _tprintf(_T("SN [%S]\n"), info.SerialNo);
    PrintFeatureData(&info.TPer);
    PrintFeatureData(&info.Locking);
    PrintFeatureData(&info.Geometry);
    PrintFeatureData(&info.Enterprise);
    PrintFeatureData(&info.OpalV100);
    PrintFeatureData(&info.SingleUserMode);
    PrintFeatureData(&info.Datastore);
    PrintFeatureData(&info.OpalV200);
}

void Usage()
{
    _tprintf(_T("Usage:\n"));
    _tprintf(_T("SED_LockDisk.exe [password] [TRUE | FALSE] [physical disk name]\n\n"));
    _tprintf(_T("Example:\n"));
    _tprintf(_T("1. use password \"1234\" to set Readonly lock \\\\.\\PhysicalDrive1 , try\n"));
    _tprintf(_T("\t SED_LockDisk.exe 1234 TRUE \\\\.\\PhysicalDrive1\n"));
    _tprintf(_T("2. use password \"5678\" to unlock \\\\.\\PhysicalDrive7 to normal read-write , try\n"));
    _tprintf(_T("\t SED_LockDisk.exe 5678 FALSE \\\\.\\PhysicalDrive7\n\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    if(argc < 4)
    {
        Usage();
        return -1;
    }
    
    tstring pwd = argv[1];
    tstring diskname = argv[3];
    bool locking = false;

    if(0==_tcsicmp(argv[2], _T("TRUE")))
        locking = true;

    COpalDevice *dev = new COpalNvme(diskname);
    if(locking)
        dev->LockGlobalRange(pwd.c_str());
    else
        dev->UnlockGlobalRange(pwd.c_str());

    delete[] dev;
}


