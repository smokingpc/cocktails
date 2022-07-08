// QueryDeviceBDF.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Common.h"

bool StrCompare(tstring str1, tstring str2, bool case_ignore = true)
{
    if (str1.size() != str2.size())
        return false;

    if (case_ignore)
    {
        tstring temp1 = str1;
        tstring temp2 = str2;
        std::transform(temp1.begin(), temp1.end(), temp1.begin(), tcslower);
        std::transform(temp2.begin(), temp2.end(), temp2.begin(), tcslower);
        if (0 != temp1.compare(temp2))
            return false;
    }
    else
    {
        if (0 != str1.compare(str2))
            return false;
    }

    return true;

}
void ParsePropertyToStringList(BYTE* data, list<tstring>& result)
{
//    case DEVPROP_TYPE_STRING_LIST:
//This function is only for StringList. it is REG_MULTI_SZ type in registry.
    wchar_t* ptr = (wchar_t*)data;
    while (ptr[0] != _T('\0'))
    {
        size_t len = _tcslen(ptr);
        if (0 == len)
            break;
#ifdef _UNICODE
        tstring data = ptr;
        result.push_back(data);
#else
        wstring data = ptr;
        string conv_data = "";
        WStrToStr(conv_data, data);
        result.push_back(data);
#endif
        ptr += (len + 1);
    }
}
void ParsePropertyToString(BYTE* data, DEVPROPTYPE type, wstring& result)
{
    switch (type)
    {
    case DEVPROP_TYPE_BOOLEAN:
    {
        result = (data[0] > 0) ? _T("TRUE") : _T("FALSE");
    }
    break;

    case DEVPROP_TYPE_SBYTE:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((char*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_BYTE:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((unsigned char*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_INT16:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((short*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_UINT16:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((unsigned short*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_INT32:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((int*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_UINT32:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%d"), ((unsigned int*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_INT64:
    case DEVPROP_TYPE_CURRENCY:     //64bit signed int
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%lld"), ((INT64*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_UINT64:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%lld"), ((UINT64*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_FLOAT:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%f"), ((float*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_DOUBLE:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, _T("%lf"), ((double*)data)[0]);
        result = buf;
    }
    break;
    case DEVPROP_TYPE_DECIMAL:      //128bit int
    case DEVPROP_TYPE_GUID:
    {
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, FORMAT_16_BYTES,
            ((wchar_t*)data)[0], ((wchar_t*)data)[1], ((wchar_t*)data)[2], ((wchar_t*)data)[3],
            ((wchar_t*)data)[4], ((wchar_t*)data)[5], ((wchar_t*)data)[6], ((wchar_t*)data)[7],
            ((wchar_t*)data)[8], ((wchar_t*)data)[9], ((wchar_t*)data)[10], ((wchar_t*)data)[11],
            ((wchar_t*)data)[12], ((wchar_t*)data)[13], ((wchar_t*)data)[14], ((wchar_t*)data)[15]);
        result = buf;
    }
    break;

    case DEVPROP_TYPE_DATE:     //DATE is OLE Automation date. 1899/12/30 0:0:0 ==> 0.00, 1900/01/01 0:0:0 is 2.00
    {
        //todo: implement this convertion
    }
    break;
    case DEVPROP_TYPE_FILETIME:
    {
        SYSTEMTIME systime = { 0 };
        FileTimeToSystemTime((FILETIME*)data, &systime);
        wchar_t buf[SMALL_BUFFER_SIZE] = { 0 };
        _stprintf_s(buf, SMALL_BUFFER_SIZE, FORMAT_SYSTEMTIME,
            systime.wYear, systime.wMonth, systime.wDay,
            systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
        result = buf;
    }
    break;

    case DEVPROP_TYPE_STRING:
    {
        result = (wchar_t*)data;
    }
    break;
    }

}
void GetDevicePropertry(HDEVINFO infoset, PSP_DEVINFO_DATA infodata,
    CONST DEVPROPKEY* key, DEVPROPTYPE type, tstring& result)
{
    BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
    BOOL ok = SetupDiGetDeviceProperty(infoset, infodata, key, &type, buffer, BIG_BUFFER_SIZE, NULL, 0);
    if (!ok)
    {
        DWORD error = GetLastError();
        //result = _T("");
        return;
    }

#ifdef _UNICODE
    ParsePropertyToString(buffer, type, result);
#else
    wstring temp = L"";
    ParsePropertyToString(buffer, type, temp);
    WStrToStr(result, temp);
#endif
}

void GetDevicePropertryStringList(HDEVINFO infoset, PSP_DEVINFO_DATA infodata,
    CONST DEVPROPKEY* key, list<tstring>& result)
{
    DEVPROPTYPE type = DEVPROP_TYPE_STRING_LIST;
    BYTE buffer[BIG_BUFFER_SIZE] = { 0 };
    BOOL ok = SetupDiGetDeviceProperty(infoset, infodata, key, &type, buffer, BIG_BUFFER_SIZE, NULL, 0);
    if (!ok)
    {
        DWORD error = GetLastError();
        return;
    }
    ParsePropertyToStringList(buffer, result);
}

BOOL ParseCmd(CMD_OPTION& option, int argc, TCHAR* argv[])
{
    if (argc < 3)
        return FALSE;

    option.Id = argv[2];

    if (0 == _tcsnicmp(argv[1], _T("hwid"), 4))
        option.Type = HWID;
    else if (0 == _tcsnicmp(argv[1], _T("defif"), 5))
        option.Type = DEV_IF;
    else if (0 == _tcsnicmp(argv[1], _T("nqn"), 3))
        option.Type = NQN;
    else
        return FALSE;
    return TRUE;
}
void Usage()
{
    _tprintf(_T("QueryDeviceBDF [option] [option arguments]\n"));
    _tprintf(_T("Options:\n"));
    _tprintf(_T("\t-devif=[device interface name]\n"));
    _tprintf(_T("\t  e.g. -devif=\\\\?\\pci#ven_8086&dev_f1a6&subsys_390b8086&rev_03#4&304661e1&0&0008#{2accfe60-c130-11d2-b082-00a0c91efb8b}\n"));
    _tprintf(_T("\t-hwid=[hardware id of device]\n"));
    _tprintf(_T("\t  e.g. -hwid=PCI\\VEN_8086&DEV_F1A6&SUBSYS_390B8086&REV_03\n"));
    _tprintf(_T("\t-nqn=[nqn of disk]\n"));
    _tprintf(_T("\t  e.g. -nqn=nqn.1994-11.com.samsung:nvme:980PRO:M.2:S5GXNF0R743809V\n"));
    _tprintf(_T("\tNotice: the -nqn option is used to query adapter BDF of NQN specified disk.\n"));
    _tprintf(_T("\t\n"));

}

void QueryDeviceByHwid(OUT DEV_INFO &result, tstring &hwid)
{
    HDEVINFO infoset;
    tstring enum_str = _T("");
    TCHAR *enumerator = NULL;
    ParseEnumeratorByHwid(enum_str, hwid);

    if (enum_str.size() > 0)
        enumerator = (TCHAR*)enum_str.c_str();

    DWORD flag = DIGCF_ALLCLASSES | DIGCF_PRESENT;
    infoset = SetupDiGetClassDevs(NULL, enumerator, NULL, flag);

    if (INVALID_HANDLE_VALUE != infoset && NULL != infoset)
    {
        DWORD id = 0;
        SP_DEVINFO_DATA infodata = { 0 };
        infodata.cbSize = sizeof(SP_DEVINFO_DATA);
        result.Hwid = _T("");
        while (SetupDiEnumDeviceInfo(infoset, id, &infodata))
        {
            id++;
            DEVPROPTYPE type = 0;
            tstring bdf_str = _T(""), found_hwid=_T("");
            //In old windows, we should use CfgMgr API to retrieve these information from registry.
            //Since VISTA, all information of device and driver are unified to [Unified Device Property Model].
            //https://docs.microsoft.com/en-us/windows-hardware/drivers/install/unified-device-property-model--windows-vista-and-later-
            type = DEVPROP_TYPE_STRING;    //not use DEVPROP_TYPE_STRING_LIST. because returned data sometimes have garbage after string.
            list<tstring> found_hwids;
            GetDevicePropertryStringList(infoset, &infodata, &DEVPKEY_Device_HardwareIds, found_hwids);
            
            //found_hwid could have multiple hwid. 
            for(tstring item : found_hwids)
            {
                if(StrCompare(item, hwid))
                {
                    result.Hwid = item;
                    break;
                }
            }
            if(result.Hwid.empty())
                continue;

            type = DEVPROP_TYPE_STRING;
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_LocationInfo, type, bdf_str);
            ParseBDF((TCHAR*) bdf_str.c_str(), result.BDF);

            type = DEVPROP_TYPE_STRING;
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_Parent, type, result.ParentId);
            break;
        }

        if(!result.Hwid.empty())
        {
            id = 0;
            while (SetupDiEnumDeviceInfo(infoset, id, &infodata))
            {
                id++;
                DEVPROPTYPE type = 0;
                tstring bdf_str = _T(""), found_id = _T("");
                //In old windows, we should use CfgMgr API to retrieve these information from registry.
                //Since VISTA, all information of device and driver are unified to [Unified Device Property Model].
                //https://docs.microsoft.com/en-us/windows-hardware/drivers/install/unified-device-property-model--windows-vista-and-later-
                type = DEVPROP_TYPE_STRING;
                GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_InstanceId, type, found_id);

                if (!StrCompare(found_id, result.ParentId))
                    continue;

                type = DEVPROP_TYPE_STRING;
                GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_LocationInfo, type, bdf_str);
                ParseBDF((TCHAR*)bdf_str.c_str(), result.ParentBDF);
            }
        }
        SetupDiDestroyDeviceInfoList(infoset);
    }
    else
        _tprintf(_T("SetupDiGetClassDevs() failed, LastError=%d\n"), GetLastError());
}
void QueryDeviceByDevInterface(OUT DEV_INFO& result, tstring devif)
{
    HDEVINFO infoset;
    tstring enum_str = _T("");
    TCHAR* enumerator = NULL;

    if (enum_str.size() > 0)
        enumerator = (TCHAR*)enum_str.c_str();

    DWORD flag = DIGCF_DEVICEINTERFACE | DIGCF_PRESENT;
    infoset = SetupDiGetClassDevs(NULL, NULL, NULL, flag);

    if (INVALID_HANDLE_VALUE != infoset && NULL != infoset)
    {
        DWORD id = 0;
        tstring found_devif = _T("");
        SP_DEVINFO_DATA infodata = { 0 };
        infodata.cbSize = sizeof(SP_DEVINFO_DATA);
        while (SetupDiEnumDeviceInfo(infoset, id, &infodata))
        {
            id++;
            DEVPROPTYPE type = 0;
            tstring bdf_str = _T(""), found_hwid = _T("");
            
            PSP_DEVICE_INTERFACE_DETAIL_DATA ifdetail = NULL;
            SP_DEVICE_INTERFACE_DATA ifdata = {0};
            BYTE buffer[BIG_BUFFER_SIZE] = {0};
            ifdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if (!SetupDiEnumDeviceInterfaces(infoset, &infodata, &infodata.ClassGuid, 0, &ifdata))
                continue;
            
            ifdetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) buffer;
            ifdetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            if (!SetupDiGetDeviceInterfaceDetail(
                  infoset, &ifdata, ifdetail, BIG_BUFFER_SIZE,NULL, NULL))
                continue;
            
            found_devif = ifdetail->DevicePath;
            if(found_devif != devif)
                continue;


            //In old windows, we should use CfgMgr API to retrieve these information from registry.
            //Since VISTA, all information of device and driver are unified to [Unified Device Property Model].
            //https://docs.microsoft.com/en-us/windows-hardware/drivers/install/unified-device-property-model--windows-vista-and-later-
            type = DEVPROP_TYPE_STRING;    //not use DEVPROP_TYPE_STRING_LIST. because returned data sometimes have garbage after string.
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_HardwareIds, type, result.Hwid);

            type = DEVPROP_TYPE_STRING;
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_LocationInfo, type, bdf_str);
            ParseBDF((TCHAR*)bdf_str.c_str(), result.BDF);

            type = DEVPROP_TYPE_STRING;
            GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_Parent, type, result.ParentId);
            break;
        }

        if (!result.Hwid.empty())
        {
            id = 0;
            while (SetupDiEnumDeviceInfo(infoset, id, &infodata))
            {
                id++;
                DEVPROPTYPE type = 0;
                tstring bdf_str = _T(""), found_id = _T("");
                //In old windows, we should use CfgMgr API to retrieve these information from registry.
                //Since VISTA, all information of device and driver are unified to [Unified Device Property Model].
                //https://docs.microsoft.com/en-us/windows-hardware/drivers/install/unified-device-property-model--windows-vista-and-later-
                type = DEVPROP_TYPE_STRING;    //not use DEVPROP_TYPE_STRING_LIST. because returned data sometimes have garbage after string.
                GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_InstanceId, type, found_id);

                if (found_id != result.ParentId)
                    continue;

                type = DEVPROP_TYPE_STRING;
                GetDevicePropertry(infoset, &infodata, &DEVPKEY_Device_LocationInfo, type, bdf_str);
                ParseBDF((TCHAR*)bdf_str.c_str(), result.ParentBDF);
            }
        }
        SetupDiDestroyDeviceInfoList(infoset);
    }
    else
        _tprintf(_T("SetupDiGetClassDevs() failed, LastError=%d\n"), GetLastError());
}
void QueryDeviceByNqn(OUT DEV_INFO& result, tstring nqn)
{}

void PrintResult(DEV_INFO result, tstring id, ID_TYPE type)
{
    _tprintf(_T("Device [%s] ID type = %d\n"), id.c_str(), type);
    _tprintf(_T("  Device Hwid = %s\n"), result.Hwid.c_str());
    _tprintf(_T("  device BDF(HEX) = %02X:%02X.%02X\n"), result.BDF.Bus, result.BDF.Dev, result.BDF.Func);
    _tprintf(_T("  parent DevicePath = %s\n"), result.ParentId.c_str());
    _tprintf(_T("  parent BDF(HEX) = %02X:%02X.%02X\n"), result.ParentBDF.Bus, result.ParentBDF.Dev, result.ParentBDF.Func);
    _tprintf(_T("\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
    CMD_OPTION option;
    if(!ParseCmd(option, argc, argv))
    {
        Usage();
        return -1;
    }
    
    DEV_INFO result;
    switch(option.Type)
    {
    case HWID:
        QueryDeviceByHwid(result, option.Id);
        break;
    case DEV_IF:
        QueryDeviceByDevInterface(result, option.Id);
        break;
    case NQN:
        QueryDeviceByNqn(result, option.Id);
        break;
    }

    PrintResult(result, option.Id, option.Type);

    return 0;
}

