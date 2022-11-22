#pragma once

#pragma pack(push)
#pragma pack(1)
typedef struct _OPAL_COMPACKET{
public:
    void SetOutstandingData(UINT32 value) { SwapEndian(&value, &OutstandingData); }
    UINT32 GetOutstandingData() { return SwapEndian(OutstandingData); }

    void SetMinTx(UINT32 value) { SwapEndian(&value, &MinTx); }
    UINT32 GetMinTx(UINT32 value) { return SwapEndian(MinTx); }

    void SetLength(UINT32 value) { SwapEndian(&value, &Length); }
    UINT32 GetLength() { return SwapEndian(Length); }
private:
    UINT32 Reserved = 0;
    UINT8 ExtComID[4] = {0x07,0xFE, 0, 0};//0x07FE0000 in big endian
    UINT32 OutstandingData = 0;   //big endian
    UINT32 MinTx = 0;             //big endian
    UINT32 Length;                //(big endian) ==> sizeof(OPAL_PACKET) + OPAL_PACKET::Length
}OPAL_COMPACKET;

typedef struct _OPAL_PACKET {
public:
    void SetTSN(UINT32 value) { SwapEndian(&value, &TSN); }
    UINT32 GetTSN() { return SwapEndian(TSN); }
    void SetHSN(UINT32 value) { SwapEndian(&value, &HSN); }
    UINT32 GetHSN() { return SwapEndian(HSN); }
    void SetSessionID(UINT64 value) { SwapEndian(&value, &SessionID); }
    UINT32 GetSessionID() { return SwapEndian(SessionID); }

    void SetSeqNo(UINT32 value) { SwapEndian(&value, &SeqNo); }
    UINT32 GetSeqNo() { return SwapEndian(SeqNo); }
    void SetAckType(UINT16 value) { SwapEndian(&value, &AckType); }
    UINT16 GetAckType() { return SwapEndian(AckType); }
    void SetAck(UINT32 value) { SwapEndian(&value, &Ack); }
    UINT32 GetAck() { return SwapEndian(Ack); }
    void SetLength(UINT32 value) { SwapEndian(&value, &Length); }
    UINT32 GetLength() { return SwapEndian(Length); }

private:
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
}OPAL_PACKET;

typedef struct _OPAL_DATA_SUB_PACKET {
public:
    void SetKind(UINT16 value) { SwapEndian(&value, &Kind); }
    UINT16 GetKind() { return SwapEndian(Kind); }

    void SetLength(UINT32 value) { SwapEndian(&value, &Length); }
    UINT32 GetLength() { return SwapEndian(Length); }

private:
    UINT8 Reserved[6] = {0};
    UINT16 Kind = 0;        //??
    UINT32 Length = 0;      //total length of following DataPayload block, NOT INCLUDING padding...
}OPAL_DATA_SUB_PACKET;
#pragma pack(pop)

class COpalCommand {
public:
    COpalCommand();
    COpalCommand(UINT32 tsn, UINT32 hsn);
    ~COpalCommand();

private:
    OPAL_COMPACKET ComPacket;
    OPAL_PACKET Packet;
    OPAL_DATA_SUB_PACKET SubPacket;

    std::vector<BYTE> Payload;
    std::vector<BYTE> Padding;  //Is ComPacket::Length or Packet::Length need align to DWORD?

    void UpdatePacketLength();        //sync Length fields of ComPacket, Packet, and SubPacket.
};
