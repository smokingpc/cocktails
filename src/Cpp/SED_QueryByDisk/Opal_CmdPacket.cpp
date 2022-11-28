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

static inline void UpdateSizeAndCursor(BYTE*& cursor, size_t & used, size_t& total_used, size_t& remains)
{
    remains -= used;
    cursor += used;
    total_used += used;
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

#pragma region ======== COpalComPacket ========
size_t COpalComPacket::GetOpalBytes(BYTE* buffer, size_t max_len)
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
void COpalComPacket::PutOpalBytes(BYTE* buffer, size_t max_len)
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
size_t COpalComPacket::GetOpalDataLen() { return sizeof(COpalComPacket); }
void COpalComPacket::Reset()
{
    Reserved = 0;
    //0x07FE0000 in big endian
    ExtComID[0] = 0x07;
    ExtComID[1] = 0xFE;
    ExtComID[2] = 0x00;
    ExtComID[3] = 0x00;
    OutstandingData = 0;
    MinTx = 0;
    Length = 0;
}
#pragma endregion

#pragma region ======== COpalPacket ========
size_t COpalPacket::GetOpalBytes(BYTE* buffer, size_t max_len)
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
void COpalPacket::PutOpalBytes(BYTE* buffer, size_t max_len)
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
size_t COpalPacket::GetOpalDataLen() { return sizeof(COpalPacket); }
void COpalPacket::Reset()
{
    SessionID = 0;   //unique session id is built by TSN+HSN.
    SeqNo = 0;
    Reserved = 0;
    AckType = 0;
    Ack = 0;
    Length = 0;
}
#pragma endregion

#pragma region ======== COpalSubPacket ========
size_t COpalSubPacket::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return 0;

    size = sizeof(Reserved);
    RtlCopyMemory(cursor, Reserved, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Kind);
    SwapEndian(&Kind, (UINT16*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    SwapEndian(&Length, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
void COpalSubPacket::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < GetOpalDataLen())
        return;

    size = sizeof(Reserved);
    RtlCopyMemory(Reserved, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Kind);
    SwapEndian((UINT16*)cursor, &Kind);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    SwapEndian((UINT32*)cursor, &Length);
    UpdateSizeAndCursor(cursor, size, remain_size);
}
size_t COpalSubPacket::GetOpalDataLen() { return sizeof(COpalSubPacket); }
void COpalSubPacket::Reset()
{
    RtlZeroMemory(Reserved, sizeof(Reserved));
    Kind = 0;
    Length = 0;
}
#pragma endregion

#pragma region ======== COpalDataAtom ========
COpalDataAtom::COpalDataAtom()
{}

COpalDataAtom::COpalDataAtom(COpalDataAtom &newdata) : COpalDataAtom()
{
    this->operator=(newdata);
}
COpalDataAtom::COpalDataAtom(COpalDataAtom* newdata) : COpalDataAtom()
{
    this->operator=(*newdata);
}
COpalDataAtom::~COpalDataAtom()
{}
COpalDataAtom::COpalDataAtom(char* str, int strlen) : COpalDataAtom()
{
    PutString(str, strlen);
}
COpalDataAtom::COpalDataAtom(BYTE* data, int data_len) : COpalDataAtom()
{
    PutBytes(data, data_len);
}
COpalDataAtom::COpalDataAtom(BYTE* data) : COpalDataAtom()
{
    PutUID(data);
}

template<typename T>
COpalDataAtom::COpalDataAtom(T data) : COpalDataAtom()
{
    PutUint(data);
}

void COpalDataAtom::PutString(char* str, int strlen)
{
    PutBytes((BYTE*)str, strlen);
}
void COpalDataAtom::PutBytes(BYTE* data, int data_len)
{
    Reset();

    //todo: raise errors...
    if (data_len >= sizeof(Data) || 0 == data_len)
        return;

    RtlCopyMemory(Data, data, data_len);
    Type = GetAtomBytesToken(data_len);
}
void COpalDataAtom::PutUID(BYTE* uid)
{
    Reset();
    //UID is 8-bytes length predefined bytes array...
    PutBytes(uid, OPAL_UID_LEN);
}

size_t COpalDataAtom::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
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

    if(0 == (token_size + data_size))
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

    used_size = max_len - remain_size;
    return used_size;
}
void COpalDataAtom::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    throw new exception("Not implemented.");
}
size_t COpalDataAtom::GetOpalDataLen()
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

void COpalDataAtom::Reset()
{
    Type = OPAL_ATOM_TOKEN::NO_TOKEN;
    RtlZeroMemory(Data, sizeof(Data));
}

