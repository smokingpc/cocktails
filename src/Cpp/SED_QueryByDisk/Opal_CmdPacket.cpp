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
size_t COpalComPacket::ToOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if(max_len < OpalDataLen())
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

size_t COpalComPacket::OpalDataLen() { return sizeof(COpalComPacket); }
void COpalComPacket::Reset()
{
    Reserved = 0;
    memset(ExtComID, 0, sizeof(ExtComID));
    OutstandingData = 0;
    MinTx = 0;
    Length = 0;
}
#pragma endregion

#pragma region ======== COpalPacket ========
size_t COpalPacket::ToOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < OpalDataLen())
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

size_t COpalPacket::OpalDataLen() { return sizeof(COpalPacket); }
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
size_t COpalSubPacket::ToOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < OpalDataLen())
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

size_t COpalSubPacket::OpalDataLen() { return sizeof(COpalSubPacket); }
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
template<typename T> COpalDataAtom::COpalDataAtom(T data) : COpalDataAtom()
{
    PutUint(data);
}
void COpalDataAtom::PutString(char* str, size_t strlen)
{
    PutBytes((BYTE*)str, strlen);
}
void COpalDataAtom::PutBytes(BYTE* data, size_t data_len)
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
void COpalDataAtom::GetBytes(BYTE* buffer, size_t max_len)
{
    if(!IsBytes())
        return;

    size_t data_len = 0;
    if(Type > OPAL_ATOM_TOKEN::MID_BYTES)
    {
        data_len = (UINT16)Type - (UINT16)OPAL_ATOM_TOKEN::MID_BYTES;
    }
    else if (Type > OPAL_ATOM_TOKEN::SHORT_BYTES)
    {
        data_len = (UINT16)Type - (UINT16)OPAL_ATOM_TOKEN::SHORT_BYTES;
    }
    
    data_len = min(data_len, max_len);
    memcpy(buffer, Data, data_len);
}
size_t COpalDataAtom::ToOpalBytes(BYTE* buffer, size_t max_len)
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

    if(max_len < (token_size + data_size) || 0 == (token_size + data_size))
        return 0;

    //if datasize == 1, it is TINY_ATOM so no need to save type
    if (1 == data_size)
        token_size = 0;

    switch(token_size)
    {
        case 2:
            SwapEndian((UINT16*)&Type, (UINT16*)cursor);
            break;

        case 1:
            cursor[0] = (UINT8)Type;
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
size_t COpalDataAtom::FromOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t total_used = 0;
    size_t remain_size = max_len;
    size_t used = 0;
    UINT8 data_len = 0;
    Reset();

    //first element is OPAL_ATOM_TOKEN::MID_BYTESxxx
    if(cursor[0] > 0xD0)
    {
        SwapEndian((UINT16*)cursor, (UINT16*)&Type);
        data_len = (UINT16) Type - (UINT16)OPAL_ATOM_TOKEN::MID_BYTES;
        memcpy(Data, cursor+sizeof(UINT16), data_len);
        used = sizeof(UINT16) + data_len;
    }
    else if(cursor[0] > (UINT8)OPAL_ATOM_TOKEN::SHORT_BYTES)
    {
        Type = (OPAL_ATOM_TOKEN)cursor[0];
        data_len = (UINT16)Type - (UINT16)OPAL_ATOM_TOKEN::SHORT_BYTES;
        memcpy(Data, cursor + sizeof(UINT8), data_len);
        used = sizeof(UINT16) + data_len;
    }
    else if (cursor[0] > (UINT8)OPAL_ATOM_TOKEN::SHORT_UINT)
    {
        Type = (OPAL_ATOM_TOKEN)cursor[0];
        data_len = (UINT16)Type - (UINT16)OPAL_ATOM_TOKEN::SHORT_UINT;
        cursor++;

        switch (Type)
        {
        case OPAL_ATOM_TOKEN::SHORT_UINT1:
            //memcpy(Data, cursor + sizeof(UINT8), 1);
            Data[0] = cursor[0];
            break;
        case OPAL_ATOM_TOKEN::SHORT_UINT2:
            SwapEndian((UINT16*)cursor, (UINT16*)Data);
            break;
        case OPAL_ATOM_TOKEN::SHORT_UINT4:
            SwapEndian((UINT32*)cursor, (UINT32*)Data);
            break;
        case OPAL_ATOM_TOKEN::SHORT_UINT8:
            SwapEndian((UINT64*)cursor, (UINT64*)Data);
            break;
        default:
            break;
        }
        used = sizeof(UINT8) + data_len;
    }
    else
    {
        //this is TinyAtom datatype...
        Type = OPAL_ATOM_TOKEN::SHORT_UINT1;
        Data[0] = cursor[0];
        used = 1;
    }

//END:
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);
    return total_used;
}
size_t COpalDataAtom::OpalDataLen()
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
        if (Type != SHORT_UINT1)
            token_size = 1;
        else
            token_size = 0;
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
COpalNamePair::COpalNamePair(COpalNamePair& newdata) :COpalNamePair(&newdata)
{}
COpalNamePair::COpalNamePair(COpalNamePair* newdata) :COpalNamePair()
{
    COpalNamePair *tmp = (COpalNamePair*)DuplicateOpalData(newdata);
    this->operator=(*tmp);
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
void COpalNamePair::Get(COpalDataAtom& name, COpalDataBase** value)
{
    if(nullptr != value)
    {
        name = Name;
        *value = DuplicateOpalData(this->Value);
    }
}
//using RTTI to make sure COpalNamePair::Value contents will be copied
void COpalNamePair::CopyValue(COpalDataBase* newdata)
{
    this->Value = DuplicateOpalData(newdata);
}
size_t COpalNamePair::ToOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;
    size_t name_len = Name.OpalDataLen();

    if (max_len < OpalDataLen())
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

        size = Name.ToOpalBytes(cursor, remain_size);
        UpdateSizeAndCursor(cursor, size, remain_size);
    }

    size = Value->ToOpalBytes(cursor, remain_size);
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
size_t COpalNamePair::FromOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t total_used = 0;
    size_t remain_size = max_len;
    size_t used = 0;

    if (cursor[0] != (BYTE)OPAL_DATA_TOKEN::STARTNAME)
        return 0;

    used = 1;   //consume OPAL_DATA_TOKEN::STARTLIST token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    //following items only have 3 types: Atom, List, NamePair.
    //there are many objects. so keep loop to parse until OPAL_DATA_TOKEN::ENDLIST met.
    while (cursor[0] != OPAL_DATA_TOKEN::ENDNAME || total_used >= max_len)
    {

    //"Name" field in NamePair is always DataAtom.
    //"Value" field in NamePair could be List, NamePair, or DataAtom
        used = Name.FromOpalBytes(cursor, remain_size);
        UpdateSizeAndCursor(cursor, used, total_used, remain_size);

        COpalDataBase* newdata = NULL;

        if (cursor[0] == OPAL_DATA_TOKEN::STARTLIST)
        {
            newdata = new COpalList();
        }
        else if (cursor[0] == OPAL_DATA_TOKEN::STARTNAME)
        {
            newdata = new COpalNamePair();
        }
        else
        {
            newdata = new COpalDataAtom();
        }
        used = newdata->FromOpalBytes(cursor, remain_size);
        Value = newdata;
        UpdateSizeAndCursor(cursor, used, total_used, remain_size);
    }

    used = 1;   //consume OPAL_DATA_TOKEN::ENDNAME token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    return total_used;
}
size_t COpalNamePair::OpalDataLen()
{
    //if no "Name" field, the data_pair become a primitive type. Only have "Value" field.
    //in such situation, "Start" and "End" tag are un-necessary.
    size_t name_len = Name.OpalDataLen();
    size_t data_len = (NULL!=Value)? Value->OpalDataLen() : 0;
    return (sizeof(Start) + sizeof(End) + name_len + data_len);
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
    this->List.push_back(item);
}
void COpalList::PushOpalItem(COpalDataBase &item)
{
    PushOpalItem(DuplicateOpalData(&item));
}
void COpalList::GetRawList(list<COpalDataBase*>& list)
{
    CopyList(list);
}
size_t COpalList::ToOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    if (max_len < OpalDataLen())
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
        size = item->ToOpalBytes(cursor, remain_size);
        UpdateSizeAndCursor(cursor, size, remain_size);
    }

    size = sizeof(OPAL_DATA_TOKEN);
    RtlCopyMemory(cursor, &End, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
size_t COpalList::FromOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t total_used = 0;
    size_t remain_size = max_len;
    size_t used = 0;

    if(cursor[0] != (BYTE)OPAL_DATA_TOKEN::STARTLIST)
        return 0;

    used = 1;   //consume OPAL_DATA_TOKEN::STARTLIST token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    //following items only have 3 types: Atom, List, NamePair.
    //there are many objects. so keep loop to parse until OPAL_DATA_TOKEN::ENDLIST met.
    while (cursor[0] != OPAL_DATA_TOKEN::ENDLIST || total_used >= max_len)
    {
        COpalDataBase* newdata = NULL;

        if (cursor[0] == OPAL_DATA_TOKEN::STARTLIST)
        {
            newdata = new COpalList();
        }
        else if (cursor[0] == OPAL_DATA_TOKEN::STARTNAME)
        {
            newdata = new COpalNamePair();
        }
        else
        {
            newdata = new COpalDataAtom();
        }
        used = newdata->FromOpalBytes(cursor, remain_size);
        List.push_back(newdata);
        UpdateSizeAndCursor(cursor, used, total_used, remain_size);
    }

    used = 1;   //consume OPAL_DATA_TOKEN::ENDLIST token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    return total_used;
}
size_t COpalList::OpalDataLen()
{
    size_t ret = sizeof(Start) + sizeof(End);
    for(COpalDataBase *item : List)
        ret += item->OpalDataLen();

    return ret;
}
#pragma endregion

