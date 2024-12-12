// HelloWorldTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

int _tmain(int argc, TCHAR* argv[])
{
	BOOL pause = FALSE;
	if (argc > 1 && 0 == _tcsicmp(argv[1], _T("-pause")))
		pause = TRUE;

	_tprintf(_T("Hello World!\n"));
	if (pause) {
		_tprintf(_T("Pause and wait input...\n"));
		getchar();
	}
}
