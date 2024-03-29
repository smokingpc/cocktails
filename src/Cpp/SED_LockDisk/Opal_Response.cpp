#include "Common.h"


#pragma region ======== COpalResponse ========
COpalResponse::COpalResponse()
{}
COpalResponse::COpalResponse(BYTE* buffer, size_t max_len) : COpalResponse()
{
    FromOpalBuffer(buffer, max_len);
}
COpalResponse::~COpalResponse()
{
    if (nullptr != RespBuf)
    {
        delete[] RespBuf;
        RespBuf = nullptr;
    }

    if(nullptr != Payload)
    {
        delete Payload;
        Payload = nullptr;
    }
}
size_t COpalResponse::FromOpalBuffer(BYTE* buffer, size_t max_len)
{
    BYTE* cursor = NULL;
    size_t total_size = 0;
    ComPacket = (COpalComPacket*)buffer;
    total_size = SwapEndian(ComPacket->Length) + sizeof(COpalComPacket);

    RespBuf = new BYTE[total_size];
    memcpy(RespBuf, buffer, total_size);
    cursor = RespBuf;

    ComPacket = (COpalComPacket*)cursor;
    cursor += sizeof(COpalComPacket);

    Packet = (COpalPacket*)cursor;
    cursor += sizeof(COpalPacket);

    SubPacket = (COpalSubPacket*)cursor;
    cursor += sizeof(COpalSubPacket);
    PayloadBegin = cursor;

    if(!IsEndSession())
    {
        Payload = new COpalCmdPayload();
        Payload->FromOpalBytes(cursor, (cursor-buffer));
    }

    RespBufSize = total_size;
    PayloadMaxSize = RespBufSize - ((UINT64)cursor - (UINT64)RespBuf);
    return total_size;
}
void COpalResponse::GetHeaders(COpalComPacket* compkt, COpalPacket* pkt, COpalSubPacket* subpkt)
{
    GetHeader(compkt);
    GetHeader(pkt);
    GetHeader(subpkt);
}
void COpalResponse::GetHeader(COpalComPacket* compkt)
{
    if (nullptr != compkt)
        memcpy(compkt, ComPacket, sizeof(COpalComPacket));
}
void COpalResponse::GetHeader(COpalPacket* pkt)
{
    if (nullptr != pkt)
        memcpy(pkt, Packet, sizeof(COpalPacket));
}
void COpalResponse::GetHeader(COpalSubPacket* subpkt)
{
    if (nullptr != subpkt)
        memcpy(subpkt, SubPacket, sizeof(COpalSubPacket));
}

//void COpalResponse::GetHeaders(COpalComPacket& compkt, COpalPacket& pkt, COpalSubPacket& subpkt)
//{
//    return GetHeaders(&compkt, &pkt, &subpkt);
//}
void COpalResponse::GetPayload(COpalCmdPayload& result)
{
    if (IsEndSession())
        return;
    //result.FromOpalBytes(PayloadBegin, PayloadMaxSize);
    if(Payload != nullptr)
        result = *Payload;
}
void COpalResponse::GetPayload(list<COpalData*>& result)
{
    if (IsEndSession())
        return;

    //COpalCmdPayload temp;
    //temp.FromOpalBytes(PayloadBegin, PayloadMaxSize);
    //temp.GetArgList(result);
    if (Payload != nullptr)
        Payload->GetArgList(result);
}
#pragma endregion
