#include "CommonInclude.h"

DWORD SvcInstall()
{
    SC_HANDLE scm = NULL;
    SC_HANDLE service = NULL;
    TCHAR exe_path[MAX_PATH] = { 0 };
    DWORD error = ERROR_SUCCESS;

    if (!GetModuleFileName(NULL, exe_path, MAX_PATH))
    {
        error = GetLastError();
        printf("Cannot install service (%d)\n", error);
        return error;
    }

    // In case the path contains a space, it must be quoted so that
    // it is correctly interpreted. For example,
    // "d:\my share\myservice.exe" should be specified as
    // ""d:\my share\myservice.exe"".
    // **This is because a file path need double quote to escape space in path string
    // e.g. c:\program files\my service\service.exe  => we should double quote it to "c:\program files\my service\service.exe"
    TCHAR svc_path[MAX_PATH] = { 0 };
    StringCbPrintf(svc_path, MAX_PATH, TEXT("\"%s\""), exe_path);

    // Get a handle to the SCM database. 

    scm = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == scm)
    {
        error = GetLastError();
        printf("OpenSCManager failed (%d)\n", error);
        return error;
    }

    // Create the service
    service = CreateService(
        scm,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type 
        SERVICE_ERROR_NORMAL,      // error control type 
        svc_path,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (service == NULL)
    {
        printf("CreateService failed (%d)\n", error);
        CloseServiceHandle(scm);
        return error;
    }
    else
        printf("Service installed successfully\n");

    CloseServiceHandle(service);
    CloseServiceHandle(scm);

    return ERROR_SUCCESS;
}
DWORD SvcUninstall()
{
    SC_HANDLE scm = NULL;
    SC_HANDLE service = NULL;
    DWORD error = ERROR_SUCCESS;

    // Get a handle to the SCM database. 
    scm = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == scm)
    {
        error = GetLastError();
        printf("OpenSCManager failed (%d)\n", error);
        return error;
    }

    //open service then delete it.
    service = OpenService(scm, SVCNAME, SERVICE_ALL_ACCESS);
    if (service == NULL)
    {
        error = GetLastError();
        printf("OpenService failed (%d)\n", error);
        CloseServiceHandle(scm);
        return error;
    }

    if(!DeleteService(service))
    {
        error = GetLastError();
        printf("DeleteService failed (%d)\n", error);
    }
    else
        printf("Service deleted successfully\n");

    CloseServiceHandle(service);
    CloseServiceHandle(scm);
    return ERROR_SUCCESS;
}

/// <param name="name">name of value</param>
/// <param name="type">data type in registry, e.g. REG_DWORD or REG_SZ</param>
/// <param name="data">actual data you want to write into registry</param>
/// <param name="data_bytes">sizeof(data) , in bytes</param>
/// <returns> LRESULT of registry APIs </returns>
DWORD WriteToServiceRegistry(LPTSTR name, DWORD type, PVOID data, DWORD data_bytes)
{
    HKEY key = NULL;
    LSTATUS status = RegOpenKey(HKEY_LOCAL_MACHINE, SVC_REGKEY, &key);
    if(status != ERROR_SUCCESS)
        return status;

    status = RegSetValueEx(key, name, 0, type, (const BYTE*)data, data_bytes);
    if (status != ERROR_SUCCESS)
    {
        //todo: log error
    }

    RegCloseKey(key);
    return status;
}

/// <param name="name">name of value</param>
/// <param name="data">actual data you want to write into registry</param>
/// <param name="data_bytes">sizeof(data) , in bytes</param>
/// <param name="type">data type in registry, e.g. REG_DWORD or REG_SZ</param>
/// <returns> LRESULT of registry APIs </returns>
DWORD ReadFromServiceRegistry(LPTSTR name, PVOID data, DWORD *data_bytes, DWORD *type)
{
    HKEY key = NULL;
    LSTATUS status = RegOpenKey(HKEY_LOCAL_MACHINE, SVC_REGKEY, &key);
    if (status != ERROR_SUCCESS)
        return status;

    status = RegQueryValueEx(key, name, 0, type, (LPBYTE)data, data_bytes);
    if (status != ERROR_SUCCESS)
    {
        //todo: log error
    }

    RegCloseKey(key);
    return status;
}
