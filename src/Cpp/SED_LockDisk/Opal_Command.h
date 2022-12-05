#pragma once

//MethodStatus indicates end of CmdPayload. it has only 5 bytes: 0xF0 0x00 0x00 0x00 0xF1
//I treat it as a COpalList with three 1-byte COpalDataAtom. Because the COpalSubPacket::Length
//fields need this value so I put MethodStatusList in COpalCmdPayload.
//**MethodStatusList is named by official example....
//When response from device, this structure will carry return code from device.
class COpalMethodStatus : public COpalList
{
public:
    COpalMethodStatus();
    virtual ~COpalMethodStatus();
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);
    void Reset();

    inline OPAL_STATUS GetStatus(){return Status;}

protected:
    OPAL_STATUS Status = OPAL_STATUS::SUCCESS;
};

// CmdPayload indicates the data payload of SubPacket.
// In each communication, besides "EndSession" command, has same structure in payload:
// (listed by sequence in byte array)
//      Begin by "Call" token (0xF8)
//          ServiceProvider UID, which indicate "service you send request to / receive response from".
//          ServiceMethod(features) you want to execute.
//          Argument List of ServiceMethod.
//      End by EndOfData token (0xF9)
//      End of CmdPayload bytes (0xF0 0x00 0x00 0x00 0xF1)
class COpalCmdPayload : public COpalData {
public:
    COpalCmdPayload();
    COpalCmdPayload(BYTE* invoke_uid, BYTE* method_uid);
    virtual ~COpalCmdPayload();

    void PushOpalItem(COpalData* newarg);
    void PushOpalItem(COpalData& newarg);
    void GetArgList(COpalList& result);
    void GetArgList(list<COpalData*>& list);

    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();
    void Set(BYTE* invoke_uid, BYTE* method_uid);
    void Reset();
    void operator= (COpalCmdPayload& newdata);

private:
    const BYTE CallToken = (BYTE)OPAL_DATA_TOKEN::CALL;
    const BYTE EndCallToken = (BYTE)OPAL_DATA_TOKEN::ENDOFDATA;
    COpalDataAtom InvokingUID;
    COpalDataAtom Method;
    COpalList ArgList;

    //MethodStatusList indicates end of CmdPayload. it has only 5 bytes: 0xF0 0x00 0x00 0x00 0xF1
    //I treat it as a COpalList with three 1-byte COpalDataAtom. Because the COpalSubPacket::Length
    //fields need this value so I put MethodStatusList in COpalCmdPayload.
    //**MethodStatusList is named by official example....
    //COpalList MethodStatusList;
    COpalMethodStatus MethodStatus;
};

// Most commands to OPAL Device has same structure in sequence as following: 
// (End of Session command is special case. It is different in Payload.)
//      ComPacket
//      Packet
//      SubPacket
//      Payload of SubPacket (refer to COpalCmdPayload comments)
//      Paddings
//
// The "EndOfSession" command is special case because it doesn't have CmdPayload structure.
// It only have 1-byte in Payload : 0xFA .
// No call token, no endcall token, no uid and method, no "end of CmdPayload" bytes.
// There is only one byte after SubPacket header.
// After 0xFA, there are still paddings.
class COpalCommand {
public:
    //Payload will be a DataAtom. used for EndSession command
    COpalCommand();

    //normal operation, Payload will be COpalCmdPayload
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method, UINT32 host_sid, UINT32 tper_sid, USHORT comid);
    //comid is BaseComID comes from Discovery0 command
    //COpalCommand(USHORT comid);
    COpalCommand(UINT32 host_sid, UINT32 tper_sid, USHORT comid);
    
    ~COpalCommand();

    size_t BuildOpalBuffer(BYTE* buffer, size_t max_buf_size);

protected:
    COpalComPacket ComPacket;
    COpalPacket Packet;
    COpalSubPacket SubPacket;
    COpalCmdPayload Payload;

    UINT32 PaddingSize = 0;
    UINT32 CmdLength = 0;
    UINT16 BaseComID = 0;

    //A complete session id is built by TSN+HSN, TSN=>TperSID,  HSN=>HostSID
    UINT32 Host_Sid = 0;
    UINT32 Tper_Sid = 0;

    OPAL_UID_TAG Invoker = OPAL_UID_TAG::SMUID;
    OPAL_METHOD_TAG Method = OPAL_METHOD_TAG::STARTSESSION;
    virtual size_t BuildOpalBufferPayload(BYTE* buffer, size_t max_buf_size);
    virtual size_t BuildOpalBufferTail(BYTE* buffer, size_t max_buf_size);
    virtual UINT32 CalcPayloadLenInBytes();

    //sync Length fields of ComPacket, Packet, and SubPacket.
    void CompleteCmd();
};

class CCmdStartSession : public COpalCommand
{
public:
    CCmdStartSession(UINT32 host_sid, USHORT comid);
    //CCmdStartSession(UINT32 host_sid, UINT32 tper_sid, USHORT comid);
    virtual ~CCmdStartSession();
    void PrepareCmd(OPAL_UID_TAG target_sp, OPAL_UID_TAG sign_auth, BOOLEAN is_write, char *pwd);
};

class CCmdEndSession : public COpalCommand
{
public:
    CCmdEndSession(UINT32 host_sid, UINT32 tper_sid, USHORT comid);
    virtual ~CCmdEndSession();

protected:
    size_t BuildOpalBufferPayload(BYTE* buffer, size_t max_buf_size);
    size_t BuildOpalBufferTail(BYTE* buffer, size_t max_buf_size);
    UINT32 CalcPayloadLenInBytes();

    OPAL_DATA_TOKEN EOS = OPAL_DATA_TOKEN::ENDOFSESSION;
};

//Lock or Unlock global range...
class CLockGlobalRange : public COpalCommand
{
public:
    CLockGlobalRange(UINT32 host_sid, UINT32 tper_sid, USHORT comid);
    virtual ~CLockGlobalRange();
    void PrepareCmd(BOOLEAN read_lock, BOOLEAN write_lock);
};

class CCmdQueryProperties : public COpalCommand
{
public:
    CCmdQueryProperties(UINT32 host_sid, USHORT comid);
    virtual ~CCmdQueryProperties();
    void PrepareCmd(UINT16 max_compkt_size);
};