#pragma region ======== COpalCmdPayload ========
COpalCmdPayload::COpalCmdPayload()
{
    //at the end of each COpalCmdPayload, should have a empty OPAL_DATA_LIST with three bytes "0x00".
    // (total 5 bytes, 1byte OPAL_DATA_TOKEN::STARTLIST + 3bytes 0x00 + 1byte OPAL_DATA_TOKEN::ENDLIST
    //it represents the end of entire data payload.
    COpalDataAtom atom((UINT8)0);
    MethodStatusList.PushOpalItem(atom);
    MethodStatusList.PushOpalItem(atom);
    MethodStatusList.PushOpalItem(atom);
}
COpalCmdPayload::COpalCmdPayload(BYTE* invoke_uid, BYTE* method_uid) : COpalCmdPayload()
{
    Set(invoke_uid, method_uid);
}
COpalCmdPayload::~COpalCmdPayload() 
{
    Reset();
}
void COpalCmdPayload::PushOpalItem(COpalDataBase* newarg)
{
    ArgList.PushOpalItem(newarg);
}
void COpalCmdPayload::PushOpalItem(COpalDataBase& newarg)
{
    PushOpalItem(&newarg);
}
void COpalCmdPayload::GetArgList(list<COpalDataBase*>& list)
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

    size = MethodStatusList.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    used_size = max_len - remain_size;
    return used_size;
}
size_t COpalCmdPayload::FromOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE *cursor = buffer;
    size_t total_used = 0;
    size_t remain_size = max_len;
    size_t used = 0;
    //command payload structure begin by OPAL_DATA_TOKEN::CALL
    if(cursor[0] != (BYTE)OPAL_DATA_TOKEN::CALL)
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

    used = 1;   //consume OPAL_DATA_TOKEN::ENDOFDATA token in buffer
    UpdateSizeAndCursor(cursor, used, total_used, remain_size);

    //skip MethodStatusList. It is useless. Only used to identify "CmdPayload ends"
    //I am too lazy to write parsing function so hardcode them here..... :p
    assert(cursor[0] == OPAL_DATA_TOKEN::ENDOFDATA);
    assert(cursor[1] == OPAL_DATA_TOKEN::STARTLIST);
    assert(cursor[2] == 0x00);
    assert(cursor[3] == 0x00);
    assert(cursor[4] == 0x00);
    assert(cursor[5] == OPAL_DATA_TOKEN::ENDLIST);

    return total_used;
}
size_t COpalCmdPayload::OpalDataLen()
{
//not include MethodStatusList length
    size_t size1 = sizeof(CallToken) + sizeof(EndCallToken);
    size_t size2 = InvokingUID.OpalDataLen();
    size_t size3 = Method.OpalDataLen();
    size_t size4 = ArgList.OpalDataLen();
//    return size1+size2+size3+size4;
    return sizeof(CallToken) + InvokingUID.OpalDataLen() + Method.OpalDataLen() + 
            ArgList.OpalDataLen() + sizeof(EndCallToken) + MethodStatusList.OpalDataLen();
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
{
    Payload = new COpalDataAtom();
}
COpalCommand::COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method)
{
    Payload = new COpalCmdPayload(INVOKE_UIDs[invoking].data(), METHOD_UIDs[method].data());
    //Payload->Set(INVOKE_UIDs[invoking].data(), METHOD_UIDs[method].data());
}
COpalCommand::COpalCommand(OPAL_UID_TAG invoking, OPAL_METHOD_TAG method, USHORT comid)
    : COpalCommand(invoking, method)
{
    SetBaseComID(comid);
}
COpalCommand::~COpalCommand() 
{
    if (nullptr != Payload)
    {
        delete Payload;
        Payload = nullptr;
    }
}
void COpalCommand::PushCmdArg(COpalDataBase &item)
{
    if (IsCmdPayload(&item))
        ((COpalCmdPayload*)Payload)->PushOpalItem(item);

//    Payload->PushOpalItem(&item);
}
void COpalCommand::SetBaseComID(USHORT comid)
{
    SwapEndian(&comid, (UINT16*)ComPacket.ExtComID);
}
USHORT COpalCommand::GetBaseComID()
{
    UINT16 comid = 0;
    SwapEndian((UINT16*)ComPacket.ExtComID, &comid);
    return comid;
}
void COpalCommand::PushCmdArg(COpalDataBase *item)
{
    if (IsCmdPayload(item))
        ((COpalCmdPayload*)Payload)->PushOpalItem(item);
}
size_t COpalCommand::BuildOpalBuffer(BYTE* buffer, size_t max_buf_size)
{
    size_t remain_size = max_buf_size;
    size_t data_size = 0;
    BYTE* cursor = buffer;

    UpdatePacketLength();
    if(CmdLength < max_buf_size)
        return 0;

    data_size = ComPacket.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = Packet.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = SubPacket.ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    data_size = Payload->ToOpalBytes(cursor, remain_size);
    UpdateSizeAndCursor(cursor, data_size, remain_size);

    //put padding bytes
    for(UINT32 i=0; i< PaddingSize; i++)
        cursor[i] = 0x00;

    size_t used_size = max_buf_size - remain_size;
    return used_size;
}
void COpalCommand::UpdatePacketLength()
{
    UINT32 size = 0;
    SubPacket.Length = (UINT32) Payload->OpalDataLen();
    size = (UINT32)sizeof(COpalSubPacket) + SubPacket.Length;
    Packet.Length = (UINT32)ROUND_UP_ALIGN_2N(size, sizeof(UINT32));
    PaddingSize = Packet.Length - size;
    size = sizeof(COpalPacket);
    ComPacket.Length = (UINT32)sizeof(COpalPacket) + Packet.Length;
    CmdLength = ComPacket.Length + sizeof(COpalComPacket);
}
#pragma endregion

