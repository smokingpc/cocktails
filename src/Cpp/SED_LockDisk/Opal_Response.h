#pragma once

class COpalResponse {
public:
    COpalResponse();
    COpalResponse(BYTE* buffer, size_t max_len);
    ~COpalResponse();

    //FromOpalBuffer() returns "how many bytes consumed in parsing?
    size_t FromOpalBuffer(BYTE* buffer, size_t max_len);
    void GetHeaders(COpalComPacket* compkt, COpalPacket* pkt, COpalSubPacket* subpkt);
    void GetHeaders(COpalComPacket& compkt, COpalPacket& pkt, COpalSubPacket& subpkt);
    void GetPayload(COpalCmdPayload& result);
    void GetPayload(list<COpalData*>& result);

    //Response of EndSession is different from other responses.
    //It only has 1 char (0xFA) following after Data SubPacket.
    //After 0xFA, there are only padding bytes.
    //No Invoking and Method, no CALL token, No List....
    inline bool IsEndSession()
    {
        return (OPAL_DATA_TOKEN::ENDOFSESSION == PayloadBegin[0]);
    }

private:
    COpalComPacket* ComPacket = nullptr;
    COpalPacket* Packet = nullptr;
    COpalSubPacket* SubPacket = nullptr;
    //COpalData* Payload = nullptr;

    BYTE* RespBuf = nullptr;
    size_t RespBufSize = 0;
    BYTE* PayloadBegin = nullptr;
    size_t PayloadMaxSize = 0;
};

