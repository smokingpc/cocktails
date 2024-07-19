#pragma once

#define OPA_PWDHASH_LEN 32

//NOTE: in sedutil, its hash password length is 32bytes+2. additional 2 bytes are 0xD0 0x20.
//      0xD020 ==> OPAL_DATA_TOKEN::MID_BYTES_32

class COpalPwdHash {
public: 
    virtual ~COpalPwdHash(){}

    virtual int HashOpalPwd(
        BYTE* raw_pwd, UINT32 raw_pwd_size,
        UINT32 iter_count,
        BYTE* salt, UINT32 salt_size,
        BYTE* outbuf, UINT32 outbuf_size) = 0;

    virtual int HashOpalPwd(
        vector<BYTE>& raw_pwd, UINT32 iter_count,
        vector<BYTE>& salt, vector<BYTE>& outdata) = 0;
};


class COpalPwdHash_Win32 : public COpalPwdHash 
{
public :
    virtual ~COpalPwdHash_Win32();

    virtual int HashOpalPwd(
            BYTE* raw_pwd, UINT32 raw_pwd_size,
            UINT32 iter_count, 
            BYTE* salt, UINT32 salt_size,
            BYTE* outbuf, UINT32 outbuf_size);

    virtual int HashOpalPwd(
            vector<BYTE>& raw_pwd, UINT32 iter_count,
            vector<BYTE>& salt, vector<BYTE>& outdata)
    {
        outdata.reserve(OPA_PWDHASH_LEN);
        return HashOpalPwd(raw_pwd.data(), (UINT32)raw_pwd.size(), iter_count,
            salt.data(), (UINT32)salt.size(), outdata.data(), (UINT32)outdata.size());
    }
};