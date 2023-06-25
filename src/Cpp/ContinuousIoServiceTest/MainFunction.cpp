// ContinuousIoServiceTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// [SmokingPC]
// This service will continuously write data to specified file.
// Write frequency is 64BYTEs per 100 milliseconds
// it is used to test some special scenario, e.g. disk service shutdown sequence.
// [Service Arguments]
// This service need one arg: the filepath you want write I/O.
// If this arg is not assigned, service will fail to start up.

#include "CommonHeader.h"

//bool ValidateArgs(int argc, TCHAR* argv[])
//{
//    if(argc < 2)
//        return false;
//
//    return true;
//}

int _tmain(int argc, TCHAR* argv[]) 
{
    //if(!ValidateArgs(argc, argv))
    //{
    //    return -1;
    //}

    SERVICE_TABLE_ENTRY dispatch[] = {
        { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain },
        { NULL, NULL }
    };
    StartServiceCtrlDispatcher(dispatch);

    return 0;
}
