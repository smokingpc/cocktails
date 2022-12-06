#include "Common.h"
using namespace std;

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
    //SessionID = 0;   //unique session id is built by TSN+HSN.
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
COpalDataAtom::COpalDataAtom(OPAL_DATA_TOKEN data)
{
    PutUint((UINT8)data);
}
void COpalDataAtom::PutString(char* str, size_t strlen)
{
    PutBytes((BYTE*)str, strlen);
}
void COpalDataAtom::PutBytes(BYTE* data, size_t data_len)
{
    Reset();

    //todo: raise errors...
    if (data_len > sizeof(Data) || 0 == data_len)
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
    if(cursor[0] >= 0xD0)
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
        used = sizeof(UINT8) + data_len;        //short_xxx type only occupy 1 byte, so take sizeof(UINT8);
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

        case OPAL_ATOM_TOKEN::SHORT_UINT3:
        case OPAL_ATOM_TOKEN::SHORT_UINT5:
        case OPAL_ATOM_TOKEN::SHORT_UINT6:
        case OPAL_ATOM_TOKEN::SHORT_UINT7:
        case OPAL_ATOM_TOKEN::SHORT_UINT9:
        case OPAL_ATOM_TOKEN::SHORT_UINT10:
        //reverse numberic data endian and copy them to this->Data
            for(int i=0; i<data_len; i++)
            {
                Data[(data_len-1)-i] = cursor[i];
            }
            break;
        default:
            break;
        }
        used = sizeof(UINT8) + data_len;        //short_xxx type only occupy 1 byte, so take sizeof(UINT8);
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
//    COpalNamePair *tmp = (COpalNamePair*)DuplicateOpalData(newdata);
    this->operator=(*newdata);
}
COpalNamePair::COpalNamePair(COpalDataAtom& name, COpalData* value) : COpalNamePair()
{
    Set(name, value);
}
COpalNamePair::COpalNamePair(COpalDataAtom* name, COpalData* value) : COpalNamePair()
{
    Set(name, value);
}
COpalNamePair::COpalNamePair(COpalDataAtom& name) : COpalNamePair()
{
    this->Name = name;
}
COpalNamePair::COpalNamePair(COpalDataAtom* name) : COpalNamePair()
{
    this->Name = *name;
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
void COpalNamePair::Set(COpalDataAtom& name, COpalData* value)
{
    this->Name = name;
    CopyValue(value);
}
void COpalNamePair::Set(COpalDataAtom* name, COpalData* value)
{
    this->Name = *name;
    CopyValue(value);
}
void COpalNamePair::Get(COpalDataAtom& name, COpalData** value)
{
    if(nullptr != value)
    {
        name = Name;
        *value = DuplicateOpalData(this->Value);
    }
}
//using RTTI to make sure COpalNamePair::Value contents will be copied
void COpalNamePair::CopyValue(COpalData* newdata)
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
    while (cursor[0] != OPAL_DATA_TOKEN::ENDNAME && total_used <= max_len)
    {

    //"Name" field in NamePair is always DataAtom.
    //"Value" field in NamePair could be List, NamePair, or DataAtom
        used = Name.FromOpalBytes(cursor, remain_size);
        UpdateSizeAndCursor(cursor, used, total_used, remain_size);

        COpalData* newdata = NULL;

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
void COpalList::CopyList(list<COpalData*>& newlist)
{
    for(COpalData *item : newlist)
    {
        PushOpalItem(*item);
    }
}
void COpalList::Reset()
{
    for(COpalData* item : List)
    {
        delete item;
    }
    List.clear();
}
void COpalList::PushOpalItem(COpalData *item)
{
    this->List.push_back(item);
}
void COpalList::PushOpalItem(COpalData &item)
{
    PushOpalItem(DuplicateOpalData(&item));
}
void COpalList::GetRawList(list<COpalData*>& list)
{
    for(COpalData* item : this->List)
    {
        list.push_back(DuplicateOpalData(item));
    }
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

    for(COpalData *item : List)
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
        COpalData* newdata = NULL;

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
    for(COpalData *item : List)
        ret += item->OpalDataLen();

    return ret;
}
#pragma endregion
