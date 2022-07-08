#include "Common.h"

static inline void string_trim(string& str)
{
    std::size_t found = str.find_last_not_of(" ");
    if (found != string::npos)
        str.erase(found + 1);
    else
        str.clear();            // str is all whitespace
}

void BuildNqn(OUT tstring &result, PNVME_IDENTIFY_CONTROLLER_DATA14 identify)
{
    string ret = "";
    string subnqn = "";
    string model = "";
    string serial = "";
    string fw_rev = "";

    subnqn.assign((char*)identify->SUBNQN);
    if (subnqn == "" || identify->VER == 0)
    {
        int offset = snprintf((char*)identify->SUBNQN, 256,
            "nqn.2014.08.org.nvmexpress:%04x%04x",
            identify->VID, identify->SSVID);

        memcpy((char*)identify->SUBNQN + offset, identify->SN, sizeof(identify->SN));
        offset += sizeof(identify->SN);
        memcpy((char*)identify->SUBNQN + offset, identify->MN, sizeof(identify->MN));
        offset += sizeof(identify->MN);
        memset((char*)identify->SUBNQN + offset, 0, sizeof(identify->SUBNQN) - offset);
        subnqn = (char*)identify->SUBNQN;
    }

    string_trim(subnqn);
    ret.resize(subnqn.length());

#ifdef _UNICODE
    WStrToStr(ret, result);
#else
    result = ret;
#endif
}

