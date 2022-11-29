#pragma once

class COpalDataBase
{
public:
    virtual ~COpalDataBase(){}
    //bytes endian of Opal Data Blob is BIG-ENDIAN
    //size_t GetOpalBytes(list<BYTE>& result);
    virtual size_t GetOpalBytes(BYTE* buffer, size_t max_len)=0;

    //bytes endian of Opal Data Blob is BIG-ENDIAN
    //void PutOpalBytes(list<BYTE>& input);
    virtual void PutOpalBytes(BYTE* buffer, size_t max_len) = 0;
    //get total length if output OpalData in BigEndian
    virtual size_t GetOpalDataLen() = 0;

    virtual void Reset() = 0;
};

#pragma pack(push)
#pragma pack(1)
class COpalComPacket{// : public COpalDataBase{
public:
    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    //void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
    void Reset();

    UINT32 Reserved = 0;
    UINT8 ExtComID[4] = { 0x07,0xFE, 0, 0 };//0x07FE0000 in big endian
    UINT32 OutstandingData = 0;   //big endian
    UINT32 MinTx = 0;             //big endian
    UINT32 Length = 0;            //(big endian) ==> sizeof(OPAL_PACKET) + OPAL_PACKET::Length

};

class COpalPacket{// : public COpalDataBase {
public:
    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    //void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
    void Reset();

    union
    {
        struct {
            UINT32 TSN;         //session SN of TPer
            UINT32 HSN;         //session SN of Host
        }DUMMYSTRUCTNAME;
        UINT64 SessionID = 0;   //unique session id is built by TSN+HSN.
    };
    UINT32 SeqNo = 0;
    UINT16 Reserved = 0;
    UINT16 AckType = 0;
    UINT32 Ack = 0;
    UINT32 Length = 0;          //==sizeof(OPAL_DATA_SUB_PACKET) + OPAL_DATA_SUB_PACKET::Length + padding length
};

class COpalSubPacket{// : public COpalDataBase {
public:
    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    //void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
    void Reset();

    UINT8 Reserved[6] = { 0 };
    UINT16 Kind = 0;        //??
    UINT32 Length = 0;      //total length of following DataPayload block, NOT INCLUDING padding...
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

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
    void Reset();

    void operator= (COpalDataAtom& newdata);
    template<typename T> void PutUint(T data)
    {
        Reset();
        //GetOpalBytes() will swap Endian again.
        //Still store int values in LittleEndian.
        RtlCopyMemory(this->Data, &data, sizeof(T));
        Type = GetAtomUintToken(sizeof(T));
    }

private:
    OPAL_ATOM_TOKEN Type = OPAL_ATOM_TOKEN::NO_TOKEN;
    BYTE Data[32] = {0};        //hardcode.... MAX 32bytes data.

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

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();

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
    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
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

    void PushCmdArg(COpalDataBase *newarg);
    void PushCmdArg(COpalDataBase &newarg);

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
    void Set(BYTE *invoke_uid, BYTE *method_uid);
    void Reset();

private:
    const BYTE CallToken = (BYTE)CALL;
    const BYTE EndCallToken = (BYTE)ENDOFDATA;
    COpalDataAtom InvokingUID;
    COpalDataAtom Method;
    COpalList ArgList;
    COpalList MethodStatusList; //indicates end of CmdPayload
};

class COpalCommand {
public:
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method);
    ~COpalCommand();

    void PushCmdArg(COpalDataBase &item);
    void PushCmdArg(COpalDataBase *item);
    size_t BuildCmdBuffer(BYTE* buffer, size_t max_buf_size);
    void CompleteCmd();     //add empty list, add padding, calculate Length field in packets...etc.

    void UpdateBaseComID(USHORT comid);
protected:
    //sync Length fields of ComPacket, Packet, and SubPacket.
    void UpdatePacketLength();

protected:
    COpalComPacket ComPacket;
    COpalPacket Packet;
    COpalSubPacket SubPacket;
    COpalCmdPayload Payload;
    
    UINT32 PaddingSize = 0;

    size_t CmdLength = 0;
};