void COpalDataAtom::operator= (COpalDataAtom& newdata)
{
    this->Type = newdata.Type;
    RtlCopyMemory(this->Data, newdata.Data, sizeof(this->Data));
}
#pragma endregion

#pragma region ======== COpalNamePair ========
COpalNamePair::COpalNamePair()
{}

COpalNamePair::COpalNamePair(COpalNamePair& newdata) :COpalNamePair()
{
    this->operator=(newdata);
}

COpalNamePair::COpalNamePair(COpalNamePair* newdata) :COpalNamePair()
{
    this->operator=(*newdata);
}

COpalNamePair::COpalNamePair(COpalDataAtom& name, COpalDataBase* value) : COpalNamePair()
{
    Set(name, value);
}
COpalNamePair::COpalNamePair(COpalDataAtom& name) : COpalNamePair()
{
    this->Name = name;
}

COpalNamePair::~COpalNamePair()
{
    Reset();
}

void COpalNamePair::operator= (COpalNamePair& newdata)
{
    Set(newdata.Name, newdata.Value);
}

void COpalNamePair::Reset ()
{
    if (NULL != Value)
    {
        delete Value;
        Value = NULL;
    }
    Name.Reset();
}

void COpalNamePair::Set(COpalDataAtom& name, COpalDataBase* value)
{
    this->Name = name;
    CopyValue(value);
}

//using RTTI to make sure COpalNamePair::Value contents will be copied
void COpalNamePair::CopyValue(COpalDataBase* newdata)
{
    COpalDataAtom* atom = dynamic_cast<COpalDataAtom*> (newdata);
    COpalList* list = dynamic_cast<COpalList*> (newdata);
    COpalNamePair* pair = dynamic_cast<COpalNamePair*> (newdata);

    if (NULL != atom)
        this->Value = new COpalDataAtom(atom);
    else if (NULL != list)
        this->Value = new COpalList(list);
    else if (NULL != pair)
        this->Value = new COpalNamePair(pair);
}

size_t COpalNamePair::GetOpalBytes(BYTE* buffer, size_t max_len)
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

    size = Value->GetOpalBytes(cursor, remain_size);
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
void COpalNamePair::PutOpalBytes(BYTE* buffer, size_t max_len)
{
//todo: fix the parsing from OPAL Buffer.
    throw new exception("not implemented");
#if 0
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
#endif
}
size_t COpalNamePair::GetOpalDataLen()
{
    //if no "Name" field, the data_pair become a primitive type. Only have "Value" field.
    //in such situation, "Start" and "End" tag are un-necessary.
    size_t name_len = Name.GetOpalDataLen();
    size_t data_len = (NULL!=Value)? Value->GetOpalDataLen() : 0;
    return (sizeof(Start) + sizeof(End) + name_len, data_len);
}
#pragma endregion

#pragma region ======== COpalList ========
COpalList::COpalList()
{}

COpalList::COpalList(COpalList& newlist) : COpalList()
{
    this->operator=(newlist);
}

COpalList::COpalList(COpalList* newlist) : COpalList()
{
    this->operator=(*newlist);
}

COpalList::~COpalList()
{
    Reset();
}

void COpalList::operator= (COpalList& newlist)
{
    Reset();
    CopyList(newlist.List);
}

//using RTTI to make sure COpalList::List contents will be copied
void COpalList::CopyList(list<COpalDataBase*>& newlist)
{
    for(COpalDataBase *item : newlist)
    {
        PushOpalItem(*item);
    }
}

void COpalList::Reset()
{
    for(COpalDataBase* item : List)
    {
        delete item;
    }
    List.clear();
}

void COpalList::PushOpalItem(COpalDataBase *item)
{
    PushOpalItem(*item);
}

void COpalList::PushOpalItem(COpalDataBase &item)
{
    COpalDataAtom* atom = dynamic_cast<COpalDataAtom*> (&item);
    COpalList* list = dynamic_cast<COpalList*> (&item);
    COpalNamePair* pair = dynamic_cast<COpalNamePair*> (&item);

    if (NULL != atom)
        this->List.push_back(new COpalDataAtom(atom));
    else if (NULL != list)
        this->List.push_back(new COpalList(list));
    else if (NULL != pair)
        this->List.push_back(new COpalNamePair(pair));
}

size_t COpalList::GetOpalBytes(BYTE* buffer, size_t max_len)
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

    for(COpalDataBase *item : List)
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
void COpalList::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    //todo: fix the parsing from OPAL Buffer.
    throw new exception("not implemented");
