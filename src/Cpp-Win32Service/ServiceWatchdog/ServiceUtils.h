#pragma once

DWORD SvcInstall();
DWORD SvcUninstall();

DWORD WriteToServiceRegistry(LPTSTR name, DWORD type, PVOID data, DWORD data_bytes);
DWORD ReadFromServiceRegistry(LPTSTR name, PVOID data, DWORD* data_bytes, DWORD* type);
