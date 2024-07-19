// SED_QueryByDisk.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Common.h"
void PrintOpalList(COpalList* data);
void PrintOpalNamePair(COpalNamePair* data);

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


void PrintOpalDeviceInfo(tstring &diskname, OPAL_DEVICE_INFO& diskinfo)
{
    _tprintf(_T("===> %s\n"), diskname.c_str());
    _tprintf(_T("Model [%S], Rev [%S]\n"), diskinfo.ModelName, diskinfo.FirmwareRev);
    _tprintf(_T("SN [%S]\n"), diskinfo.SerialNo);
    PrintFeatureData(&diskinfo.TPer);
    PrintFeatureData(&diskinfo.Locking);
    PrintFeatureData(&diskinfo.Geometry);
    PrintFeatureData(&diskinfo.Enterprise);
    PrintFeatureData(&diskinfo.OpalV100);
    PrintFeatureData(&diskinfo.SingleUserMode);
    PrintFeatureData(&diskinfo.Datastore);
    PrintFeatureData(&diskinfo.OpalV200);
}

void PrintOpalComPacket(COpalComPacket &compkt)
{
    UINT32 Reserved = 0;
    UINT8 ExtComID[4] = { 0 };      //big endian
    UINT32 OutstandingData = 0;     //big endian
    UINT32 MinTx = 0;               //big endian
    UINT32 Length = 0;              //(big endian) ==> sizeof(OPAL_PACKET) + OPAL_PACKET::Length

    _tprintf(_T("=> compkt: ExtComID=0x%02X 0x%02X 0x%02X 0x%02X, OutstandingData=%d\n"), 
                compkt.ExtComID[0], compkt.ExtComID[1], compkt.ExtComID[2], compkt.ExtComID[3], 
                SwapEndian(compkt.OutstandingData));

    _tprintf(_T("=> compkt: MinTx=%d, Length=%d(0x%08X)\n"),
        SwapEndian(compkt.MinTx), SwapEndian(compkt.Length), SwapEndian(compkt.Length));
}
void PrintOpalPacket(COpalPacket& pkt)
{
    _tprintf(_T("==> pkt: SessionID = 0x%llX(TSN=0x%08d:HSN=0x%08d)\n"),
        SwapEndian(pkt.SessionID), SwapEndian(pkt.TSN), SwapEndian(pkt.HSN));

    _tprintf(_T("==> pkt: SeqNo=%d, AckType=%d, Ack=%d, Length=%d(0x%08X)\n"),
        SwapEndian(pkt.SeqNo), SwapEndian(pkt.AckType), SwapEndian(pkt.Ack), 
        SwapEndian(pkt.Length), SwapEndian(pkt.Length));
}
void PrintOpalSubPacket(COpalSubPacket& subpkt)
{
    UINT16 Kind = 0;        //??
    UINT32 Length = 0;      //total length of following DataPayload block, NOT INCLUDE padding...

    _tprintf(_T("===> subpkt: Kind=%d, Length=%d(0x%08X)\n"),
        SwapEndian(subpkt.Kind), SwapEndian(subpkt.Length), SwapEndian(subpkt.Length));
}

void PrintOpalDataAtom(COpalDataAtom *atom)
{
    if(atom->IsTiny())
    {
        UINT8 data = 0;
        atom->GetUint(data);
        _tprintf(_T("DataAtom => TinyAtom [0x%02X](%d)"), data, data);
    }
    else if (atom->IsNumeric())
    {
        UINT64 data = 0;
        atom->GetUint(data);
        _tprintf(_T("DataAtom => Numeric [0x%llX](%lld)"), data, data);
    }
    else if (atom->IsBytes())
    {
        BYTE buffer[32] = {0};
        atom->GetBytes(buffer, 32);
        _tprintf(_T("DataAtom => Bytes "));
        for(int i=0; i<32; i++)
            _tprintf(_T("0x%02X "), buffer[i]);
        _tprintf(_T("\n"));

        _tprintf(_T("         => \"%S\" \n"), (char*)buffer);
    }
    else
    {
        _tprintf(_T("  **  OOPS! DataAtom parsing error!! "));
    }
}

void PrintOpalList(COpalList* data)
{
    _tprintf(_T(" [ \n"));
    list<COpalData*> list;
    data->GetRawList(list);
    for (COpalData* item : list)
    {
        if(IsOpalList((COpalList*)item))
            PrintOpalList((COpalList *)item);
        else if (IsOpalNamePair((COpalNamePair*)item))
            PrintOpalNamePair((COpalNamePair*)item);
        else if (IsOpalAtom((COpalDataAtom*)item))
            PrintOpalDataAtom((COpalDataAtom*)item);
    }
    _tprintf(_T(" ] \n"));
}
void PrintOpalNamePair(COpalNamePair* data)
{
    COpalDataAtom name;
    COpalData* value=nullptr;
    _tprintf(_T(" { \n"));
    data->Get(name, &value);

    PrintOpalDataAtom((COpalDataAtom*)&name);

    if (IsOpalList((COpalList*)value))
        PrintOpalList((COpalList*)value);
    else if (IsOpalNamePair((COpalNamePair*)value))
        PrintOpalNamePair((COpalNamePair*)value);
    else if (IsOpalAtom((COpalDataAtom*)value))
        PrintOpalDataAtom((COpalDataAtom*)value);

    if(nullptr != value)
        delete value;
    _tprintf(_T(" } \n"));
}

void PrintOpalProperties(BYTE* buffer, size_t max_len)
{
    COpalResponse resp;

    _tprintf(_T("[TPer to HOST properties]\n"));

    resp.FromOpalBuffer(buffer, max_len);

    COpalComPacket compkt;
    COpalPacket pkt;
    COpalSubPacket subpkt;
    resp.GetHeaders(compkt, pkt, subpkt);
    PrintOpalComPacket(compkt);
    PrintOpalPacket(pkt);
    PrintOpalSubPacket(subpkt);

    COpalCmdPayload payload;
    resp.GetPayload(payload);
    COpalList list;
    payload.GetArgList(list);
    PrintOpalList(&list);
}

int _tmain(int argc, TCHAR* argv[])
{
    list<COpalDevice*> opallist;
    EnumOpalDevices(opallist);

    for(COpalDevice* dev : opallist)
    {
        OPAL_DEVICE_INFO info;
        dev->GetDeviceInfo(info);
        PrintOpalDeviceInfo(dev->DevPath, info);

        if(dev->IsOpal2()|| dev->IsOpal1() || dev->IsEnterprise())
        {
            BYTE buffer[PAGE_SIZE] = {0};
            dev->QueryTPerProperties(buffer, PAGE_SIZE);
            PrintOpalProperties(buffer, PAGE_SIZE);
        }
    }

    getchar();
    return ERROR_SUCCESS;
}