#if 0
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
#endif 
}

size_t COpalList::GetOpalDataLen()
{
    size_t ret = sizeof(Start) + sizeof(End);
    for(COpalDataBase *item : List)
        ret += item->GetOpalDataLen();

    return ret;
}
#pragma endregion

//#pragma region ======== OPAL_END_CALL ========
//COpalCmdPayload_END::COpalCmdPayload_END() : OPAL_DATA_LIST()
//{
//    //at the end of each OPAL_CMD_PAYLOAD, should push a empty OPAL_DATA_LIST with three bytes "0x00".
//    //it represents the end of "CallToken".
//    OPAL_DATA_ATOM atom((UINT8)0x00);
//    List.push_back(atom);
//    List.push_back(atom);
//    List.push_back(atom);
//}
//#pragma endregion

#pragma region ======== COpalCmdPayload ========
COpalCmdPayload::COpalCmdPayload(){}
COpalCmdPayload::COpalCmdPayload(BYTE* invoke_uid, BYTE* method_uid) : COpalCmdPayload()
{
    Set(invoke_uid, method_uid);
}
COpalCmdPayload::~COpalCmdPayload() 
{
    Reset();
}
void COpalCmdPayload::PushCmdArg(COpalDataBase* newarg)
{
    ArgList.PushOpalItem(newarg);
}
void COpalCmdPayload::PushCmdArg(COpalDataBase& newarg)
{
    PushCmdArg(&newarg);
}
size_t COpalCmdPayload::GetOpalBytes(BYTE* buffer, size_t max_len)
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

    size - ArgList.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
void COpalCmdPayload::PutOpalBytes(BYTE* buffer, size_t max_len)
{ throw new exception("not implemented"); }
size_t COpalCmdPayload::GetOpalDataLen()
{
    return sizeof(CallToken) + InvokingUID.GetOpalDataLen() + Method.GetOpalDataLen();
}
void COpalCmdPayload::Reset()
{
    InvokingUID.Reset();
    Method.Reset();
    ArgList.Reset();
}
void COpalCmdPayload::Set(BYTE* invoke_uid, BYTE* method_uid)
{
    Reset();
    InvokingUID.PutUID(invoke_uid);
    Method.PutUID(method_uid);
}
#pragma endregion

#pragma region ======== COpalCommand ========
COpalCommand::COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method) :PaddingSize(0), CmdLength(0)
{
    Payload.Set(INVOKE_UIDs[invoking].data(), METHOD_UIDs[method].data());
}
COpalCommand::~COpalCommand() {}
void COpalCommand::PushCmdArg(COpalDataBase &item)
{
    Payload.PushCmdArg(&item);
}

void COpalCommand::PushCmdArg(COpalDataBase *item)
{
    Payload.PushCmdArg(item);
}

size_t COpalCommand::BuildCmdBuffer(BYTE* buffer, size_t max_buf_size)
{
    size_t remain_size = max_buf_size;
    size_t data_size = 0;
    BYTE* cursor = buffer;

    if(CmdLength > max_buf_size)
        return 0;

    data_size = ComPacket.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = Packet.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = SubPacket.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = Payload.GetOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    //put padding bytes
    for(UINT32 i=0; i< PaddingSize; i++)
        cursor[i] = 0x00;

    return CmdLength;
}

//add empty list, add padding, calculate Length field in packets...etc.
void COpalCommand::CompleteCmd()
{
//at the end of each OPAL_CMD_PAYLOAD, should push a empty OPAL_DATA_LIST with three bytes "0x00".
//it represents the end of entire data payload.
    COpalList endlist;
    COpalDataAtom atom((UINT8) 0);
    endlist.PushOpalItem(atom);
    endlist.PushOpalItem(atom);
    endlist.PushOpalItem(atom);
    PushCmdArg(endlist);
    UpdatePacketLength();
}

void COpalCommand::UpdatePacketLength()
{
    UINT32 size = 0;
    SubPacket.Length = (UINT32) Payload.GetOpalDataLen();
    size = (UINT32)sizeof(COpalSubPacket) + SubPacket.Length;
    Packet.Length = (UINT32)ROUND_UP_ALIGN_2N(size, sizeof(UINT32));
    PaddingSize = Packet.Length - size;

    ComPacket.Length = (UINT32)sizeof(COpalPacket) + Packet.Length;
    CmdLength = ComPacket.Length + sizeof(COpalComPacket);
}
#pragma endregion
