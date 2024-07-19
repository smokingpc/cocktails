#include "common.h"

bool IsOpalAtom(COpalData* ptr)
{
//using RTTI to check ptr type...
    return (nullptr != dynamic_cast<COpalDataAtom*>(ptr));
}
bool IsOpalList(COpalData* ptr)
{
    return (nullptr != dynamic_cast<COpalList*>(ptr));
}
bool IsOpalNamePair(COpalData* ptr)
{
    return (nullptr != dynamic_cast<COpalNamePair*>(ptr));
}
bool IsCmdPayload(COpalData* ptr)
{
    return (nullptr != dynamic_cast<COpalCmdPayload*>(ptr));
}

COpalData* DuplicateOpalData(COpalData* ptr)
{
    if(IsOpalAtom(ptr))
        return new COpalDataAtom((COpalDataAtom*)ptr);
    else if (IsOpalList(ptr))
        return new COpalList((COpalList*)ptr);
    else if (IsOpalNamePair(ptr))
        return new COpalNamePair((COpalNamePair*)ptr);

    return nullptr;
}
