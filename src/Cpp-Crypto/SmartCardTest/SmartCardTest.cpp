// SmartCardTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <string>
#include <Windows.h>
#include <winscard.h>   // winscard.lib
#include <ncrypt.h>     // ncrypt.lib
#include <wincrypt.h>   // crypt32.lib

#pragma comment(lib, "winscard.lib")
#pragma comment(lib, "ncrypt.lib")
#pragma comment(lib, "crypt32.lib")

#define BIG_BUFFER_SIZE     4096
#define SMALL_BUFFER_SIZE   1024
using namespace std;

using tstring = std::basic_string<TCHAR>;

typedef struct _SCARD_READER_INFO {
    tstring Name;
    DWORD CurrentState; // current state of reader at time of call
    DWORD EventState;   // state of reader after state change
    DWORD cbAtr;          // Number of bytes in the returned ATR.
    BYTE rgbAtr[36];     // Atr of inserted card, (extra alignment bytes)
}SCARD_READER_INFO, *PSCARD_READER_INFO;

typedef struct _SCARD_INFO {
    PSCARD_READER_INFO Reader;
    tstring KSPName;

}SCARD_INFO, * PSCARD_INFO;


inline bool IsCardStateChanged(PSCARD_READERSTATE sc_state) {
    return (SCARD_STATE_CHANGED == (sc_state->dwEventState & SCARD_STATE_CHANGED));
}
inline bool IsCardExist(PSCARD_READERSTATE sc_state) {
    return (SCARD_STATE_PRESENT == (sc_state->dwEventState & SCARD_STATE_PRESENT));
}
inline bool IsCardInserted(PSCARD_READERSTATE sc_state) {
    return (IsCardStateChanged(sc_state) && IsCardExist(sc_state));
}
inline bool IsCardRemoved(PSCARD_READERSTATE sc_state) {
    return (IsCardStateChanged(sc_state) && !IsCardExist(sc_state));
    //(TRUE == (sc_state->dwEventState & SCARD_STATE_EMPTY))
}

void GetCurrentSmartCardState(SCARDCONTEXT cardctx, LPSCARD_READERSTATE sc_states, DWORD reader_count) {
    LONG sc_ret = SCardGetStatusChange(cardctx, INFINITE, sc_states, reader_count);
    if (SCARD_S_SUCCESS == sc_ret) {
        for (int i = 0; i < reader_count; i++) {
            PSCARD_READERSTATE reader_state = sc_states + i;
            if (IsCardExist(reader_state)) {
                _tprintf(_T("[Current State] Smartcard currently in reader\n"));
            }
            else {
                _tprintf(_T("[Current State] Reader is empty, smartcard is not there\n"));
            }
        }
    }
}

void GetCardTypeProviderName(SCARDCONTEXT cardctx, SCARD_READERSTATE* state, wstring& provider_name) {
    WCHAR card_name[SMALL_BUFFER_SIZE] = { 0 };
    DWORD  card_name_len = SMALL_BUFFER_SIZE;

    LONG rc = SCardListCardsW(cardctx, state->rgbAtr, nullptr, 0, reinterpret_cast<LPWSTR>(&card_name), &card_name_len);
    if (rc == SCARD_S_SUCCESS && card_name && *card_name) {
        // Card name -> KSP name ("Smart Card Key Storage Provider" value)
        wchar_t ksp[SMALL_BUFFER_SIZE] = { 0 };
        DWORD   cchKsp = _countof(ksp);
        rc = SCardGetCardTypeProviderNameW(cardctx, card_name, SCARD_PROVIDER_KSP, ksp, &cchKsp);
        if (rc == SCARD_S_SUCCESS && ksp[0]) {
            provider_name = ksp;
            if (provider_name.empty()) {
                _tprintf(_T("KSP name is empty, use default name: [%s]\n"), MS_SMART_CARD_KEY_STORAGE_PROVIDER);
                provider_name = MS_SMART_CARD_KEY_STORAGE_PROVIDER; //using default KSP name if can't retrieve it.
            }
        }
    }
}

bool FindFirstReaderWithCard(
    SCARDCONTEXT cardctx, 
    LPSCARD_READERSTATE sc_states, 
    DWORD reader_count, 
    PSCARD_READER_INFO found_reader, 
    PSCARD_INFO found_card) 
{
    LONG sc_ret = SCardGetStatusChange(cardctx, INFINITE, sc_states, reader_count);
    if (SCARD_S_SUCCESS == sc_ret) {
        for (int i = 0; i < reader_count; i++) {
            PSCARD_READERSTATE state = sc_states + i;
            if (IsCardExist(state)) {
                found_reader->Name = state->szReader;
                found_reader->cbAtr = state->cbAtr;
                found_reader->CurrentState = state->dwCurrentState;
                found_reader->EventState = state->dwEventState;
                memcpy(found_reader->rgbAtr, state->rgbAtr, state->cbAtr);
                GetCardTypeProviderName(cardctx, state, found_card->KSPName);
                found_card->Reader = found_reader;
                return true;
            }
        }
    }
    _tprintf(_T("[Current State] No cards in all Readers....\n"));
    return false;
}

