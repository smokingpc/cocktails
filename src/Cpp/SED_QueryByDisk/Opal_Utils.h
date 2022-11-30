#pragma once
bool IsOpalAtom(COpalDataBase *ptr);
bool IsOpalList(COpalDataBase* ptr);
bool IsOpalNamePair(COpalDataBase* ptr);
bool IsCmdPayload(COpalDataBase* item);
COpalDataBase* DuplicateOpalData(COpalDataBase* item);
