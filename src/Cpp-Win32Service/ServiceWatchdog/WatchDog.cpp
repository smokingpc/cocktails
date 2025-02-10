#include "CommonInclude.h"

bool RestartServiceIfNotExist(LPTSTR svcname)
{
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);

    //todo: log error. 
    if(NULL == scm)
        return false;

    SC_HANDLE svc = OpenService(scm, svcname, 
        SERVICE_START | SERVICE_QUERY_STATUS | SERVICE_PAUSE_CONTINUE);
    if (NULL != scm)
    {
        while (TRUE)
        {
            SERVICE_STATUS status = {0};
            if(QueryServiceStatus(svc, &status))
            {
                switch(status.dwCurrentState)
                {
                case SERVICE_STOP_PENDING:      //wait it stop. skip this round and check it later.
                case SERVICE_START_PENDING:     //it is going to start, do nothing.
                case SERVICE_CONTINUE_PENDING:  //it is resumed, going to start.
                case SERVICE_PAUSE_PENDING:     //it is going to pause, skip this round and check it later.
                case SERVICE_RUNNING:   //target is still alive, nothing to do.
                    break;
                case SERVICE_STOPPED:   //it is gone, recall it back.
                    StartService(svc, 0, NULL);
                    break;
                case SERVICE_PAUSED:
                    ControlService(svc, SERVICE_CONTROL_CONTINUE, &status);
                    break;
                }
            }
            else
            {
                //todo: log error. 
            }
            Sleep(CHECK_INTERVAL);
        }
    }
    else
    {
        //todo: log error. 
    }

    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return true;
}
