#pragma once
//================================================================================
//The TCG Storage OPAL-SSC standard defines Self-Encrypted Drive and related storage security features.
//This library "SimpleSED" is built to communicate with OPAL devices more easily.
//2022-12-01 Roy Wang. 
//================================================================================

// [Overview]
// Let's define 2 roles first:
//      "Host" is the computer which run the program.
//      "Device" is the StorageDevice. It could be SATA / SAS / NVMe devices.
// Communication between Host and Device are packet based. TCG defined packet based
//  structures to carry data in communication:
//      |-----------------------------------|
//      |          ComPacket                | 
//      ||---------------------------------||
//      ||          Packet                 ||
//      |||-------------------------------|||
//      |||        SubPacket              |||
//      ||||-----------------------------||||
//      ||||        Payload              ||||
//      ||||-----------------------------||||
//      ||| end of payload (in SubPacket) |||
//      |||-------------------------------|||
//      ||        Padding (in Packet)      ||
//      ||---------------------------------||
//      |-----------------------------------|
// 
// ComPacket carries Packet, Packet carries SubPacket/Paddings, and SubPacket carries 
// payload/end of payload(special structure).
// 
// The ComPacket::Length should be sizeof(entire Packet blob).
// The Packet::Length should be sizeof(entire SubPacket blob) + Padding Size. Because
//   the entire Packet structure must be be 4-bytes aligned. So we have to add paddings 
//   after SubPacket structure. 
// The SubPacket::Length should be sizeof(payload) + sizeof(end of payload).
// 
// The "end of payload" is a special bytes sequence to represent "the Payload is end here!".
// It is 5 bytes data => 0xF0 0x00 0x00 0x00 0xF1 .
// Because "begin from 0xF0 and end on 0xF1" is same as a OPAL LIST, so I use this structure 
// to represent it.
// 
// [NOTE]A very important reminding: All data in OPAL communication are BIG-ENDIAN!

// [Data Structure defined in Payload]
// Currently there are 3 types of data element in Payload:
//      DataAtom
//      DataList
//      NamePair
// It is very similar as JSON concept, but is binary based.
// (More similar as BSON concept?)
// 
// In this Library, "OpalBuffer" and "OpalBytes" indicate the buffer used to communicate 
// with OPAL Devices. (including send to and received from Opal Devices.
// This buffer must have ComPacket/Packet/SubPacket headers, follow rules of payload, have 
// "end of payload" bytes, and paddings.
// 
// Please refer to comments of each class for detailed explaination .


class COpalData
{
public:
    virtual ~COpalData(){}
    
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

// DataAtom is smallest element of data. Each DataAtom has a "Type" token 
// (represent the "data type" and "bytes length of following array"), and a 
// "Value" to store data in BigEndian Bytes Array. 
//
// Numeric data stored in COpalDataAtom::Data is Little-Endian. 
// (**Maybe change it to Big-Endian is better?)
//
// When exporting data to communication buffer, there are some special rules:
// 1. If there is only 1-byte data, skip "Type" field in output. Such data 
//    is "TinyAtom". I guess data range of TinyAtom is 0x00~0x7F, because the
//    atom token value range definition.(refer to OPAL_ATOM_TOKEN)
// 2. Integer value is always "unsigned".
// 3. To reduce memory space using, there could be 3-bytes int, 5-bytes int...etc.
//    Integer is not always fit UINT16 , UINT32, UINT64....
class COpalDataAtom : public COpalData {
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
            temp = *(UINT16*)Data;
            //SwapEndian((UINT16*)Data, (UINT16*)&temp);
            data = (T)temp;
            break;
        }
        case OPAL_ATOM_TOKEN::SHORT_UINT4:
        {
            UINT32 temp = 0;
            temp = *(UINT32*)Data;
            //SwapEndian((UINT32*)Data, (UINT32*)&temp);
            data = (T)temp;
            break;
        }
        case OPAL_ATOM_TOKEN::SHORT_UINT8:
        {
            UINT64 temp = 0;
            temp = *(UINT64*)Data;
            //SwapEndian((UINT64*)Data, (UINT64*)&temp);
            data = (T)temp;
            break;
        }
        case OPAL_ATOM_TOKEN::SHORT_UINT3:
            if(sizeof(T) >= 3)
            {
                //SwapEndian((T*)Data, &data);
                memcpy(&data, (T*)Data, 3);
            }
            break;
        case OPAL_ATOM_TOKEN::SHORT_UINT5:
        case OPAL_ATOM_TOKEN::SHORT_UINT6:
        case OPAL_ATOM_TOKEN::SHORT_UINT7:
            if (sizeof(T) >= 7)
            {
                //SwapEndian((T*)Data, &data);
                memcpy(&data, (T*)Data, 7);
            }
            break;

