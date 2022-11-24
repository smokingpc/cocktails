#pragma once

interface IOPAL_DATA_INOUT {
    //bytes endian of Opal Data Blob is BIG-ENDIAN
    //size_t GetOpalBytes(list<BYTE>& result);
    size_t GetOpalBytes(BYTE *buffer, size_t max_len);

    //bytes endian of Opal Data Blob is BIG-ENDIAN
    //void PutOpalBytes(list<BYTE>& input);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();     //get total length if output OpalData in BigEndian
};

interface IOPAL_DATA_ATOM {
    void PutString(char* str, int strlen);
    void PutBytes(BYTE* blob, int bloblen);
    void PutUID(BYTE* uid);
    //void PutUint8(UINT8 data);
    //void PutUint16(UINT16 data);
    //void PutUint32(UINT32 data);
    //void PutUint64(UINT64 data);

    template<typename T> void PutUint(T data);
};

#pragma pack(push)
#pragma pack(1)
typedef struct _OPAL_COMPACKET : public IOPAL_DATA_INOUT {
    UINT32 Reserved = 0;
    UINT8 ExtComID[4] = { 0x07,0xFE, 0, 0 };//0x07FE0000 in big endian
    UINT32 OutstandingData = 0;   //big endian
    UINT32 MinTx = 0;             //big endian
    UINT32 Length;                //(big endian) ==> sizeof(OPAL_PACKET) + OPAL_PACKET::Length

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
}OPAL_COMPACKET;

typedef struct _OPAL_PACKET : public IOPAL_DATA_INOUT {
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

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
}OPAL_PACKET;

typedef struct _OPAL_DATA_SUB_PACKET : public IOPAL_DATA_INOUT {
    UINT8 Reserved[6] = { 0 };
    UINT16 Kind = 0;        //??
    UINT32 Length = 0;      //total length of following DataPayload block, NOT INCLUDING padding...

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen() { return sizeof(_OPAL_DATA_SUB_PACKET); }
}OPAL_DATA_SUB_PACKET;
#pragma pack(pop)

typedef struct _OPAL_UID {
    BYTE UID[8] = { 0 };

    _OPAL_UID(_OPAL_UID& newdata) { RtlCopyMemory(UID, newdata.UID, sizeof(UID)); }
    void operator= (_OPAL_UID &data) { RtlCopyMemory(UID, data.UID, sizeof(UID)); }
}OPAL_UID, * POPAL_UID;

typedef struct _OPAL_DATA_ATOM : public IOPAL_DATA_ATOM, public IOPAL_DATA_INOUT {
    OPAL_ATOM_TOKEN Type;
    BYTE Data[32] = {0};        //hardcode.... MAX 32bytes data.

    _OPAL_DATA_ATOM();
    _OPAL_DATA_ATOM(char* str, int strlen);
    _OPAL_DATA_ATOM(BYTE* data, int data_len);
    _OPAL_DATA_ATOM(BYTE* data);
    template<typename T>
    _OPAL_DATA_ATOM(T data);

    void PutString(char* str, int strlen);
    void PutBytes(BYTE* data, int data_len);
    void PutUID(BYTE* data);
    template<typename T>
        void PutUint(T data);

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();

    void operator= (_OPAL_DATA_ATOM& newdata);
}OPAL_DATA_ATOM;

typedef struct _OPAL_DATA{
    OPAL_DATA_TOKEN Start;
    OPAL_DATA_TOKEN End;
} OPAL_DATA;

typedef struct _OPAL_DATA_PAIR : public OPAL_DATA, public IOPAL_DATA_INOUT {
    OPAL_DATA_ATOM Name;
    //OPAL_DATA_ATOM Value;
    IOPAL_DATA_INOUT Value;
    _OPAL_DATA_PAIR();
    _OPAL_DATA_PAIR(OPAL_DATA_ATOM name, IOPAL_DATA_INOUT value);

    void operator= (_OPAL_DATA_PAIR &newdata);

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
} OPAL_DATA_PAIR;

typedef struct _OPAL_DATA_LIST : public OPAL_DATA, IOPAL_DATA_INOUT {
    list<IOPAL_DATA_INOUT*> List;

    _OPAL_DATA_LIST()
    {
        Start = STARTLIST;
        End = ENDLIST;
    }
    ~_OPAL_DATA_LIST()
    {
        for(IOPAL_DATA_INOUT* &item : List)
            delete item;
        List.clear();
    }
    void PushOpalItem(IOPAL_DATA_INOUT *item);
    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
} OPAL_DATA_LIST;

#if 0
typedef struct _OPAL_SESSION_ARG : public _OPAL_DATA_LIST {
    OPAL_DATA_ATOM HostSessionID;
    OPAL_DATA_ATOM SPID;
    OPAL_DATA_ATOM ReadWrite;

    size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    void PutOpalBytes(BYTE* buffer, size_t max_len);
    size_t GetOpalDataLen();
} OPAL_SESSION_ARG;
#endif
typedef struct _OPAL_DATA_LIST OPAL_COMMAND_ARG;

typedef struct _OPAL_CMD_PAYLOAD : public IOPAL_DATA_INOUT {
    BYTE CallToken = (BYTE)CALL;
    OPAL_DATA_ATOM InvokingUID;
    OPAL_DATA_ATOM Method;
    OPAL_COMMAND_ARG ArgList;

    //size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    //void PutOpalBytes(BYTE* buffer, size_t max_len);
    //size_t GetOpalDataLen();
}OPAL_CMD_PAYLOAD;

class COpalCmdBase{
public:
    COpalCmdBase();
    ~COpalCmdBase();

    virtual void CompleteCmd() = 0;     //add empty list, add padding, calculate Length field in packets...etc.
protected:
    //sync Length fields of ComPacket, Packet, and SubPacket.
    virtual void UpdatePacketLength() = 0;

protected:
    OPAL_COMPACKET ComPacket;
    OPAL_PACKET Packet;
    OPAL_DATA_SUB_PACKET SubPacket;
    OPAL_CMD_PAYLOAD Payload;

    //BYTE MethodStatusList[5] = { 0xF0, 0, 0, 0, 0xF1 };
    vector<BYTE> Padding;  //Packet::Length need align to DWORD, so put some padding...

    BYTE CmdBuf[PAGE_SIZE] = {0};
    size_t CmdLength = 0;
};


//class COpalSession : public COpalCmdBase {
//    OPAL_SESSION_ARG Payload;
//    void UpdatePacketLength();
//};

class COpalCommand : public COpalCmdBase {

public:
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method);
    ~COpalCommand();


    void CompleteCmd();     //add empty list, add padding, calculate Length field in packets...etc.
    void UpdatePacketLength();

    //size_t GetOpalBytes(BYTE* buffer, size_t max_len);
    //void PutOpalBytes(BYTE* buffer, size_t max_len);
    //size_t GetOpalDataLen();
};

