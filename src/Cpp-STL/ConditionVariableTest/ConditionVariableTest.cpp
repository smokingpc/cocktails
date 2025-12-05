// ConditionVariableTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <mutex>
#include <windows.h>

using namespace std;

void thread1(condition_variable* cv)
{
    Sleep(1000);
    //檢查 notify_one的組語發現它的邏輯是：
    //先呼叫wait( notify_one virtual function ptr)再notify(cv ptr) (兩者都是virtual function)
    cv->notify_one();
    Sleep(5000);
}

int main()
{
    condition_variable cv;
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);

    std::thread t1(thread1, &cv);
    cv.wait(lock);
//    cv.notify_one();

    t1.join();
}
