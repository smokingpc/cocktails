#include "Common.h"
using namespace std;

static std::map<OPAL_UID_TAG, vector<BYTE>> INVOKE_UIDs =
{
    //{SESSION_MGMT,      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }},

    // UID of Users 
    {SMUID,      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }},
    {THISSP,            { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }},
    {ADMINSP,           { 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x01 }},
    {LOCKINGSP,         { 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x02 }},
    {ENT_LOCKINGSP,     { 0x00, 0x00, 0x02, 0x05, 0x00, 0x01, 0x00, 0x01 }},
    {ANYBODY,           { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01 }},
    {SECURITY_ID,               { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06 }},
    {ADMIN1,            { 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0x00, 0x01 }},
    {USER1,             { 0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x00, 0x01 }},
    {USER2,             { 0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x00, 0x02 }},
    {PSID_USER,              { 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0xff, 0x01 }},
    {ENT_BANDMASTER0,   { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x80, 0x01 }},
    {ENT_ERASEMASTER,   { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x84, 0x01 }},

    //UID of Tables
    {LOCKRANGE_GLOBAL,  { 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x00, 0x01 }},
    {LOCKRANGE_ACE_READDLOCK,   { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE0, 0x01 }},
    {LOCKRANGE_ACE_WRITELOCK,   { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE8, 0x01 }},
    {LOCKRANGE_GLOBAL_ACE_READLOCK,     { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE0, 0x00 }},
    {LOCKRANGE_GLOBAL_ACE_WRITELOCK,    { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE8, 0x00 }},
    {MBRCONTROL_SET_DONE_TO_DOR,        { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xF8, 0x01 }},
    {MBR_CONTROL,       { 0x00, 0x00, 0x08, 0x03, 0x00, 0x00, 0x00, 0x01 }},

    {SHADOW_MBR,        { 0x00, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00 }},
    {AUTHORITY_TABLE,   { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00}},
    {C_PIN_TABLE,       { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00}},
    {OPAL_LOCKING_INFO_TABLE,   { 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x01 }},
    {ENT_LOCKING_INFO_TABLE,    { 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00 }},

    //C_PIN_TABLE object ID's
    {C_PIN_MSID, { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x84, 0x02}}, //< C_PIN_MSID
    {C_PIN_SID, { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01}}, //< C_PIN_SID
    {C_PIN_ADMIN1, { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x01, 0x00, 0x01}}, //< C_PIN_ADMIN1
    //half UID's (only first 4 bytes used)
    {HALF_UID_AUTHORITY_OBJ_REF, { 0x00, 0x00, 0x0C, 0x05, 0xff, 0xff, 0xff, 0xff }}, // Half-UID - Authority_object_ref
    {HALF_UID_BOOLEAN_ACE, { 0x00, 0x00, 0x04, 0x0E, 0xff, 0xff, 0xff, 0xff }}, // Half-UID - Boolean ACE

    // special value for omitted optional parameter
    {HEXFF_OMITTER, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}}, //< HEXFF for omitted
};

static std::map<OPAL_METHOD_TAG, vector<BYTE>> METHOD_UIDs =
{
    {PROPERTIES, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01}}, /**< Properties */
    {STARTSESSION,{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02}}, /**<STARTSESSION */
    {REVERT,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x02}}, /**< Revert */
    {ACTIVATE,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x03}}, /**< Activate */
    {EGET,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06 }}, /**< Enterprise Get */
    {ESET,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07 }}, /**< Enterprise Set */
    {NEXT,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x08}}, /**< NEXT */
    {EAUTHENTICATE,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0c }}, /**< Enterprise Authenticate */
    {GETACL,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0d}}, /**< GetACL */
    {GENKEY,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x10}}, /**< GenKey */
    {REVERTSP,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x11}}, /**< revertSP */
    {GET,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16}}, /**<Get */
    {SET,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x17}}, /**< Set */
    {AUTHENTICATE,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1c}}, /**< Authenticate */
    {RANDOM,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x01}}, /**< Random */
    {ERASE,{ 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x08, 0x03 }}, /**< Erase */
};

static inline void UpdateSizeAndCursor(BYTE* &cursor, size_t &used, size_t &remains)
{
    remains -= used;
    cursor += used;
}

//determine OPAL_ATOM_TOKEN::xxx_BYTES_xx by bytes array length
static inline OPAL_ATOM_TOKEN GetAtomBytesToken(size_t size)
{
    OPAL_ATOM_TOKEN base = NO_TOKEN;
    if(size>=16)
        base = MID_BYTES;
    else if (size > 0)
        base = SHORT_BYTES;
    
    //check the definition of OPAL_ATOM_TOKEN.
    //each token are defined as "BASE + length"
    return (OPAL_ATOM_TOKEN)(base + size);
}