#pragma region ======== COpalResponse ========
COpalResponse::COpalResponse()
{}
COpalResponse::COpalResponse(BYTE* buffer, size_t max_len) : COpalResponse()
{
    FromOpalBuffer(buffer, max_len);
}
COpalResponse::~COpalResponse()
{
    if(nullptr != RespBuf)
    {
        delete[] RespBuf;
        RespBuf = nullptr;
    }
}
size_t COpalResponse::FromOpalBuffer(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = RespBuf;
    size_t total_size = 0;
    ComPacket = (COpalComPacket*)buffer;
    total_size = ComPacket->Length + sizeof(COpalComPacket);

    RespBuf = new BYTE[total_size];
    memcpy(RespBuf, buffer, total_size);

    ComPacket = (COpalComPacket*)cursor;
    cursor += sizeof(COpalComPacket);
    
    Packet = (COpalPacket*)cursor;
    cursor += sizeof(COpalPacket);
    
    SubPacket = (COpalSubPacket*)cursor;
    cursor += sizeof(COpalSubPacket);
    PayloadBegin = cursor;

    PayloadMaxSize = ((UINT64)cursor - (UINT64)RespBuf);
    RespBufSize = total_size;
    return total_size;
}
void COpalResponse::GetHeaders(COpalComPacket* compkt, COpalPacket* pkt, COpalSubPacket* subpkt)
{
    memcpy(compkt, ComPacket, sizeof(COpalComPacket));
    memcpy(pkt, Packet, sizeof(COpalPacket));
    memcpy(subpkt, SubPacket, sizeof(COpalSubPacket));
}
void COpalResponse::GetHeaders(COpalComPacket& compkt, COpalPacket& pkt, COpalSubPacket& subpkt)
{
    return GetHeaders(&compkt, &pkt, &subpkt);
}
void COpalResponse::GetPayload(COpalCmdPayload& result)
{
    if(IsEndSession())
        return;
    result.FromOpalBytes(PayloadBegin, PayloadMaxSize);
}
void COpalResponse::GetPayload(list<COpalDataBase*>& result)
{
    if (IsEndSession())
        return;

    COpalCmdPayload temp;
    temp.FromOpalBytes(PayloadBegin, PayloadMaxSize);
    temp.GetArgList(result);
}
#pragma endregion
