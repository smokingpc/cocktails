#include "Common.h"

#pragma region ======== COpalMethodStatus ========
COpalMethodStatus::COpalMethodStatus()
{
    Reset();
}
COpalMethodStatus::~COpalMethodStatus(){}
size_t COpalMethodStatus::FromOpalBytes(BYTE* buffer, size_t max_len)
{
    COpalList::FromOpalBytes(buffer, max_len);

    //the content of COpalList is put by sequence...
    UINT8 data = 0;
    ((COpalDataAtom *)this->List.front())->GetUint(data);
    Status = (OPAL_STATUS)data;
}
void COpalMethodStatus::Reset()
{
    COpalList::Reset();
    COpalDataAtom atom;
    atom.PutUint((UINT8)0);
    this->PushOpalItem(atom);
    this->PushOpalItem(atom);
    this->PushOpalItem(atom);
}
#pragma endregion

#pragma region ======== COpalCmdPayload ========
COpalCmdPayload::COpalCmdPayload()
{
    //at the end of each COpalCmdPayload, should have a empty OPAL_DATA_LIST with three bytes "0x00".
    // (total 5 bytes, 1byte OPAL_DATA_TOKEN::STARTLIST + 3bytes 0x00 + 1byte OPAL_DATA_TOKEN::ENDLIST
    //it represents the end of entire data payload.
    COpalDataAtom atom((UINT8)0);
    MethodStatusList.PushOpalItem(atom);
    MethodStatusList.PushOpalItem(atom);
    MethodStatusList.PushOpalItem(atom);
}
COpalCmdPayload::COpalCmdPayload(BYTE* invoke_uid, BYTE* method_uid) : COpalCmdPayload()
{
    Set(invoke_uid, method_uid);
}
COpalCmdPayload::~COpalCmdPayload()
{
    Reset();
}
void COpalCmdPayload::PushOpalItem(COpalData* newarg)
{
    ArgList.PushOpalItem(newarg);
}
void COpalCmdPayload::PushOpalItem(COpalData& newarg)
{
    //PushOpalItem(newarg);
    ArgList.PushOpalItem(newarg);
}
void COpalCmdPayload::GetArgList(list<COpalData*>& list)
{
    ArgList.GetRawList(list);
}
void COpalCmdPayload::GetArgList(COpalList& result)
{
    result = this->ArgList;
}
size_t COpalCmdPayload::ToOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < OpalDataLen())
        return 0;

    size = sizeof(BYTE);
    RtlCopyMemory(cursor, &CallToken, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = InvokingUID.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = Method.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = ArgList.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(BYTE);
    RtlCopyMemory(cursor, &EndCallToken, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = MethodStatusList.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
size_t COpalCmdPayload::FromOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t total_used = 0;
    size_t remain_size = max_len;
    size_t used = 0;
    //command payload structure begin by OPAL_DATA_TOKEN::CALL
    if (cursor[0] != (BYTE)OPAL_DATA_TOKEN::CALL)
        return 0;

    Reset();

    used = 1;   //consume OPAL_DATA_TOKEN::CALL token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);
    //first element in cmd payload is InvokingUID.
    //it is COpalDataAtom object
    used = InvokingUID.FromOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    //second element in cmd playload is Method.
    //it is COpalDataAtom object
    used = Method.FromOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    used = ArgList.FromOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    assert(cursor[0] == OPAL_DATA_TOKEN::ENDOFDATA);
    used = 1;   //consume OPAL_DATA_TOKEN::ENDOFDATA token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    //skip MethodStatusList. It is useless. Only used to identify "CmdPayload ends"
    //I am too lazy to write parsing function so hardcode them here..... :p
    assert(cursor[0] == OPAL_DATA_TOKEN::STARTLIST);
    assert(cursor[1] == 0x00);
    assert(cursor[2] == 0x00);
    assert(cursor[3] == 0x00);
    assert(cursor[4] == OPAL_DATA_TOKEN::ENDLIST);

    return total_used;
}
size_t COpalCmdPayload::OpalDataLen()
{
    return sizeof(CallToken) + InvokingUID.OpalDataLen() + Method.OpalDataLen() +
        ArgList.OpalDataLen() + sizeof(EndCallToken) + MethodStatusList.OpalDataLen();
}
void COpalCmdPayload::Reset()
{
    InvokingUID.Reset();
    Method.Reset();
    ArgList.Reset();
}
void COpalCmdPayload::operator= (COpalCmdPayload& newdata)
{
    this->InvokingUID = newdata.InvokingUID;
    this->Method = newdata.Method;
    this->ArgList = newdata.ArgList;
}
void COpalCmdPayload::Set(BYTE* invoke_uid, BYTE* method_uid)
{
    Reset();
    InvokingUID.PutUID(invoke_uid);
    Method.PutUID(method_uid);
}
#pragma endregion

#pragma region ======== COpalCommand ========
COpalCommand::COpalCommand()
{
    //Payload = new COpalDataAtom();
    //((COpalDataAtom*)Payload)->PutToken(OPAL_DATA_TOKEN::ENDOFSESSION);
}
//COpalCommand::COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method)
//{
//    //Payload = new COpalCmdPayload(INVOKE_UIDs[invoking].data(), METHOD_UIDs[method].data());
//}
COpalCommand::COpalCommand(USHORT comid) : COpalCommand()
{
    BaseComID = comid;
}
COpalCommand::~COpalCommand()
{
    Payload.Reset();
    //if (nullptr != Payload)
    //{
    //    delete Payload;
    //    Payload = nullptr;
    //}
}
//void COpalCommand::PushCmdArg(COpalData &item)
//{
//    if (IsCmdPayload(Payload))
//        ((COpalCmdPayload*)Payload)->PushOpalItem(item);
//}
//void COpalCommand::SetBaseComID(USHORT comid)
//{
//    SwapEndian(&comid, (UINT16*)ComPacket.ExtComID);
//}
//USHORT COpalCommand::GetBaseComID()
//{
//    UINT16 comid = 0;
//    SwapEndian((UINT16*)ComPacket.ExtComID, &comid);
//    return comid;
//}
//void COpalCommand::SetSessionID(UINT32 host_sid, UINT32 tper_sid)
//{
//    Packet.HSN = host_sid;
//    Packet.TSN = tper_sid;
//}
//void COpalCommand::PushCmdArg(COpalData *item)
//{
//    if (IsCmdPayload(item))
//        ((COpalCmdPayload*)Payload)->PushOpalItem(item);
//}
size_t COpalCommand::BuildOpalBuffer(BYTE* buffer, size_t max_buf_size)
{
    size_t remain_size = max_buf_size;
    size_t data_size = 0;
    BYTE* cursor = buffer;

    CompleteCmd();
    if (CmdLength > max_buf_size)
        return 0;
    
    data_size = ComPacket.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = Packet.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = SubPacket.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = BuildOpalBufferPayload(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    //put padding bytes
    size_t pad_len = PaddingSize;
    for (UINT32 i = 0; i < pad_len; i++)
        cursor[i] = 0x00;
    UpdateSizeAndCursor(cursor, pad_len, remain_size);

    size_t used_size = max_buf_size - remain_size;
    return used_size;
}
void COpalCommand::CompleteCmd()
{
    UINT32 size = 0;

    //child object should calculate SubPacket.Length
    SubPacket.Length = CalcPayloadLenInBytes();
    size = (UINT32)sizeof(COpalSubPacket) + SubPacket.Length;
    Packet.Length = (UINT32)ROUND_UP_ALIGN_2N(size, sizeof(UINT32));
    PaddingSize = Packet.Length - size;
    size = sizeof(COpalPacket);
    ComPacket.Length = (UINT32)sizeof(COpalPacket) + Packet.Length;
    CmdLength = ComPacket.Length + sizeof(COpalComPacket);
}

size_t COpalCommand::BuildOpalBufferPayload(BYTE* buffer, size_t max_buf_size)
{
    size_t remain_size = max_buf_size;
    size_t data_size = 0;
    BYTE* cursor = buffer;

    data_size = Payload.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    size_t used_size = max_buf_size - remain_size;
    return used_size;
}

UINT32 COpalCommand::CalcPayloadLenInBytes()
{
    return (UINT32)Payload.OpalDataLen();
}
#pragma endregion

#pragma region ======== CCmdEndSession ========
CCmdStartSession::CCmdStartSession(UINT32 host_sid, USHORT comid)
{
    PrepareCmdArg(host_sid, comid);
}
CCmdStartSession::~CCmdStartSession()
{}
//size_t CCmdStartSession::BuildOpalBuffer(BYTE* buffer, size_t max_buf_size)
//{}

void CCmdStartSession::PrepareCmdArg(UINT32 host_sid, USHORT comid)
{
    //OPAL_UID_TAG::SMUID, OPAL_METHOD_TAG::STARTSESSION
    Payload.Set(GetOpalUID(OPAL_UID_TAG::SMUID), GetOpalUID())
}
#pragma endregion


#pragma region ======== CCmdEndSession ========
CCmdEndSession::CCmdEndSession(UINT32 host_sid, UINT32 tper_sid, USHORT comid)
{}
CCmdEndSession::~CCmdEndSession()
{}
size_t CCmdEndSession::BuildOpalBuffer(BYTE* buffer, size_t max_buf_size)
{}
#pragma endregion
