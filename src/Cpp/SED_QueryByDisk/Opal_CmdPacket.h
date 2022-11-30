#pragma once

class COpalDataBase
{
public:
    virtual ~COpalDataBase(){}
    
    //bytes endian of Opal Data Blob is BIG-ENDIAN
    //ToOpalBytes() returns "how many bytes copied to buffer?"
    //caller can use it to determine buffer new position ptr...
    virtual size_t ToOpalBytes(BYTE* buffer, size_t max_len)=0;
    
    //FromOpalBytes() returns "how many bytes consumed in parsing?
    //caller can use it to determine buffer new position ptr...
    virtual size_t FromOpalBytes(BYTE* buffer, size_t max_len) = 0;

    //bytes endian of Opal Data Blob is BIG-ENDIAN
    //void PutOpalBytes(list<BYTE>& input);
    //virtual void PutOpalBytes(BYTE* buffer, size_t max_len) = 0;
    //get total length if output OpalData in BigEndian
    virtual size_t OpalDataLen() = 0;

    virtual void Reset() = 0;
};

#pragma pack(push)
#pragma pack(1)
class COpalComPacket{
public:
    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t OpalDataLen();
    void Reset();

    UINT32 Reserved = 0;
    UINT8 ExtComID[4] = { 0 };      //big endian
    UINT32 OutstandingData = 0;     //big endian
    UINT32 MinTx = 0;               //big endian
    UINT32 Length = 0;              //(big endian) ==> sizeof(OPAL_PACKET) + OPAL_PACKET::Length

};

class COpalPacket{
public:
    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t OpalDataLen();
    void Reset();

    union
    {
        struct {
        //the TSN and HSN comes from session open.
        //All commands during this session shoud use them to communicate with device.
            UINT32 TSN;         //session SN of TPer
            UINT32 HSN;         //session SN of Host
        }DUMMYSTRUCTNAME;
        UINT64 SessionID = 0;   //unique session id is built by TSN+HSN.
    };
    UINT32 SeqNo = 0;
    UINT16 Reserved = 0;
    UINT16 AckType = 0;
    UINT32 Ack = 0;
    UINT32 Length = 0;
};

class COpalSubPacket{
public:
    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t OpalDataLen();
    void Reset();

    UINT8 Reserved[6] = { 0 };
    UINT16 Kind = 0;        //??
    UINT32 Length = 0;      //total length of following DataPayload block, NOT INCLUDE padding...
};
#pragma pack(pop)


class COpalDataAtom : public COpalDataBase {
public:
    COpalDataAtom();
    COpalDataAtom(COpalDataAtom * newdata);
    COpalDataAtom(COpalDataAtom& newdata);
    COpalDataAtom(char* str, int strlen);
    COpalDataAtom(BYTE* data, int data_len);
    COpalDataAtom(BYTE* data);
    template<typename T> COpalDataAtom(T data);
    virtual ~COpalDataAtom();

    void PutString(char* str, size_t strlen);
    void PutBytes(BYTE* data, size_t data_len);
    void PutUID(BYTE* data);
    
    void GetBytes(BYTE* buffer, size_t max_len);
    //template<typename T> void GetUint(T &data);

    inline bool IsBytes(){ return (Type > OPAL_ATOM_TOKEN::SHORT_BYTES);}
    inline bool IsNumeric() { return ((Type > OPAL_ATOM_TOKEN::SHORT_UINT) && !IsBytes()); }
    inline bool IsTiny() { return ((Type == OPAL_ATOM_TOKEN::SHORT_UINT1) || (Type == OPAL_ATOM_TOKEN::SHORT_BYTES_1)); }

    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();
    void Reset();

    void operator= (COpalDataAtom& newdata);
    template<typename T> void GetUint(T& data)
    {
        if (!IsNumeric() && !IsTiny())
            return;

        switch (Type)
        {
        case OPAL_ATOM_TOKEN::SHORT_UINT2:
        {
            UINT16 temp = 0;
            SwapEndian((UINT16*)Data, (UINT16*)&temp);
            data = (T)temp;
            break;
        }
        case OPAL_ATOM_TOKEN::SHORT_UINT4:
        {
            UINT32 temp = 0;
            SwapEndian((UINT32*)Data, (UINT32*)&temp);
            data = (T)temp;
            break;
        }
        case OPAL_ATOM_TOKEN::SHORT_UINT8:
        {
            UINT64 temp = 0;
            SwapEndian((UINT64*)Data, (UINT64*)&temp);
            data = (T)temp;
            break;
        }
        case OPAL_ATOM_TOKEN::SHORT_UINT1:
        case OPAL_ATOM_TOKEN::SHORT_BYTES_1:
        default:
            data = (T)Data[0];
            break;
        }
    }

    template<typename T> void PutUint(T data)
    {
    //filter out the types which IS NOT numberic type...
        if(std::is_same<T, BYTE*>::value)
            return;
        if (std::is_same<T, char*>::value)
            return;

        Reset();
        //ToOpalBytes() will swap Endian again.
        //Still store int values in LittleEndian.
        RtlCopyMemory(this->Data, &data, sizeof(T));
        Type = GetAtomUintToken(sizeof(T));
    }

private:
    OPAL_ATOM_TOKEN Type = OPAL_ATOM_TOKEN::NO_TOKEN;   //data stored here is LittleEndian.
    BYTE Data[32] = {0};        //hardcode.... MAX 32bytes data. data stored here is LittleEndian.

