#pragma once

using namespace ATL;

class CTestDialog : public CDialogImpl<CTestDialog>
{
public:
    CTestDialog() {}
    ~CTestDialog() {}
    enum { IDD = IDD_TEST_DLG };

    LRESULT OnInitDialog(UINT msg, WPARAM wparam, LPARAM lparam, BOOL& handled);
    LRESULT onOKCancel(WORD, WORD wID, HWND, BOOL&)
    {
        // This method overrides IDCANCEL message to stop dialog box getting closed on ESCAPE key press.
        EndDialog(wID);
        return LRESULT();
    }

    BEGIN_MSG_MAP(CTestDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDCANCEL, onOKCancel)
        COMMAND_ID_HANDLER(IDOK, onOKCancel)
    END_MSG_MAP()
};

