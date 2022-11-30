#include "common.h"
bool IsOpalAtom(COpalDataBase* ptr)
{
//using RTTI to check ptr type...
    return (nullptr != dynamic_cast<COpalDataAtom*>(ptr));
}
bool IsOpalList(COpalDataBase* ptr)
{
    return (nullptr != dynamic_cast<COpalList*>(ptr));
}
bool IsOpalNamePair(COpalDataBase* ptr)
{
    return (nullptr != dynamic_cast<COpalNamePair*>(ptr));
}
bool IsCmdPayload(COpalDataBase* ptr)
{
    return (nullptr != dynamic_cast<COpalCmdPayload*>(ptr));
}

COpalDataBase* DuplicateOpalData(COpalDataBase* ptr)
{
    if(IsOpalAtom(ptr))
        return new COpalDataAtom((COpalDataAtom*)ptr);
    else if (IsOpalList(ptr))
        return new COpalList((COpalList*)ptr);
    else if (IsOpalNamePair(ptr))
        return new COpalNamePair((COpalNamePair*)ptr);

    return nullptr;
}