void TriggerInputPIN(wstring &reader_name, wstring &provider_name, bool custom_scope) {
    NCRYPT_PROV_HANDLE hCryptProvider = NULL;
    SECURITY_STATUS sec_status = ERROR_SUCCESS;
    wstring scope_str = wstring(_T("\\\\.\\")) + reader_name.c_str() + wstring(_T("\\"));
    LPCWSTR scope = (custom_scope && !scope_str.empty()) ? scope_str.c_str() : NULL;
    NCryptKeyName* keyname = NULL;
    NCRYPT_KEY_HANDLE hKey = NULL;
    HWND hDesktop = GetDesktopWindow();
    PVOID enum_state = NULL;
    BCRYPT_PKCS1_PADDING_INFO padinfo = {0};
    BYTE data[32] = { 0 };
    BYTE sign[256] = { 0 };
    DWORD hash_ret = 0;
    BCRYPT_ALG_HANDLE bcrypt_alg = NULL;
    int key_idx = -1;
    DWORD opt = NCRYPT_PIN_CACHE_CLEAR_FOR_CALLING_PROCESS_OPTION;

    _tprintf(_T("TriggerInputPIN() ==> IN\n"));

    //BOOL ok = CryptAcquireContext(&hCSP, NULL, MS_SCARD_PROV, PROV_RSA_FULL, CRYPT_DEFAULT_CONTAINER_OPTIONAL);
    //if (!ok) {
    //    _tprintf(_T("CryptAcquireContext() failed, error = 0x%08X\n"), GetLastError());
    //    goto EXIT;
    //}

    sec_status = NCryptOpenStorageProvider(&hCryptProvider, provider_name.c_str(), 0);
    if (!SUCCEEDED(sec_status)) {
        _tprintf(_T("NCryptOpenStorageProvider() failed, error = 0x%08X\n"), sec_status);
        goto EXIT;
    }

    sec_status = NCryptSetProperty(hCryptProvider, NCRYPT_WINDOW_HANDLE_PROPERTY, (PBYTE)&hDesktop, sizeof(hDesktop), 0);
    if (!SUCCEEDED(sec_status) && NTE_NOT_SUPPORTED != sec_status) {
        _tprintf(_T("NCryptSetProperty(NCRYPT_WINDOW_HANDLE_PROPERTY) failed, error = 0x%08X\n"), sec_status);
        goto EXIT;
    }
    
    do {
        sec_status = NCryptEnumKeys(hCryptProvider, scope, &keyname, &enum_state, 0);
        if (!SUCCEEDED(sec_status)) {
            _tprintf(_T("NCryptEnumKeys() failed, error = 0x%08X\n"), sec_status);
            break;
        }
        key_idx++;
        sec_status = NCryptOpenKey(hCryptProvider, &hKey, keyname->pszName, 0, 0);
        NCryptFreeBuffer(keyname);
        if (hKey) {
            //use encryption to enforce OS popup PIN input dialog
            data[0] = 'a';
            padinfo.pszAlgId = BCRYPT_SHA256_ALGORITHM;
            sec_status = NCryptSignHash(hKey, &padinfo, data, _countof(data),
                                            sign, _countof(sign), &hash_ret, BCRYPT_PAD_PKCS1);
            _tprintf(_T("NCryptSignHash() RESULT=0x%08X\n"), sec_status);
            NCryptFreeObject(hKey);
        }
    } while (SUCCEEDED(sec_status));

    if (NULL != enum_state) {
        NCryptFreeBuffer(enum_state);
    }

EXIT:
    if (NULL != hCryptProvider) {
        NCryptFreeObject(hCryptProvider);
        hCryptProvider = NULL;
    }

    //if (NULL != hCSP) {
    //    CryptReleaseContext(hCSP, 0);
    //    hCSP = NULL;
    //}

    _tprintf(_T("TriggerInputPIN() <== OUT\n"));
}

//not working....
void ResetCardPinCache(SCARDCONTEXT cardctx, tstring &reader_name) {
    SCARDHANDLE  hCard = 0;
    DWORD        proto = 0;
    LONG rc = SCardConnect(cardctx, reader_name.c_str(), SCARD_SHARE_SHARED,
        SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &hCard, &proto);
    if (rc == SCARD_S_SUCCESS) {
        SCardDisconnect(hCard, SCARD_LEAVE_CARD);   // SCARD_UNPOWER_CARD is cold reset, SCARD_RESET_CARD is warm reset.
    }
    else {
        _tprintf(_T("SCardConnect() failed. error=0x%08X\n"), rc);
    }
}

