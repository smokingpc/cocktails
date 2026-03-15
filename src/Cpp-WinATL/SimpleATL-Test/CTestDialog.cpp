#include <atlbase.h>
#include <atlapp.h>
#include <atlgdi.h>
//#include <atlcrack.h>
//#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlwin.h>
#include <atlmisc.h>
#include <comutil.h>
#include <atlddx.h>

#include "resource.h"
#include <windows.h>
#include "CTestDialog.h"

#include <string>

const WCHAR *data = LR"({\rtf1 Abacus.AI has been identified as an unapproved/unsanctioned AI application and has been blocked.  For valid business needs, please use {\field{\*\fldinst {HYPERLINK "https://wwt.sharepoint.com/sites/AICC/SitePages/WWT-Compliance-and-Acceptable-Use-Guidelines-for-Supported-AI-Tools.aspx"}}{\fldrslt{approved AI Tools}}}.\line \line If you need this application, please submit a {\field{\*\fldinst {HYPERLINK "https://servicenow.wwt.com/esc?id=sc_cat_item&sys_id=d8c9209a0a0a3c1f01f8e1bbbd667f16"}}{\fldrslt{Non-Standard Software Request}}}.})";
const CHAR* data2 = R"(Abacus.AI has been identified as an unapproved/unsanctioned AI application and has been blocked.  For valid business needs, please use {\field{\*\fldinst {HYPERLINK "https://wwt.sharepoint.com/sites/AICC/SitePages/WWT-Compliance-and-Acceptable-Use-Guidelines-for-Supported-AI-Tools.aspx"}}{\fldrslt{approved AI Tools}}}.\line \line If you need this application, please submit a {\field{\*\fldinst {HYPERLINK "https://servicenow.wwt.com/esc?id=sc_cat_item&sys_id=d8c9209a0a0a3c1f01f8e1bbbd667f16"}}{\fldrslt{Non-Standard Software Request}}}.)";

LRESULT CTestDialog::OnInitDialog(UINT msg, WPARAM wparam, LPARAM lparam, BOOL& handled) {
    //HWND window = ::GetDlgItem(m_hWnd, IDC_CUSTOM1);
    CRichEditCtrl edit(::GetDlgItem(this->m_hWnd, IDC_CUSTOM1));
    SETTEXTEX info = { 0 };

    std::string data3 = "abcdefg";

    info.codepage = CP_UTF8;
    int check = strlen(data2);
    int size = MultiByteToWideChar(CP_UTF8, 0, data2, -1, NULL, 0);
    printf("need wstr cch=%d", size);
    std::wstring wstr_temp(size, 0);
    std::wstring wstr = data;
    std::wstring wstr2 = LR"(})";
    //std::wstring wstr = data;
    wstr = wstr + wstr2;


    size = MultiByteToWideChar(CP_UTF8, 0, data2, strlen(data2), &wstr[0], size);
    printf("converted wstr cch=%d", size);

    //wstr = std::wstring(LR"({\rtf1 )") + wstr + std::wstring(LR"(})");
    wstr = std::wstring(LR"({\rtf1 )") + wstr;
    //wstr = wstr + std::wstring(LR"(})");

    size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, 0, 0, 0, 0);
    printf("need str cch=%d", size);
    
    BOOL used_default = FALSE;
    std::string str(size, 0);
    size = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], -1, &str[0], size, "~", &used_default);
    printf("converted str cch=%d", size);

    edit.SetTextEx(&info, (LPCTSTR) str.c_str());
    int data = edit.GetLineCount();
    CenterWindow();
    return 0;
}

