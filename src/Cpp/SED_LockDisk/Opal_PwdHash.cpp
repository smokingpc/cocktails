#include "Common.h"

#define SHA1_BLOCK_SIZE 64

COpalPwdHash_Win32::~COpalPwdHash_Win32(){}

int COpalPwdHash_Win32::HashOpalPwd(
    BYTE* raw_pwd, UINT32 raw_pwd_size,
    UINT32 iter_count,
    BYTE* salt, UINT32 salt_size,
    BYTE* outbuf, UINT32 outbuf_size)
{
    HCRYPTPROV provider = NULL;
    HCRYPTHASH hash_sha1 = NULL;
    int ret = 0;

    CryptAcquireContextA(&provider, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT);
    if (NULL == provider) {
        ret = -1;
        goto END;
    }
    CryptCreateHash(provider, CALG_SHA1, NULL, 0, &hash_sha1);
    if (NULL == hash_sha1) {
        ret = -1;
        goto END;
    }
    //pbkdf2 functions comes from https://github.com/hynninen/win-pbkdf2
    ret = pbkdf2_derive_bytes_hmac(hash_sha1, SHA1_BLOCK_SIZE, iter_count, 
                raw_pwd_size, raw_pwd, salt_size, salt, 
                outbuf_size, outbuf);
END:
    if (hash_sha1 != NULL)
        CryptDestroyHash(hash_sha1);
    if (provider != NULL)
        CryptReleaseContext(provider, 0);

    return ret;
}