int Test_CHT_GPKI_Card(bool reset_card_conn) {
    SCARDCONTEXT cardctx = NULL;
    SCARD_READERSTATE   sc_states[MAXIMUM_SMARTCARD_READERS] = { 0 };
    TCHAR reader_names[BIG_BUFFER_SIZE] = { 0 };   //multistring
    DWORD reader_names_size = BIG_BUFFER_SIZE;    //in count of chars
    DWORD reader_count = 0;
    DWORD reader_idx_with_card = 0;
    LONG sc_ret = 0;

    sc_ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &cardctx);
    if (SCARD_S_SUCCESS != sc_ret) {
        _tprintf(_T("SCardEstablishContext() failed, error=%X\n"), sc_ret);
        return -1;
    }

    sc_ret = SCardListReaders(cardctx, NULL, reader_names, &reader_names_size);
    if (SCARD_S_SUCCESS == sc_ret) {
        TCHAR* name = reader_names;
        while (_tcslen(name) > 0 && reader_count <= MAXIMUM_SMARTCARD_READERS) {
            _tprintf(_T("Found card reader[%d] = %s\n"), reader_count, name);
            sc_states[reader_count].szReader = name;
            sc_states[reader_count].dwCurrentState = SCARD_STATE_UNAWARE;
            reader_count++;
            name = name + _tcslen(name) + 1;    //iterate next string
        }
        _tprintf(_T("Total card readers=%d\n"), reader_count);
        if (reader_count > 0) {
            GetCurrentSmartCardState(cardctx, sc_states, reader_count);
            SCARD_READER_INFO sc_reader;
            SCARD_INFO scard;
            FindFirstReaderWithCard(cardctx, sc_states, reader_count, &sc_reader, &scard);

            TriggerInputPIN(sc_reader.Name, scard.KSPName, true);
            Sleep(10 * 1000);
            if (reset_card_conn) {
                ResetCardPinCache(cardctx, sc_reader.Name);
            }

            TriggerInputPIN(sc_reader.Name, scard.KSPName, true);
        }
        else {
            _tprintf(_T("No card readers found\n"));
        }
    }
    else {
        _tprintf(_T("SCardListReaders() failed, error=%X\n"), sc_ret);
    }

    sc_ret = SCardReleaseContext(cardctx);
    if (SCARD_S_SUCCESS != sc_ret) {
        _tprintf(_T("SCardReleaseContext() failed, error=%X\n"), sc_ret);
    }
}

int Test_Generic_Card(bool reset_card_conn) {
    SCARDCONTEXT cardctx = NULL;
    SCARD_READERSTATE   sc_states[MAXIMUM_SMARTCARD_READERS] = { 0 };
    TCHAR reader_names[BIG_BUFFER_SIZE] = { 0 };   //multistring
    DWORD reader_names_size = BIG_BUFFER_SIZE;    //in count of chars
    DWORD reader_count = 0;
    DWORD reader_idx_with_card = 0;
    LONG sc_ret = 0;

    sc_ret = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, &cardctx);
    if (SCARD_S_SUCCESS != sc_ret) {
        _tprintf(_T("SCardEstablishContext() failed, error=%X\n"), sc_ret);
        return -1;
    }

    sc_ret = SCardListReaders(cardctx, NULL, reader_names, &reader_names_size);
    if (SCARD_S_SUCCESS == sc_ret) {
        TCHAR* name = reader_names;
        while (_tcslen(name) > 0 && reader_count <= MAXIMUM_SMARTCARD_READERS) {
            _tprintf(_T("Found card reader[%d] = %s\n"), reader_count, name);
            sc_states[reader_count].szReader = name;
            sc_states[reader_count].dwCurrentState = SCARD_STATE_UNAWARE;
            reader_count++;
            name = name + _tcslen(name) + 1;    //iterate next string
        }
        _tprintf(_T("Total card readers=%d\n"), reader_count);
        if (reader_count > 0) {
            GetCurrentSmartCardState(cardctx, sc_states, reader_count);
            SCARD_READER_INFO sc_reader;
            SCARD_INFO scard;
            FindFirstReaderWithCard(cardctx, sc_states, reader_count, &sc_reader, &scard);
            TriggerInputPIN(sc_reader.Name, scard.KSPName, false);

            Sleep(10 * 1000);
            if (reset_card_conn) {
                ResetCardPinCache(cardctx, sc_reader.Name);
            }

            TriggerInputPIN(sc_reader.Name, scard.KSPName, false);
        }
        else {
            _tprintf(_T("No card readers found\n"));
        }
    }
    else {
        _tprintf(_T("SCardListReaders() failed, error=%X\n"), sc_ret);
    }

    sc_ret = SCardReleaseContext(cardctx);
    if (SCARD_S_SUCCESS != sc_ret) {
        _tprintf(_T("SCardReleaseContext() failed, error=%X\n"), sc_ret);
    }
}

int _tmain(int argc, TCHAR* argv[])
{
    _tprintf(_T("Usage: SmartCardTest.exe <%Type>\n"));
    _tprintf(_T("  Type == 1 or 2. 1 is Generic Smart Card. 2 is CHT_GPKI_Card.\n"));
    _tprintf(_T("  If type is not specified, using 1 as default.\n"));
    _tprintf(_T("\n"));

    if (1 == argc) {
        return Test_Generic_Card(true);
    }
    else {
        if (0 == _tcsicmp(argv[1], _T("1"))) 
            return Test_Generic_Card(true);
        else
            return Test_CHT_GPKI_Card(true);
    }
}
