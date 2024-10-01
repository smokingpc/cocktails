// LockTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <iostream>
#include <mutex>
#include <chrono>

using std::mutex;
using std::recursive_mutex;
using std::timed_mutex;
using std::recursive_timed_mutex;

class CThreadBase {
public:
    CThreadBase(int idx, int data, DWORD time_len)
    {
        Index = Data = idx;
        Runtime = time_len;
        ThreadHandle = INVALID_HANDLE_VALUE;
        ThreadId = MAXDWORD;
    }

    operator HANDLE () { return ThreadHandle; }

    int Index;
    int Data;
    DWORD Runtime;  //how many milli-seconds thread will run?
    HANDLE ThreadHandle;
    DWORD ThreadId;
};

//try mutex
class CThreadMutex : CThreadBase{
public:
    CThreadMutex(int idx, int data, DWORD time_len) : 
        CThreadBase(idx, data, time_len){}
    ~CThreadMutex(){}
    static DWORD WINAPI MyThreadLockGuard(LPVOID arg)
    {
        CThreadMutex *ctx = (CThreadMutex*)arg;
    }
    static DWORD WINAPI MyThreadUniqueLock(LPVOID arg) 
    {
        CThreadMutex* ctx = (CThreadMutex*)arg;
    }
    static DWORD WINAPI MyThreadSharedLock(LPVOID arg)
    {
        CThreadMutex* ctx = (CThreadMutex*)arg;
    }

    mutex Mutex;
};

//try recursive_mutex
class CThreadRecursiveMutex : CThreadBase {
public:
    CThreadRecursiveMutex(int idx, int data, DWORD time_len) :
        CThreadBase(idx, data, time_len) {}
    ~CThreadRecursiveMutex() {}
    static DWORD WINAPI MyThreadLockGuard(LPVOID arg)
    {
        CThreadRecursiveMutex* ctx = (CThreadRecursiveMutex*)arg;
    }
    static DWORD WINAPI MyThreadUniqueLock(LPVOID arg)
    {
        CThreadRecursiveMutex* ctx = (CThreadRecursiveMutex*)arg;
    }
    static DWORD WINAPI MyThreadSharedLock(LPVOID arg)
    {
        CThreadRecursiveMutex* ctx = (CThreadRecursiveMutex*)arg;
    }

    recursive_mutex Mutex;
};

//try recursive_mutex
class CThreadTimedMutex : CThreadBase {
public:
    CThreadTimedMutex(int idx, int data, DWORD time_len) :
        CThreadBase(idx, data, time_len) {}
    ~CThreadTimedMutex() {}
    static DWORD WINAPI MyThreadLockGuard(LPVOID arg)
    {
        CThreadTimedMutex* ctx = (CThreadTimedMutex*)arg;
    }
    static DWORD WINAPI MyThreadUniqueLock(LPVOID arg)
    {
        CThreadTimedMutex* ctx = (CThreadTimedMutex*)arg;
    }
    static DWORD WINAPI MyThreadSharedLock(LPVOID arg)
    {
        CThreadTimedMutex* ctx = (CThreadTimedMutex*)arg;
    }

    timed_mutex Mutex;
};

//try recursive_timed_mutex
class CThreadRecursiveTimedMutex : CThreadBase {
public:
    CThreadRecursiveTimedMutex(int idx, int data, DWORD time_len) :
        CThreadBase(idx, data, time_len) {}
    ~CThreadRecursiveTimedMutex() {}
    static DWORD WINAPI MyThreadLockGuard(LPVOID arg)
    {
        CThreadRecursiveTimedMutex* ctx = (CThreadRecursiveTimedMutex*)arg;
    }
    static DWORD WINAPI MyThreadUniqueLock(LPVOID arg)
    {
        CThreadRecursiveTimedMutex* ctx = (CThreadRecursiveTimedMutex*)arg;
    }
    static DWORD WINAPI MyThreadSharedLock(LPVOID arg)
    {
        CThreadRecursiveTimedMutex* ctx = (CThreadRecursiveTimedMutex*)arg;
    }

    recursive_timed_mutex Mutex;
};

static DWORD WaitAllThreads(HANDLE t1, HANDLE t2, HANDLE t3, HANDLE t4)
{
    HANDLE wait[4] = {t1, t2, t3, t4};
    DWORD ret = WaitForMultipleObjects(4, wait, TRUE, INFINITE);
    if((WAIT_OBJECT_0 + 4) == ret)
        return ERROR_SUCCESS;
    return GetLastError();
}

int main()
{
//"mutex" family is the primitive lock object.
//"lock" family is encapsulation of lock objects, give mutex different behaviors.



    std::cout << "Hello World!\n";
}
