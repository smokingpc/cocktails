// AVX2Test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <immintrin.h>
#include <stdio.h>

//SSE/AVX reference: Intel intrinsics guide.
//https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#techs=AVX_ALL&ig_expand=877

#define TEST_LOOP 10000000

void NormalCalculation(DWORD loop)
{
    LARGE_INTEGER begin={0}, end={0}, freq = {0};
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&begin);

    QueryPerformanceCounter(&end);

    _tprintf(_T("[NormalCalculation] PerfCounter freq=%lld, time cost=%lld, begin=%lld, end=%lld\n"),
            freq.QuadPart, end.QuadPart-begin.QuadPart, begin.QuadPart, end.QuadPart);
    _tprintf(_T("[NormalCalculation] Loop=%d, TotalTime cost=%lf seconds\n"),
        loop, ((double)(end.QuadPart - begin.QuadPart)/(double)freq.QuadPart));
}

void AVXCalculation(DWORD loop)
{
    LARGE_INTEGER begin = { 0 }, end = { 0 }, freq = { 0 };
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&begin);


    QueryPerformanceCounter(&end);
    _tprintf(_T("[AVXCalculation] PerfCounter freq=%lld, time cost=%lld, begin=%lld, end=%lld\n"),
        freq.QuadPart, end.QuadPart - begin.QuadPart, begin.QuadPart, end.QuadPart);
    _tprintf(_T("[AVXCalculation] Loop=%d, TotalTime cost=%lf seconds\n"),
        loop, ((double)(end.QuadPart - begin.QuadPart) / (double)freq.QuadPart));
}

int _tmain(int argc, TCHAR* argv[])
{
    NormalCalculation(TEST_LOOP);
    AVXCalculation(TEST_LOOP);

    return 0;
}
