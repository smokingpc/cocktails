#include "Common.h"
using namespace std;

static const std::map<OPAL_UID_TAG, OPAL_UID> INVOKE_UID_MAP =
{
    //{SESSION_MGMT,      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }},

    // UID of Users 
    {SESSION_MGR,      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff }},
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

static const std::map<OPAL_METHOD_TAG, list<BYTE>> METHOD_UID_MAP =
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

#pragma region ======== OPAL_COMPACKET ========
size_t _OPAL_COMPACKET::GetOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t used_size = 0;
    size_t size = 0;

    size = sizeof(Reserved);
    if(remain_size < size)
        goto END;
    SwapEndian(&Reserved, (UINT32*) cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(ExtComID);
    if (remain_size < size)
        goto END;
    RtlCopyMemory(cursor, ExtComID, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(OutstandingData);
    if (remain_size < size)
        goto END;
    SwapEndian(&OutstandingData, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(MinTx);
    if (remain_size < size)
        goto END;
    SwapEndian(&MinTx, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    if (remain_size < size)
        goto END;
    SwapEndian(&Length, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);
        
END:    
    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_COMPACKET::PutOpalBytes(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = buffer;
    size_t remain_size = max_len;
    size_t size = 0;

    size = sizeof(Reserved);
    if (remain_size < size)
        goto END;
    SwapEndian((UINT32*)cursor, &Reserved);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(ExtComID);
    if (remain_size < size)
        goto END;
    RtlCopyMemory(ExtComID, cursor, size);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(OutstandingData);
    if (remain_size < size)
        goto END;
    SwapEndian((UINT32*)cursor, &OutstandingData);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(MinTx);
    if (remain_size < size)
        goto END;
    SwapEndian((UINT32*)cursor, &MinTx);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    if (remain_size < size)
        goto END;
    SwapEndian((UINT32*)cursor, &Length);
    UpdateSizeAndCursor(cursor, size, remain_size);

END:
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

    size = sizeof(TSN);
    if (remain_size < size)
        goto END;
    SwapEndian(&TSN, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(HSN);
    if (remain_size < size)
        goto END;
    SwapEndian(&HSN, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(SeqNo);
    if (remain_size < size)
        goto END;
    SwapEndian(&SeqNo, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Reserved);
    if (remain_size < size)
        goto END;
    SwapEndian(&Reserved, (UINT16*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(AckType);
    if (remain_size < size)
        goto END;
    SwapEndian(&AckType, (UINT16*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Ack);
    if (remain_size < size)
        goto END;
    SwapEndian(&Ack, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

    size = sizeof(Length);
    if (remain_size < size)
        goto END;
    SwapEndian(&Length, (UINT32*)cursor);
    UpdateSizeAndCursor(cursor, size, remain_size);

END:
    used_size = max_len - remain_size;
    return used_size;
}
void _OPAL_PACKET::PutOpalBytes(BYTE* buffer, size_t max_len)
{}
size_t _OPAL_PACKET::GetOpalDataLen() { return sizeof(_OPAL_PACKET); }
#pragma endregion

#pragma region ======== OPAL_DATA_ATOM ========
size_t _OPAL_DATA_ATOM::GetOpalDataLen()
{
    //if only 1 byte data, don't output atom token.
    if(1 == Data.size())
        return 1;
    //if data size < 16 => this is short atom => add 1 byte short atom token
    if(16 > Data.size())
        return Data.size() + 1;
    //if data size >= 16 => this is medium atom => add 2 bytes medium atom token
    return Data.size() + 2;
}
#pragma endregion

#pragma region ======== OPAL_DATA ========
size_t _OPAL_DATA::GetOpalDataLen()
{
    return (sizeof(Start) + sizeof(End));
}
#pragma endregion

#pragma region ======== OPAL_DATA_PAIR ========
size_t _OPAL_DATA_PAIR::GetOpalDataLen()
{
    return (_OPAL_DATA::GetOpalDataLen() + Name.GetOpalDataLen() + Value.GetOpalDataLen());
}
#pragma endregion

#pragma region ======== OPAL_DATA_LIST ========
size_t _OPAL_DATA_LIST::GetOpalDataLen()
{
    size_t ret = _OPAL_DATA::GetOpalDataLen();
    for(auto &data : List)
        ret += data.GetOpalDataLen();

    return ret;
}
#pragma endregion

#pragma region ======== OPAL_SESSION_ARG ========
size_t _OPAL_SESSION_ARG::GetOpalDataLen()
{
    size_t ret = _OPAL_DATA_LIST::GetOpalDataLen();
    ret += HostSessionID.GetOpalDataLen();
    ret += SPID.GetOpalDataLen();
    ret += ReadWrite.GetOpalDataLen();
}
#pragma endregion

#pragma region ======== OPAL_PAYLOAD_HEADER ========
size_t _OPAL_PAYLOAD_HEADER::GetOpalDataLen()
{
    return sizeof(CallToken) + Invoker.GetOpalDataLen() + Method.GetOpalDataLen();
}
#pragma endregion

COpalCmdBase::COpalCmdBase()
{}
COpalCmdBase::~COpalCmdBase()
{}

void COpalSession::UpdatePacketLength()
{
////todo: calculate correct padding...
//    SubPacket.Length = //(UINT32)Payload.size();
//    Packet.Length = (UINT32)(sizeof(OPAL_DATA_SUB_PACKET) + Payload.size() + Padding.size());
//    ComPacket.Length = (UINT32)(sizeof(OPAL_PACKET) + Packet.Length);
}
