#pragma once
bool IsOpalAtom(COpalData *ptr);
bool IsOpalList(COpalData* ptr);
bool IsOpalNamePair(COpalData* ptr);
bool IsCmdPayload(COpalData* item);
COpalData* DuplicateOpalData(COpalData* item);
