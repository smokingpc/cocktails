#pragma once

//class COpalCmd
//{
//    virtual size_t ToBytes(OUT BYTE *ret_buf) = 0;   //caller should delete[] ret_buf .
//    //virtual void ReleaseBytes(BYTE* ret_buf) = 0;
//};

class COpalBlock {
public:
    COpalBlock();
    COpalBlock(OPAL_UID_TAG tag);
    ~COpalBlock();

    OPAL_UID_TAG Tag;
    BYTE BeginToken[OPAL_UID_LEN] = {0};
    BYTE EndToken[OPAL_UID_LEN] = {0};
protected:
    friend class COpalPacket;
    size_t ToBytes(OUT BYTE* ret_buf);//caller should delete[] ret_buf .
};


class COpalPacket{
public:
    COpalPacket();
    ~COpalPacket();

    void PushCmd(COpalBlock &cmd);

private:
//all COpalBlock of this packet are pushed into vector.
//when push to OS command, copy this vector(as an array) to OS command payload buffer.
    //vector<COpalBlock> CmdList;
    std::vector<BYTE> BytesBlob;
};


class CCmd_StartSession : public COpalPacket {
public:
    CCmd_StartSession();
    ~CCmd_StartSession();


};

class CCmd_Discovery0 : public COpalPacket {
public:
    CCmd_Discovery0();
    ~CCmd_Discovery0();
};