//determine OPAL_ATOM_TOKEN::xxx_BYTES_xx by sizeof(UINT data type)
static inline OPAL_ATOM_TOKEN GetAtomUintToken(size_t size)
{
    return (OPAL_ATOM_TOKEN)(SHORT_UINT + size);
}

#pragma region ======== OPAL_COMPACKET ========
size_t _OPAL_COMPACKET::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if(max_len < GetOpalDataLen())
        return 0;

    size = sizeof(Reserved);
    SwapEndian(&Reserved, (UINT32*) cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(ExtComID);
    RtlCopyMemory(cursor, ExtComID, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(OutstandingData);
    SwapEndian(&OutstandingData, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(MinTx);
    SwapEndian(&MinTx, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    SwapEndian(&Length, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_COMPACKET::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return ;

    size = sizeof(Reserved);
    SwapEndian((UINT32*)cursor, &Reserved);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(ExtComID);
    RtlCopyMemory(ExtComID, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(OutstandingData);
    SwapEndian((UINT32*)cursor, &OutstandingData);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(MinTx);
    SwapEndian((UINT32*)cursor, &MinTx);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    SwapEndian((UINT32*)cursor, &Length);
    UpdateSizeAndCursor(cursor, size, remain_size);
}
size_t _OPAL_COMPACKET::GetOpalDataLen() { return sizeof(_OPAL_COMPACKET); }
#pragma endregion

#pragma region ======== OPAL_PACKET ========
size_t _OPAL_PACKET::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return 0;

    size = sizeof(TSN);
    SwapEndian(&TSN, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(HSN);
    SwapEndian(&HSN, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(SeqNo);
    SwapEndian(&SeqNo, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Reserved);
    SwapEndian(&Reserved, (UINT16*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(AckType);
    SwapEndian(&AckType, (UINT16*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Ack);
    SwapEndian(&Ack, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    SwapEndian(&Length, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_PACKET::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return;

    size = sizeof(TSN);
    SwapEndian((UINT32*)cursor, &TSN);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(HSN);
    SwapEndian((UINT32*)cursor, &HSN);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(SeqNo);
    SwapEndian((UINT32*)cursor, &SeqNo);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Reserved);
    SwapEndian((UINT16*)cursor, &Reserved);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(AckType);
    SwapEndian((UINT16*)cursor, &AckType);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Ack);
    SwapEndian((UINT32*)cursor, &Ack);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    SwapEndian((UINT32*)cursor, &Length);
    UpdateSizeAndCursor(cursor, size, remain_size);
}
size_t _OPAL_PACKET::GetOpalDataLen() { return sizeof(_OPAL_PACKET); }
#pragma endregion

#pragma region ======== OPAL_DATA_ATOM ========
_OPAL_DATA_ATOM::_OPAL_DATA_ATOM()
{}
_OPAL_DATA_ATOM::_OPAL_DATA_ATOM(char* str, int strlen)
{
    PutString(str, strlen);
}
_OPAL_DATA_ATOM::_OPAL_DATA_ATOM(BYTE* data, int data_len)
{
    PutBytes(data, data_len);
}
_OPAL_DATA_ATOM::_OPAL_DATA_ATOM(BYTE* data)
{
    PutUID(data);
}
template<typename T>
_OPAL_DATA_ATOM::_OPAL_DATA_ATOM(T data)
{
    PutUint(data);
}

void _OPAL_DATA_ATOM::PutString(char* str, int strlen)
{
    PutBytes((BYTE*)str, strlen);
}
void _OPAL_DATA_ATOM::PutBytes(BYTE* data, int data_len)
{
    //todo: raise errors...
    if (data_len >= sizeof(Data))
        return;

    RtlCopyMemory(Data, data, data_len);
    Type = GetAtomBytesToken(data_len);
}
void _OPAL_DATA_ATOM::PutUID(BYTE* uid)
{
//UID is 8-bytes length predefined bytes array...
    PutBytes(uid, OPAL_UID_LEN);
}

template<typename T>
    void _OPAL_DATA_ATOM::PutUint(T data)
{
    SwapEndian(&data, (T *)Data);
    Type = GetAtomBytesToken(sizeof(T));
}


size_t _OPAL_DATA_ATOM::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = 0;
    size_t used_size = 0;
    size_t token_size = 0;
    size_t data_size = 0;
    if(NO_TOKEN == Type)
        return 0;

    //one byte data don't need to put type info. it is TinyAtom data.
    if (Type > MID_BYTES)
    {
        token_size = 2;
        data_size = (Type - MID_BYTES);
    }
    else if (Type > SHORT_BYTES)
    {
        //1 byte data don't need type info in opal output buffer.
        //such data is TINY ATOM DATA
        if(Type != SHORT_BYTES_1)
            token_size = 1;
        else
            token_size = 0;
        data_size = (Type - SHORT_BYTES);
    }
    else if(Type > SHORT_UINT)
    {
        token_size = 1;
        data_size = (Type - SHORT_UINT);
    }
    else
    {
        token_size = data_size = 0;
    }

    if(max_len < token_size + data_size)
        return 0;

    if(token_size + data_size)
        return 0;

    switch(token_size)
    {
        case 2:
            RtlCopyMemory(cursor, (UINT16*)&Type, token_size);
            break;

        case 1:
            RtlCopyMemory(cursor, (UINT8*)&Type, token_size);
            break;
    }
    UpdateSizeAndCursor(cursor, token_size, remain_size);

    switch (Type)
    {
        case SHORT_UINT2:
            SwapEndian((UINT16*)Data, (UINT16*)cursor);
            break;
        case SHORT_UINT4:
            SwapEndian((UINT32*)Data, (UINT32*)cursor);
            break;
        case SHORT_UINT8:
            SwapEndian((UINT64*)Data, (UINT64*)cursor);
            break;
        default:
            RtlCopyMemory(cursor, Data, data_size);
            break;
    }
    UpdateSizeAndCursor(cursor, data_size, remain_size);
END:
    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_DATA_ATOM::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    throw new exception("Not implemented.");
}
size_t _OPAL_DATA_ATOM::GetOpalDataLen()
{
    size_t token_size = 0;
    size_t data_size = 0;

    if (Type > MID_BYTES)
    {
        token_size = 2;
        data_size = (Type - MID_BYTES);
    }
    else if (Type > SHORT_BYTES)
    {
        //1 byte data don't need type info in opal output buffer.
        //such data is TINY ATOM DATA
        if (Type != SHORT_BYTES_1)
            token_size = 1;
        else
            token_size = 0;
        data_size = (Type - SHORT_BYTES);
    }
    else if (Type > SHORT_UINT)
    {
        token_size = 1;
        data_size = (Type - SHORT_UINT);
    }

    return (token_size + data_size);
}

void _OPAL_DATA_ATOM::operator= (_OPAL_DATA_ATOM& newdata)
{
    this->Type = newdata.Type;
    RtlCopyMemory(this->Data, newdata.Data, sizeof(this->Data));
}
#pragma endregion

//#pragma region ======== OPAL_DATA ========
//size_t _OPAL_DATA::GetOpalDataLen()
//{
//    return (sizeof(Start) + sizeof(End));
//}
//#pragma endregion

#pragma region ======== OPAL_DATA_PAIR ========
_OPAL_DATA_PAIR::_OPAL_DATA_PAIR()
{
    Start = STARTNAME;
    End = ENDNAME;
}
_OPAL_DATA_PAIR::_OPAL_DATA_PAIR(OPAL_DATA_ATOM name, OPAL_DATA_ATOM value)
{
    this->Name = name;
    this->Value = value;
}

void _OPAL_DATA_PAIR::operator= (_OPAL_DATA_PAIR& newdata)
{
    this->Start = newdata.Start;
    this->Name = newdata.Name;
    this->Value = newdata.Value;
    this->End = newdata.End;
}

size_t _OPAL_DATA_PAIR::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;
    size_t name_len = Name.GetOpalDataLen();

    if (max_len < GetOpalDataLen())
        return 0;
    
    //Binary structure(in opal buffer) of a DataPair is
    // <%Start Token%>
    //   <%Data Atom%> => Name
    //   <%Data Atom%> => Value
    // <%End Token%
    //it is almost same as BSON's KeyValue pair concept.

    if(name_len > 0)
    {
        size = sizeof(OPAL_DATA_TOKEN);
        RtlCopyMemory(cursor, &Start, size);
        UpdateSizeAndCursor(cursor, size, remain_size);

        size = Name.GetOpalBytes(cursor, remain_size);
        UpdateSizeAndCursor(cursor, size, remain_size);
    }

    size = Value.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    if (name_len > 0)
    {
        size = sizeof(OPAL_DATA_TOKEN);
        RtlCopyMemory(cursor, &End, size);
        UpdateSizeAndCursor(cursor, size, remain_size);
    }
    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_DATA_PAIR::PutOpalBytes(BYTE* buffer, size_t max_len)
{
//todo: fix the parsing from OPAL Buffer.
    throw new exception("not implemented");

    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return;

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(&Start, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    Name.PutOpalBytes(buffer, remain_size);
    size = Name.GetOpalDataLen();
    UpdateSizeAndCursor(cursor, size, remain_size);

    Value.PutOpalBytes(buffer, remain_size);
    size = Value.GetOpalDataLen();
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(&End, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);
}
size_t _OPAL_DATA_PAIR::GetOpalDataLen()
{
    //if no "Name" field, the data_pair become a primitive type. Only have "Value" field.
    //in such situation, "Start" and "End" tag are un-necessary.
    size_t name_len = Name.GetOpalDataLen();
    if(0 == name_len)
        return Value.GetOpalDataLen();
    return (sizeof(Start) + sizeof(End) + Name.GetOpalDataLen() + Value.GetOpalDataLen());
}
#pragma endregion

#pragma region ======== OPAL_DATA_LIST ========
void _OPAL_DATA_LIST::PushOpalItem(IOPAL_DATA_INOUT *item)
{
    List.push_back(item);
}
size_t _OPAL_DATA_LIST::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return 0;

    //Binary structure(in opal buffer) of a DataList is
    // <%StartList Token%>
    //   <%Data1%>
    //   <%Data2%>
    //   ...
    // <%EndList Token%

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(cursor, &Start, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    for(IOPAL_DATA_INOUT *item : List)
    {
        size = item->GetOpalBytes(cursor, remain_size);
        UpdateSizeAndCursor(cursor, size, remain_size);
    }

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(cursor, &End, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_DATA_LIST::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    //todo: fix the parsing from OPAL Buffer.
    throw new exception("not implemented");

    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return;

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(&Start, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);


    //Name.PutOpalBytes(buffer, remain_size);
    //size = Name.GetOpalDataLen();
    //UpdateSizeAndCursor(cursor, size, remain_size);

    //Value.PutOpalBytes(buffer, remain_size);
    //size = Value.GetOpalDataLen();
    //UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(&End, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);
}

size_t _OPAL_DATA_LIST::GetOpalDataLen()
{
    size_t ret = sizeof(Start) + sizeof(End);
    for(IOPAL_DATA_INOUT *data : List)
        ret += data->GetOpalDataLen();

    return ret;
}
#pragma endregion

#if 0
#pragma region ======== OPAL_SESSION_ARG ========
size_t _OPAL_SESSION_ARG::GetOpalDataLen()
{
    size_t ret = _OPAL_DATA_LIST::GetOpalDataLen();
    ret += HostSessionID.GetOpalDataLen();
    ret += SPID.GetOpalDataLen();
    ret += ReadWrite.GetOpalDataLen();
}
#pragma endregion
#endif

#pragma region ======== OPAL_PAYLOAD_HEADER ========
size_t _OPAL_PAYLOAD_HEADER::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return 0;

    size = sizeof(BYTE);
    RtlCopyMemory(cursor, &CallToken, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = InvokingUID.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = Method.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);
}
void _OPAL_PAYLOAD_HEADER::PutOpalBytes(BYTE* buffer, size_t max_len)
{ throw new exception("not implemented"); }
size_t _OPAL_PAYLOAD_HEADER::GetOpalDataLen()
{
    return sizeof(CallToken) + InvokingUID.GetOpalDataLen() + Method.GetOpalDataLen();
}
#pragma endregion

COpalCmdBase::COpalCmdBase()
{}
COpalCmdBase::~COpalCmdBase()
{}


COpalCommand::COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method)
{
    OPAL_DATA_PAIR* arg = NULL;
    Payload.InvokingUID.PutUID(INVOKE_UIDs[invoking].data());
    Payload.Method.PutUID(METHOD_UIDs[method].data());
    
    OPAL_DATA_ATOM name;
    OPAL_DATA_ATOM value;

    name.PutUint((BYTE) 0);
    value.
    arg = new OPAL_DATA_PAIR();
    //OPAL_DATA_ATOM* data = new OPAL_DATA_ATOM(INVOKE_UIDs[invoking].data());
    //Payload.ArgList.PushOpalItem();
}
COpalCommand::~COpalCommand(){}
void COpalCommand::UpdatePacketLength()
{
////todo: calculate correct padding...
//    SubPacket.Length = //(UINT32)Payload.size();
//    Packet.Length = (UINT32)(sizeof(OPAL_DATA_SUB_PACKET) + Payload.size() + Padding.size());
//    ComPacket.Length = (UINT32)(sizeof(OPAL_PACKET) + Packet.Length);
}
