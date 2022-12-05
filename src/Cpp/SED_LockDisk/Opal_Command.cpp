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
    //lazy way hardcode....
    Status = (OPAL_STATUS)buffer[1];
    return 5;
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
    //COpalDataAtom atom((UINT8)0);
    //MethodStatusList.PushOpalItem(atom);
    //MethodStatusList.PushOpalItem(atom);
    //MethodStatusList.PushOpalItem(atom);
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

    size = MethodStatus.ToOpalBytes(cursor, remain_size);
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
    MethodStatus.FromOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);
    //assert(cursor[1] == 0x00);
    //assert(cursor[2] == 0x00);
    //assert(cursor[3] == 0x00);
    //assert(cursor[4] == OPAL_DATA_TOKEN::ENDLIST);

    return total_used;
}
size_t COpalCmdPayload::OpalDataLen()
{
    return sizeof(CallToken) + InvokingUID.OpalDataLen() + Method.OpalDataLen() +
        ArgList.OpalDataLen() + sizeof(EndCallToken) + MethodStatus.OpalDataLen();
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
{}
COpalCommand::COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method, 
                            UINT32 host_sid, UINT32 tper_sid, USHORT comid)
    :COpalCommand(host_sid, tper_sid, comid)
{
    Invoker = invoking;
    Method = method;
}
COpalCommand::COpalCommand(UINT32 host_sid, UINT32 tper_sid, USHORT comid) : COpalCommand()
{
    BaseComID = comid;
    Host_Sid = host_sid;
    Tper_Sid = tper_sid;
}
COpalCommand::~COpalCommand()
{
    Payload.Reset();
}
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
    
    data_size = BuildOpalBufferTail(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    size_t used_size = max_buf_size - remain_size;
    return used_size;
}
void COpalCommand::CompleteCmd()
{
    UINT32 size = 0;

    Packet.HSN = Host_Sid;
    Packet.TSN = Tper_Sid;
    SwapEndian(&BaseComID, (UINT16*)ComPacket.ExtComID);

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
size_t COpalCommand::BuildOpalBufferTail(BYTE* buffer, size_t max_buf_size)
{
    //put padding bytes
    size_t pad_len = PaddingSize;
    for (UINT32 i = 0; i < pad_len; i++)
        buffer[i] = 0x00;

    return pad_len;
}
UINT32 COpalCommand::CalcPayloadLenInBytes()
{
    return (UINT32)Payload.OpalDataLen();
}
#pragma endregion

#pragma region ======== CCmdStartSession ========
CCmdStartSession::CCmdStartSession(UINT32 host_sid, USHORT comid)
    : COpalCommand(OPAL_UID_TAG::SMUID, OPAL_METHOD_TAG::STARTSESSION, host_sid, 0, comid)
{
}
CCmdStartSession::~CCmdStartSession()
{}
void CCmdStartSession::PrepareCmd(OPAL_UID_TAG target_sp, OPAL_UID_TAG sign_auth, BOOLEAN is_write, char* pwd)
{
    Payload.Set(GetSpUID(Invoker), GetMethodUID(Method));
    
    COpalNamePair* pair = nullptr;
    COpalDataAtom* name = nullptr;
    COpalDataAtom *value = new COpalDataAtom(Host_Sid);
    Payload.PushOpalItem(value);
    //set Target Service Provider for following commands
    value = new COpalDataAtom(GetSpUID(target_sp));
    Payload.PushOpalItem(value);

    //Is this session a Write Session?
    value = new COpalDataAtom((is_write? OPAL_TRUE : OPAL_FALSE));
    Payload.PushOpalItem(value);

    if (NULL != pwd && strlen(pwd) > 0)
    {
        COpalPwdHash* hasher = new COpalPwdHash_Win32();
        char salt[20] = { 0 };
        char hashed_pwd[32] = { 0 };
        //note: refer to SEDUtils-CLI, it uses empty char buffer[20] as salt for NVMe devices.
        //      So I didn't put salt in it. But for SATA device, it use SerialNo of device as salt.
        hasher->HashOpalPwd((BYTE*)pwd, (UINT32)strlen(pwd), 75000, (BYTE*)salt, 20, (BYTE*)hashed_pwd, 32);
        name = new COpalDataAtom(HOST_CHALLENGE);
        value = new COpalDataAtom((BYTE*)hashed_pwd, 32);
        
        pair = new COpalNamePair(name, value);
        Payload.PushOpalItem(pair);

        delete hasher;
    }
    
    //specify "whick kind of password I used here..."
    name = new COpalDataAtom(HOST_SIGN_AUTH);
    value = new COpalDataAtom(GetSpUID(sign_auth));
    pair = new COpalNamePair(name, value);
    Payload.PushOpalItem(pair);
}
#pragma endregion

#pragma region ======== CCmdEndSession ========
CCmdEndSession::CCmdEndSession(UINT32 tper_sid, UINT32 host_sid, USHORT comid)
    : COpalCommand(host_sid, tper_sid, comid)
{}
CCmdEndSession::~CCmdEndSession()
{}

size_t CCmdEndSession::BuildOpalBufferPayload(BYTE* buffer, size_t max_buf_size)
{
    //End of Session only have 1 byte in Payload:  OPAL_DATA_TOKEN::ENDOFSESSION
    //There is no invoker and method field in EndOfSession packet.
    buffer[0] = (UINT8)OPAL_DATA_TOKEN::ENDOFSESSION;
    return 1;
}
size_t CCmdEndSession::BuildOpalBufferTail(BYTE* buffer, size_t max_buf_size)
{
    //no MethodStatus field in EndOfSession command.
    //so do nothing here...

    return 0;
}
UINT32 CCmdEndSession::CalcPayloadLenInBytes()
{
//End of Session only have 1 byte in Payload:  OPAL_DATA_TOKEN::ENDOFSESSION
    return 1;
}
#pragma endregion

#pragma region ======== CLockGlobalRange ========
CLockGlobalRange::CLockGlobalRange(UINT32 host_sid, UINT32 tper_sid, USHORT comid)
    : COpalCommand(OPAL_UID_TAG::LOCKRANGE_GLOBAL, OPAL_METHOD_TAG::SET, host_sid, tper_sid, comid)
{}
CLockGlobalRange::~CLockGlobalRange()
{}
void CLockGlobalRange::PrepareCmd(BOOLEAN rlock, BOOLEAN wlock)
{
    //OPAL_UID_TAG target_sp = OPAL_UID_TAG::LOCKINGSP;
    COpalNamePair* pair = nullptr;
    COpalDataAtom* name = nullptr;
    COpalDataAtom* value = nullptr;
    COpalList *list = nullptr;

    Payload.Set(GetSpUID(Invoker), GetMethodUID(Method));

    //value = new COpalDataAtom(GetSpUID(target_sp));
    //Payload.PushOpalItem(value);
    //cmd arg == [ <%Range UID%>, {VALUES : [{READLOCKED: 1 or 0}, {WRITELOCKED : 1 or 0}]} ]
    list = new COpalList();
    {
        name = new COpalDataAtom(OPAL_DATA_TOKEN::READLOCKED);
        value = new COpalDataAtom((OPAL_DATA_TOKEN)rlock);
        Payload.PushOpalItem(new COpalNamePair(name, value));

        name = new COpalDataAtom(OPAL_DATA_TOKEN::WRITELOCKED);
        value = new COpalDataAtom((OPAL_DATA_TOKEN)wlock);
        Payload.PushOpalItem(new COpalNamePair(name, value));
    }
    name = new COpalDataAtom(OPAL_DATA_TOKEN::VALUES);
    pair = new COpalNamePair(name, list);
    Payload.PushOpalItem(pair);
}
#pragma endregion


#pragma region ======== CCmdQueryProperties ========
CCmdQueryProperties::CCmdQueryProperties(UINT32 host_sid, USHORT comid)
    :COpalCommand(SMUID, PROPERTIES, host_sid, 0, comid)
{}
CCmdQueryProperties::~CCmdQueryProperties()
{}
void CCmdQueryProperties::PrepareCmd(UINT16 max_compkt_size)
{
    COpalDataAtom *name = nullptr;
    COpalDataAtom *value = nullptr;
    COpalList *list = nullptr;
    COpalNamePair *pair = nullptr;
    UINT16 data_size = max_compkt_size;

    list = new COpalList();
    name = new COpalDataAtom((char*)"MaxComPacketSize", (int)strlen("MaxComPacketSize"));
    value = new COpalDataAtom(max_compkt_size);
    list->PushOpalItem(new COpalNamePair(name, value));

    name = new COpalDataAtom((char*)"MaxResponseComPacketSize", (int)strlen("MaxResponseComPacketSize"));
    value = new COpalDataAtom(data_size);
    list->PushOpalItem(new COpalNamePair(name, value));

    data_size = data_size - sizeof(COpalComPacket);
    name = new COpalDataAtom((char*)"MaxPacketSize", (int)strlen("MaxPacketSize"));
    value = new COpalDataAtom(data_size);
    list->PushOpalItem(new COpalNamePair(name, value));

    data_size = data_size - sizeof(COpalPacket) - sizeof(COpalSubPacket);
    name = new COpalDataAtom((char*)"MaxIndTokenSize", (int)strlen("MaxIndTokenSize"));
    value = new COpalDataAtom(data_size);
    list->PushOpalItem(new COpalNamePair(name, value));

    name = new COpalDataAtom((char*)"MaxPackets", (int)strlen("MaxPackets"));
    value = new COpalDataAtom((UINT8) 1);
    list->PushOpalItem(new COpalNamePair(name, value));

    name = new COpalDataAtom((char*)"MaxSubPackets", (int)strlen("MaxSubPackets"));
    value = new COpalDataAtom((UINT8)1);
    list->PushOpalItem(new COpalNamePair(name, value));

    name = new COpalDataAtom((char*)"MaxMethods", (int)strlen("MaxMethods"));
    value = new COpalDataAtom((UINT8)1);
    list->PushOpalItem(new COpalNamePair(name, value));

    name = new COpalDataAtom(HOSTPROPERTIES);
    pair = new COpalNamePair(name, list);

    Payload.PushOpalItem(pair);
}
#pragma endregion

