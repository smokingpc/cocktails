#pragma once

bool IdentifyStorageType(OUT STORAGE_BUS_TYPE& type, IN tstring& diskname);
size_t EnumPhysicalDisks(list<tstring> &result);
