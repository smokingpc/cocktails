#pragma once

interface IOPAL_DATA_INOUT {
    //bytes endian of Opal Data Blob is BIG-ENDIAN
    size_t GetOpalBytes(vector<BYTE>& result);
    //bytes endian of Opal Data Blob is BIG-ENDIAN
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();     //get total length if output OpalData in BigEndian
};

interface IOPAL_DATA_ATOM {
    void PutString(char* str, int strlen);
    void PutBytes(BYTE* blob, int bloblen);
    void PutUint8(UINT8 data);
    void PutUint16(UINT16 data);
    void PutUint32(UINT32 data);
    void PutUint64(UINT64 data);
    void PutUID(OPAL_UID data);
};

#pragma pack(push)
#pragma pack(1)
typedef struct _OPAL_COMPACKET : public IOPAL_DATA_INOUT {
    UINT32 Reserved = 0;
    UINT8 ExtComID[4] = { 0x07,0xFE, 0, 0 };//0x07FE0000 in big endian
    UINT32 OutstandingData = 0;   //big endian
    UINT32 MinTx = 0;             //big endian
    UINT32 Length;                //(big endian) ==> sizeof(OPAL_PACKET) + OPAL_PACKET::Length

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen(){return sizeof(_OPAL_COMPACKET);}
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

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen() { return sizeof(_OPAL_PACKET); }
}OPAL_PACKET;

typedef struct _OPAL_DATA_SUB_PACKET : public IOPAL_DATA_INOUT {
    UINT8 Reserved[6] = { 0 };
    UINT16 Kind = 0;        //??
    UINT32 Length = 0;      //total length of following DataPayload block, NOT INCLUDING padding...

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
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
    list<BYTE> Data;            //if data only have 1 bytes, it is TinyAtom, don't output Type field.

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();
}OPAL_DATA_ATOM;

typedef struct _OPAL_DATA{
    OPAL_DATA_TOKEN Start;
    OPAL_DATA_TOKEN End;

    size_t GetOpalDataLen();
} OPAL_DATA;

typedef struct _OPAL_DATA_PAIR : public OPAL_DATA {
    OPAL_DATA_ATOM Name;
    OPAL_DATA_ATOM Value;
    _OPAL_DATA_PAIR()
    {
        Start= STARTNAME;
        End = ENDNAME;
    }

    void operator= (_OPAL_DATA_PAIR &newdata);

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();
} OPAL_DATA_PAIR;

typedef struct _OPAL_DATA_LIST : public OPAL_DATA, IOPAL_DATA_INOUT {
    list<OPAL_DATA_PAIR> List;

    _OPAL_DATA_LIST()
    {
        Start = STARTLIST;
        End = ENDLIST;
    }

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();
} OPAL_DATA_LIST;

typedef struct _OPAL_SESSION_ARG : public _OPAL_DATA_LIST {
    OPAL_DATA_ATOM HostSessionID;
    OPAL_DATA_ATOM SPID;
    OPAL_DATA_ATOM ReadWrite;

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();
} OPAL_SESSION_ARG;

typedef struct _OPAL_DATA_LIST OPAL_COMMAND_ARG;

typedef struct _OPAL_PAYLOAD_HEADER : public IOPAL_DATA_INOUT {
    BYTE CallToken = (BYTE)CALL;
    OPAL_DATA_ATOM Invoker;
    OPAL_DATA_ATOM Method;

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();
}OPAL_PAYLOAD_HEADER;

class COpalCmdBase : IOPAL_DATA_INOUT {
public:
    COpalCmdBase();
//    COpalCmdBase(UINT32 tsn, UINT32 hsn);
    ~COpalCmdBase();

    size_t GetOpalBytes(vector<BYTE>& result);
    size_t PutOpalBytes(vector<BYTE>& result);
    size_t GetOpalDataLen();
protected:
    //sync Length fields of ComPacket, Packet, and SubPacket.
    virtual void UpdatePacketLength() = 0;

private:
    OPAL_COMPACKET ComPacket;
    OPAL_PACKET Packet;
    OPAL_DATA_SUB_PACKET SubPacket;
    OPAL_PAYLOAD_HEADER PayloadHeader;

    BYTE MethodStatusList[5] = { 0xF0, 0, 0, 0, 0xF1 };
    vector<BYTE> Padding;  //Is ComPacket::Length or Packet::Length need align to DWORD?
};

class COpalSession : public COpalCmdBase {
    OPAL_SESSION_ARG Payload;
    void UpdatePacketLength();
};

class COpalCommand : public COpalCmdBase {
    OPAL_COMMAND_ARG Payload;
    void UpdatePacketLength();
};