    //determine OPAL_ATOM_TOKEN::xxx_BYTES_xx by bytes array length
    inline OPAL_ATOM_TOKEN GetAtomBytesToken(size_t size)
    {
        OPAL_ATOM_TOKEN base = NO_TOKEN;
        if (size >= 16)
            base = MID_BYTES;
        else if (size > 0)
            base = SHORT_BYTES;

        //check the definition of OPAL_ATOM_TOKEN.
        //each token are defined as "BASE + length"
        return (OPAL_ATOM_TOKEN)(base + size);
    }

    //determine OPAL_ATOM_TOKEN::xxx_BYTES_xx by sizeof(UINT data type)
    inline OPAL_ATOM_TOKEN GetAtomUintToken(size_t size)
    {
        return (OPAL_ATOM_TOKEN)(SHORT_UINT + size);
    }

};//OPAL_DATA_ATOM;
class COpalNamePair : public COpalDataBase {
public:
    COpalNamePair();
    COpalNamePair(COpalNamePair &newdata);
    COpalNamePair(COpalNamePair *newdata);
    COpalNamePair(COpalDataAtom& name);
    COpalNamePair(COpalDataAtom& name, COpalDataBase *value);
    virtual ~COpalNamePair();
    void operator= (COpalNamePair& newdata);
    void Reset();
    void Set(COpalDataAtom& name, COpalDataBase* value);
    void Get(COpalDataAtom& name, COpalDataBase** value);

    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();

private:
    const OPAL_DATA_TOKEN Start = OPAL_DATA_TOKEN::STARTNAME;
    const OPAL_DATA_TOKEN End = OPAL_DATA_TOKEN::ENDNAME;
    COpalDataAtom Name;
    COpalDataBase *Value = NULL;
    void CopyValue(COpalDataBase* newdata);
};
class COpalList : public COpalDataBase {
public:
    COpalList();
    COpalList(COpalList &newlist);
    COpalList(COpalList *newlist);
    virtual ~COpalList();
    void Reset();
    void PushOpalItem(COpalDataBase *item);
    void PushOpalItem(COpalDataBase &item);
    void GetRawList(list<COpalDataBase*>& list);
    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();
    void operator= (COpalList& newlist);

private:
    const OPAL_DATA_TOKEN Start = OPAL_DATA_TOKEN::STARTLIST;
    const OPAL_DATA_TOKEN End = OPAL_DATA_TOKEN::ENDLIST;
    list<COpalDataBase*> List;

    void CopyList(list<COpalDataBase*> &newlist);
};

class COpalCmdPayload : public COpalDataBase {
public:
    COpalCmdPayload();
    COpalCmdPayload(BYTE* invoke_uid, BYTE* method_uid);
    virtual ~COpalCmdPayload();

    void PushOpalItem(COpalDataBase *newarg);
    void PushOpalItem(COpalDataBase &newarg);
    void GetArgList(COpalList& result);
    void GetArgList(list<COpalDataBase*>& list);

    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();
    void Set(BYTE *invoke_uid, BYTE *method_uid);
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
    //(in view of data stream, it seems better to put MethodStatusList in COpalCommand ....)
    COpalList MethodStatusList; 
};

class COpalCommand {
public:
    //Payload will be a DataAtom. used for EndSession command
    COpalCommand();

    //normal operation, Payload will be COpalCmdPayload
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method);
    //normal operation, Payload will be COpalCmdPayload
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method, USHORT comid);
    ~COpalCommand();

    void PushCmdArg(COpalDataBase &item);
    void PushCmdArg(COpalDataBase *item);
    size_t BuildOpalBuffer(BYTE* buffer, size_t max_buf_size);

    void SetBaseComID(USHORT comid);
    USHORT GetBaseComID();

private:
    //sync Length fields of ComPacket, Packet, and SubPacket.
    void UpdatePacketLength();

    COpalComPacket ComPacket;
    COpalPacket Packet;
    COpalSubPacket SubPacket;
    //COpalCmdPayload Payload;
    COpalDataBase *Payload = nullptr;

    UINT32 PaddingSize = 0;
    UINT32 CmdLength = 0;
};

class COpalResponse {
public:
    COpalResponse();
    COpalResponse(BYTE* buffer, size_t max_len);
    ~COpalResponse();

    //FromOpalBuffer() returns "how many bytes consumed in parsing?
    size_t FromOpalBuffer(BYTE* buffer, size_t max_len);
    void GetHeaders(COpalComPacket *compkt, COpalPacket *pkt, COpalSubPacket *subpkt);
    void GetHeaders(COpalComPacket &compkt, COpalPacket &pkt, COpalSubPacket &subpkt);
    void GetPayload(COpalCmdPayload &result);
    void GetPayload(list<COpalDataBase*> &result);
    
    //Response of EndSession is different from other responses.
    //It only has 1 char (0xFA) following after Data SubPacket.
    //After 0xFA, there are only padding bytes.
    //No Invoking and Method, no CALL token, No List....
    inline bool IsEndSession()
    {
        return (OPAL_DATA_TOKEN::ENDOFSESSION == PayloadBegin[0]);
    }

private:
    COpalComPacket *ComPacket = nullptr;
    COpalPacket *Packet = nullptr;
    COpalSubPacket *SubPacket = nullptr;
    //COpalDataBase* Payload = nullptr;

    BYTE* RespBuf = nullptr;
    size_t RespBufSize = 0;
    BYTE* PayloadBegin = nullptr;
    size_t PayloadMaxSize = 0;
};
