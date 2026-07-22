// boost_thread_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#define BOOST_USE_WINDOWS_H
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>

#include <boost/thread.hpp>
//#include <boost/thread/detail/thread.hpp>

void Test_ThreadInterrupt(HANDLE stop_event) {
    try {
        DWORD timeout = 1 * 1000;  //3 SECONDs
        int i = 0;
        _tprintf(_T("%s() => begin.\n"), _T(__FUNCTION__));
        while (WAIT_TIMEOUT == WaitForSingleObject(stop_event, timeout)) {
            _tprintf(_T("%s(%d)\n"), _T(__FUNCTION__), i++);
            boost::this_thread::interruption_point();
        }
    }
    catch (const boost::thread_interrupted&) {
        _tprintf(_T("%s() interrupted.\n"), _T(__FUNCTION__));
    }
    _tprintf(_T("%s() <= end.\n"), _T(__FUNCTION__));
}
void Test_SleepInterrupt(HANDLE stop_event) {
    try {
        DWORD timeout = 1 * 1000;  //3 SECONDs
        int i = 0;
        _tprintf(_T("%s() => begin.\n"), _T(__FUNCTION__));
        while (WAIT_TIMEOUT == WaitForSingleObject(stop_event, 0)) {
            _tprintf(_T("%s(%d)\n"), _T(__FUNCTION__), i++);
            boost::this_thread::sleep_for(boost::chrono::milliseconds(timeout));
        }
    }
    catch (const boost::thread_interrupted&) {
        _tprintf(_T("%s() interrupted.\n"), _T(__FUNCTION__));
    }
    _tprintf(_T("%s() <= end.\n"), _T(__FUNCTION__));
}

int _tmain(int argc, TCHAR* argv[])
{
    HANDLE stop_event = CreateEvent(NULL, TRUE, FALSE, NULL);

    boost::thread thread1 = boost::thread(&Test_ThreadInterrupt, stop_event);
    boost::thread thread2 = boost::thread(&Test_SleepInterrupt, stop_event);

    Sleep(4000);
    thread1.interrupt();
    thread2.interrupt();

    thread1.join();
    thread2.join();
}
