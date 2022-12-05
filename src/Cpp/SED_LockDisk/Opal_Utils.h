#pragma once
bool IsOpalAtom(COpalData *ptr);
bool IsOpalList(COpalData* ptr);
bool IsOpalNamePair(COpalData* ptr);
bool IsCmdPayload(COpalData* item);
COpalData* DuplicateOpalData(COpalData* item);
void UpdateSizeAndCursor(BYTE*& cursor, size_t& used, size_t& remains);
void UpdateSizeAndCursor(BYTE*& cursor, size_t& used, size_t& total_used, size_t& remains);

bool GetSpUID(OPAL_UID_TAG tag, vector<BYTE>& result);
BYTE* GetSpUID(OPAL_UID_TAG tag);
bool GetMethodUID(OPAL_METHOD_TAG tag, vector<BYTE>& result);
BYTE* GetMethodUID(OPAL_METHOD_TAG tag);