        case OPAL_ATOM_TOKEN::SHORT_UINT1:
        case OPAL_ATOM_TOKEN::SHORT_BYTES_1:
            data = (T)Data[0];
            break;
        default:
            throw new exception("Unsupported ATOM type!!!");
        }
    }

    void PutToken(OPAL_DATA_TOKEN token)
    {
        PutUint((UINT8)token);
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

// NamePair is similar as JSON's name-value pair.
// Each NamePair in bytes array has a begin token(0xF2) and end token(0xF3). 
// It is same concept as JSON "{ }" concept, just using different bracket marks.
// 
// There are 2 fields in NamePair: "Name" and "Value".
// The "Name" field is always a DataAtom.
// The "Value" field can be a DataAtom, NamePair, or List(OPAL DataList, not stl list).
// 
// Because the "Value" could be different data element, I used RTTI skill to 
// dynamically generate(or copy) new data objects.
class COpalNamePair : public COpalData {
public:
    COpalNamePair();
    COpalNamePair(COpalNamePair &newdata);
    COpalNamePair(COpalNamePair *newdata);
    COpalNamePair(COpalDataAtom& name);
    COpalNamePair(COpalDataAtom& name, COpalData *value);
    virtual ~COpalNamePair();
    void operator= (COpalNamePair& newdata);
    void Reset();
    void Set(COpalDataAtom& name, COpalData* value);
    void Get(COpalDataAtom& name, COpalData** value);

    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();

private:
    const OPAL_DATA_TOKEN Start = OPAL_DATA_TOKEN::STARTNAME;
    const OPAL_DATA_TOKEN End = OPAL_DATA_TOKEN::ENDNAME;
    COpalDataAtom Name;
    COpalData *Value = NULL;
    void CopyValue(COpalData* newdata);
};

// DataList is also similar as JSON's list.
// In JSON, a list has brackets "[  ]". Opal DataList use begin mark 0xF0 and 
// end mark 0xF1.
// 
// Data in DataList also can be DataAtom, NamePair, or DataList.
// Like NamePair, also use RTTI skill to dynamically generate(or copy) new data objects.
// 
class COpalList : public COpalData {
public:
    COpalList();
    COpalList(COpalList &newlist);
    COpalList(COpalList *newlist);
    virtual ~COpalList();
    void Reset();
    void PushOpalItem(COpalData *item);
    void PushOpalItem(COpalData &item);
    void GetRawList(list<COpalData*>& list);
    size_t ToOpalBytes(BYTE* buffer, size_t max_len);
    size_t FromOpalBytes(BYTE* buffer, size_t max_len);

    size_t OpalDataLen();
    void operator= (COpalList& newlist);

private:
    const OPAL_DATA_TOKEN Start = OPAL_DATA_TOKEN::STARTLIST;
    const OPAL_DATA_TOKEN End = OPAL_DATA_TOKEN::ENDLIST;
    list<COpalData*> List;

    void CopyList(list<COpalData*> &newlist);
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

    void PushOpalItem(COpalData *newarg);
    void PushOpalItem(COpalData &newarg);
    void GetArgList(COpalList& result);
    void GetArgList(list<COpalData*>& list);

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
    //**MethodStatusList is named by official example....
    COpalList MethodStatusList; 
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
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method);
    //normal operation, Payload will be COpalCmdPayload
    COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method, USHORT comid);
    ~COpalCommand();

    void PushCmdArg(COpalData &item);
    void PushCmdArg(COpalData *item);
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
    COpalData *Payload = nullptr;

    UINT32 PaddingSize = 0;
    UINT32 CmdLength = 0;
};

//**Response is not completely implemented.
// COpalResponse is used to parse responsed bytes from Device.
// Currently it is still under developing.
// It only can parse "Properties" command, and Locking/Unlocking command.
// 
// Response data is very similar as Command.
// They has same structure: 
//      ComPacket
//      Packet
//      SubPacket
//      Payload of SubPacket (refer to COpalCmdPayload comments)
//      Paddings
// 
// For Response of "End of Session", it is same as "End of Session" command.
// Device replies same data(as command) to notify Host : "End of Session" complete.
//

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
    void GetPayload(list<COpalData*> &result);
    
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
    //COpalData* Payload = nullptr;

    BYTE* RespBuf = nullptr;
    size_t RespBufSize = 0;
    BYTE* PayloadBegin = nullptr;
    size_t PayloadMaxSize = 0;
};


bool GetInvokeUID(OPAL_UID_TAG tag, vector<BYTE> &result);
