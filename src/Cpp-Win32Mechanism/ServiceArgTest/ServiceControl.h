#pragma once

VOID SvcInstall();
VOID SvcUninstall();
VOID SvcStart(const TCHAR *svcname);
VOID SvcStop(const TCHAR* svcname);
