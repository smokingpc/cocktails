#include "Common.h"

typedef enum _OPAL_UID {
    // users
    OPAL_SMUID_UID,
    OPAL_THISSP_UID,
    OPAL_ADMINSP_UID,
    OPAL_LOCKINGSP_UID,
    ENTERPRISE_LOCKINGSP_UID,
    OPAL_ANYBODY_UID,
    OPAL_SID_UID,
    OPAL_ADMIN1_UID,
    OPAL_USER1_UID,
    OPAL_USER2_UID,
    OPAL_PSID_UID,
    ENTERPRISE_BANDMASTER0_UID,
    ENTERPRISE_ERASEMASTER_UID,
    // tables
    OPAL_LOCKINGRANGE_GLOBAL,
    OPAL_LOCKINGRANGE_ACE_RDLOCKED,
    OPAL_LOCKINGRANGE_ACE_WRLOCKED,
    OPAL_LOCKINGRANGE_GLOBAL_ACE_RDLOCKED,
    OPAL_LOCKINGRANGE_GLOBAL_ACE_WRLOCKED,
    OPAL_MBRControl_Set_DoneToDOR,
    OPAL_MBRCONTROL,
    OPAL_MBR,
    OPAL_AUTHORITY_TABLE,
    OPAL_C_PIN_TABLE,
    OPAL_LOCKING_INFO_TABLE,
    ENTERPRISE_LOCKING_INFO_TABLE,
    //C_PIN_TABLE object ID's
    OPAL_C_PIN_MSID,
    OPAL_C_PIN_SID,
    OPAL_C_PIN_ADMIN1,
    //half UID's (only first 4 bytes used)
    OPAL_HALF_UID_AUTHORITY_OBJ_REF,
    OPAL_HALF_UID_BOOLEAN_ACE,
    // omitted optional parameter
    OPAL_UID_HEXFF,
} OPAL_UID;

static const UINT8 OPALUID[][8]{
    // users
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff}, //< session management 
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }, //< special "thisSP" syntax
    { 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x01 }, //< Administrative SP
    { 0x00, 0x00, 0x02, 0x05, 0x00, 0x00, 0x00, 0x02 }, //< Locking SP
    { 0x00, 0x00, 0x02, 0x05, 0x00, 0x01, 0x00, 0x01 }, //< ENTERPRISE Locking SP 
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01 }, //<anybody
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x06 }, //< SID
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0x00, 0x01 }, //< ADMIN1
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x00, 0x01 }, //< USER1
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x03, 0x00, 0x02 }, //< USER2
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x01, 0xff, 0x01 }, //< PSID user
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x80, 0x01 }, //< BandMaster 0
    { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x84, 0x01 }, //< EraseMaster
    // tables
    { 0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0x00, 0x01 }, //< Locking_GlobalRange
    { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE0, 0x01 }, //< ACE_Locking_Range_Set_RdLocked UID
    { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE8, 0x01 }, //< ACE_Locking_Range_Set_WrLocked UID
    { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE0, 0x00 }, //< ACE_Locking_Range_GLOBAL_Set_RdLocked UID
    { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xE8, 0x00 }, //< ACE_Locking_Range_GLOBAL_Set_WrLocked UID
    { 0x00, 0x00, 0x00, 0x08, 0x00, 0x03, 0xF8, 0x01 }, //< ACE_MBRControl_Set_DoneToDOR
    { 0x00, 0x00, 0x08, 0x03, 0x00, 0x00, 0x00, 0x01 }, //< MBR Control
        { 0x00, 0x00, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00 }, //< Shadow MBR
        { 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00}, //< AUTHORITY_TABLE
        { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00}, //< C_PIN_TABLE
        { 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x01 }, //< OPAL Locking Info
        { 0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00 }, //< Enterprise Locking Info
        //C_PIN_TABLE object ID's
        { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x84, 0x02}, //< C_PIN_MSID
        { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x01}, //< C_PIN_SID
        { 0x00, 0x00, 0x00, 0x0B, 0x00, 0x01, 0x00, 0x01}, //< C_PIN_ADMIN1
        //half UID's (only first 4 bytes used)
        { 0x00, 0x00, 0x0C, 0x05, 0xff, 0xff, 0xff, 0xff }, // Half-UID - Authority_object_ref
        { 0x00, 0x00, 0x04, 0x0E, 0xff, 0xff, 0xff, 0xff }, // Half-UID - Boolean ACE
        // special value for omitted optional parameter
        { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, //< HEXFF for omitted
};

static const uint8_t OPALMETHOD[][8]{
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01}, /**< Properties */
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x02}, /**<STARTSESSION */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x02}, /**< Revert */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x02, 0x03}, /**< Activate */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x06 }, /**< Enterprise Get */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07 }, /**< Enterprise Set */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x08}, /**< NEXT */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0c }, /**< Enterprise Authenticate */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x0d}, /**< GetACL */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x10}, /**< GenKey */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x11}, /**< revertSP */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x16}, /**<Get */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x17}, /**< Set */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x1c}, /**< Authenticate */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x06, 0x01}, /**< Random */
    { 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x08, 0x03 }, /**< Erase */
};

typedef enum _OPAL_TOKEN {
    //Boolean
    OPAL_TRUE = 0x01,
    OPAL_FALSE = 0x00,
    OPAL_BOOLEAN_EXPR = 0x03,
    // cellblocks
    TABLE = 0x00,
    STARTROW = 0x01,
    ENDROW = 0x02,
    STARTCOLUMN = 0x03,
    ENDCOLUMN = 0x04,
    VALUES = 0x01,
    // authority table
    PIN = 0x03,
    // locking tokens
    RANGESTART = 0x03,
    RANGELENGTH = 0x04,
    READLOCKENABLED = 0x05,
    WRITELOCKENABLED = 0x06,
    READLOCKED = 0x07,
    WRITELOCKED = 0x08,
    ACTIVEKEY = 0x0A,
    //locking info table
    MAXRANGES = 0x04,
    // mbr control
    MBRENABLE = 0x01,
    MBRDONE = 0x02,
    // properties
    HOSTPROPERTIES = 0x00,
    // response tokenis() returned values
    DTA_TOKENID_BYTESTRING = 0xe0,
    DTA_TOKENID_SINT = 0xe1,
    DTA_TOKENID_UINT = 0xe2,
    DTA_TOKENID_TOKEN = 0xe3, // actual token is returned
    // atoms
    STARTLIST = 0xf0,
    ENDLIST = 0xf1,
    STARTNAME = 0xf2,
    ENDNAME = 0xf3,
    CALL = 0xf8,
    ENDOFDATA = 0xf9,
    ENDOFSESSION = 0xfa,
    STARTTRANSACTON = 0xfb,
    ENDTRANSACTON = 0xfC,
    EMPTYATOM = 0xff,
    WHERE = 0x00,
} OPAL_TOKEN;

typedef enum _OPAL_STATUSCODE {
    SUCCESS = 0x00,
    NOT_AUTHORIZED = 0x01,
    //	OBSOLETE = 0x02,
    SP_BUSY = 0x03,
    SP_FAILED = 0x04,
    SP_DISABLED = 0x05,
    SP_FROZEN = 0x06,
    NO_SESSIONS_AVAILABLE = 0x07,
    UNIQUENESS_CONFLICT = 0x08,
    INSUFFICIENT_SPACE = 0x09,
    INSUFFICIENT_ROWS = 0x0A,
    INVALID_FUNCTION = 0x0B, // defined in early specs, still used in some firmware
    INVALID_PARAMETER = 0x0C,
    INVALID_REFERENCE = 0x0D,
    //	OBSOLETE = 0x0E,
    TPER_MALFUNCTION = 0x0F,
    TRANSACTION_FAILURE = 0x10,
    RESPONSE_OVERFLOW = 0x11,
    AUTHORITY_LOCKED_OUT = 0x12,
    FAIL = 0x3f,
} OPAL_STATUSCODE;

typedef enum _OPAL_SHORT_ATOM {
    UINT_3 = 0x83,
    BYTESTRING4 = 0xa4,
    BYTESTRING8 = 0xa8,
} OPAL_SHORT_ATOM;

typedef enum _OPAL_METHOD {
    PROPERTIES,
    STARTSESSION,
    REVERT,
    ACTIVATE,
    EGET,
    ESET,
    NEXT,
    EAUTHENTICATE,
    GETACL,
    GENKEY,
    REVERTSP,
    GET,
    SET,
    AUTHENTICATE,
    RANDOM,
    ERASE,
} OPAL_METHOD;

typedef enum _OPAL_TINY_ATOM {
    UINT_00 = 0x00,
    UINT_01 = 0x01,
    UINT_02 = 0x02,
    UINT_03 = 0x03,
    UINT_04 = 0x04,
    UINT_05 = 0x05,
    UINT_06 = 0x06,
    UINT_07 = 0x07,
    UINT_08 = 0x08,
    UINT_09 = 0x09,
    UINT_10 = 0x0a,
    UINT_11 = 0x0b,
    UINT_12 = 0x0c,
    UINT_13 = 0x0d,
    UINT_14 = 0x0e,
    UINT_15 = 0x0f,
} OPAL_TINY_ATOM;


typedef struct _OPALComPacket {
    uint32_t reserved0;
    uint8_t extendedComID[4];
    uint32_t outstandingData;
    uint32_t minTransfer;
    uint32_t length;
} OPALComPacket;

/** Packet structure. */
typedef struct _OPALPacket {
    uint32_t TSN;
    uint32_t HSN;
    uint32_t seqNumber;
    uint16_t reserved0;
    uint16_t ackType;
    uint32_t acknowledgement;
    uint32_t length;
} OPALPacket;

/** Data sub packet header */
typedef struct _OPALDataSubPacket {
    uint8_t reserved0[6];
    uint16_t kind;
    uint32_t length;
} OPALDataSubPacket;

typedef struct _OPALHeader {
    OPALComPacket cp;
    OPALPacket pkt;
    OPALDataSubPacket subpkt;
} OPALHeader;

void ShowStructureSizes()
{
    _tprintf(_T("sizeof(FEATURE_DESCRIPTOR) = %lld\n"), sizeof(FEATURE_DESCRIPTOR));
    _tprintf(_T("sizeof(FEATURE_DESC_HEADER) = %lld\n"), sizeof(FEATURE_DESC_HEADER));
    _tprintf(_T("\n"));
    _tprintf(_T("sizeof(FEATURE_DESC_TPer) = %lld\n"), sizeof(FEATURE_DESC_TPer));
    _tprintf(_T("sizeof(FEATURE_DESC_LOCKING) = %lld\n"), sizeof(FEATURE_DESC_LOCKING));
    _tprintf(_T("sizeof(FEATURE_DESC_GEOMETRY) = %lld\n"), sizeof(FEATURE_DESC_GEOMETRY));
    _tprintf(_T("sizeof(FEATURE_DESC_ENTERPRISE_SSC) = %lld\n"), sizeof(FEATURE_DESC_ENTERPRISE_SSC));
    _tprintf(_T("sizeof(FEATURE_DESC_SINGLE_USER_MODE) = %lld\n"), sizeof(FEATURE_DESC_SINGLE_USER_MODE));
    _tprintf(_T("sizeof(FEATURE_DESC_OPAL_V100) = %lld\n"), sizeof(FEATURE_DESC_OPAL_V100));
    _tprintf(_T("sizeof(FEATURE_DESC_OPAL_V200) = %lld\n"), sizeof(FEATURE_DESC_OPAL_V200));
    _tprintf(_T("sizeof(FEATURE_DESC_DATASTORE) = %lld\n"), sizeof(FEATURE_DESC_DATASTORE));
}

//Discovery0 is TCG defined "device discovery layer0" behavior.
bool Discovery0_NVMe(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
{
    UCHAR protocol = 1;
    USHORT comid = 1;
    bool ok = false;
    DWORD ret_size = 0;
    HANDLE device = CreateFile(diskname.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), diskname.c_str(), GetLastError());
        return false;
    }

    PSCSI_PASS_THROUGH_DIRECT cmd = (PSCSI_PASS_THROUGH_DIRECT) new BYTE[SMALL_BUFFER_SIZE];
    RtlZeroMemory(cmd, SMALL_BUFFER_SIZE);
    cmd->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    cmd->PathId = 0;
    cmd->TargetId = 1;
    cmd->Lun = 0;
    cmd->SenseInfoLength = 32;
    cmd->DataTransferLength = buf_size;
    cmd->TimeOutValue = 5;      //in seconds
    cmd->DataBuffer = buffer;
    cmd->SenseInfoOffset = (cmd->Length + (sizeof(DWORD) - 1) / sizeof(DWORD)) * sizeof(DWORD);   //align to DWORD

    PCDB cdb = (PCDB)cmd->Cdb;
    cdb->SECURITY_PROTOCOL_IN.OperationCode = SCSIOP_SECURITY_PROTOCOL_IN;
    cdb->SECURITY_PROTOCOL_IN.SecurityProtocol = protocol;

    //SecurityProtocolSpecific and AllocationLength field are BIG-ENDIAN
    SwapEndian(&comid, (USHORT*)cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific);
    SwapEndian(&buf_size, (ULONG*)cdb->SECURITY_PROTOCOL_IN.AllocationLength);
    //cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific[0] = (comid&0xFF00)>>8;
    //cdb->SECURITY_PROTOCOL_IN.SecurityProtocolSpecific[1] = (comid & 0x00FF);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[0] = (UCHAR)((buf_size & 0xFF000000) >> 24);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[1] = (UCHAR)((buf_size & 0x00FF0000) >> 16);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[2] = (UCHAR)((buf_size & 0x0000FF00) >> 8);
    //cdb->SECURITY_PROTOCOL_IN.AllocationLength[3] = (UCHAR)((buf_size & 0x000000FF));
    cmd->CdbLength = sizeof(cdb->SECURITY_PROTOCOL_IN);
    cmd->DataIn = SCSI_IOCTL_DATA_IN;

    ok = DeviceIoControl(device,
        IOCTL_SCSI_PASS_THROUGH_DIRECT,
        cmd,
        SMALL_BUFFER_SIZE,
        cmd,
        SMALL_BUFFER_SIZE,
        &ret_size,
        FALSE);

    delete[] cmd;
    if (!ok)
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return ok;
}
bool Discovery0_SATA(IN OUT BYTE buffer[], IN ULONG buf_size, IN tstring& diskname)
{
    //UCHAR protocol = 1;
    USHORT comid = 0x0001;
    UINT8 cmd_id = 1;
    UINT8 protocol = 1;
    bool ok = false;
    DWORD ret_size = 0;
    HANDLE device = CreateFile(diskname.c_str(),
        GENERIC_WRITE | GENERIC_READ,
        FILE_SHARE_WRITE | FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (INVALID_HANDLE_VALUE == device)
    {
        _tprintf(_T("Open %s failed, error = %d\n"), diskname.c_str(), GetLastError());
        return false;
    }

    PATA_PASS_THROUGH_DIRECT cmd = new ATA_PASS_THROUGH_DIRECT();
    RtlZeroMemory(cmd, sizeof(ATA_PASS_THROUGH_DIRECT));
    cmd->Length = sizeof(ATA_PASS_THROUGH_DIRECT);

    //info query and discovery0 are ATA_FLAGS_DATA_IN
    cmd->AtaFlags = ATA_FLAGS_DATA_IN;
    cmd->DataTransferLength = buf_size;
    cmd->DataBuffer = buffer;
    cmd->TimeOutValue = 5;      //in seconds
    //Roy : I don't understand why fill such values here...
    //copied from SEDUTIL src
    cmd->CurrentTaskFile[0] = protocol; // Protocol
    cmd->CurrentTaskFile[1] = (UINT8)(buf_size / 512); // how many "sectors" does Payload have?
    SwapEndian(&comid, (UINT16*)&cmd->CurrentTaskFile[3]);
    cmd->CurrentTaskFile[6] = (UCHAR)ATACOMMAND::IF_RECV;

    ok = DeviceIoControl(device,
        IOCTL_ATA_PASS_THROUGH_DIRECT,
        cmd,
        sizeof(ATA_PASS_THROUGH_DIRECT),
        cmd,
        sizeof(ATA_PASS_THROUGH_DIRECT),
        &ret_size,
        FALSE);

    delete[] cmd;
    if (!ok)
        _tprintf(_T("DeviceIoControl failed. Error=%d\n"), GetLastError());

    if (INVALID_HANDLE_VALUE != device && NULL != device)
        CloseHandle(device);
    return ok;
}


bool StartOpalSession(IN tstring& diskname, IN string& adm1_pwd)
{
//    (OPAL_UID::OPAL_LOCKINGSP_UID, adm1_pwd.c_str(), OPAL_UID::OPAL_ADMIN1_UID)
    BYTE *cmdbuf = new BYTE[OPAL_BUFFER_SIZE];
    BYTE* respbuf = new BYTE[OPAL_BUFFER_SIZE];
    int cmd_cursor = sizeof(OPALHeader);
    RtlZeroMemory(cmdbuf, OPAL_BUFFER_SIZE);
    RtlZeroMemory(respbuf, OPAL_BUFFER_SIZE);
    
    //fill start session command bytes
    cmdbuf[cmd_cursor++] = OPAL_TOKEN::CALL;
    cmdbuf[cmd_cursor++] = OPAL_UID::OPAL_LOCKINGSP_UID;
    cmdbuf[cmd_cursor++] = OPAL_SHORT_ATOM::BYTESTRING8;
    RtlCopyMemory((cmdbuf+cmd_cursor), OPALMETHOD[OPAL_METHOD::STARTSESSION], 8);
    cmd_cursor+=8;

    cmdbuf[cmd_cursor++] = OPAL_TOKEN::STARTLIST;
    cmdbuf[cmd_cursor++] = 105;
    cmdbuf[cmd_cursor++] = OPAL_UID::OPAL_LOCKINGSP_UID;
    cmdbuf[cmd_cursor++] = OPAL_TINY_ATOM::UINT_01; // "write" cmd
    cmdbuf[cmd_cursor++] = OPAL_TOKEN::STARTNAME;
    cmdbuf[cmd_cursor++] = OPAL_TINY_ATOM::UINT_00;

    //todo: hash pwd

    cmdbuf[cmd_cursor++] = OPAL_TOKEN::ENDNAME;
    cmdbuf[cmd_cursor++] = OPAL_TOKEN::STARTNAME;
    cmdbuf[cmd_cursor++] = OPAL_TINY_ATOM::UINT_03;

    RtlCopyMemory((cmdbuf + cmd_cursor), OPALUID[OPAL_UID::OPAL_ADMIN1_UID], 8);
    cmd_cursor += 8;
    cmdbuf[cmd_cursor++] = OPAL_TOKEN::ENDNAME;

    //if (d->isEprise()) {
    //    cmd->addToken(OPAL_TOKEN::STARTNAME);
    //    cmd->addToken("SessionTimeout");
    //    cmd->addToken(60000);
    //    cmd->addToken(OPAL_TOKEN::ENDNAME);
    //}

    cmdbuf[cmd_cursor++] = OPAL_TOKEN::ENDLIST;
    //send command

    delete[] cmdbuf;
    delete[] respbuf;
}

void EndOpalSession()
{
    BYTE* cmdbuf = new BYTE[OPAL_BUFFER_SIZE];
    int cmd_cursor = sizeof(OPALHeader);
    RtlZeroMemory(cmdbuf, OPAL_BUFFER_SIZE);

    cmdbuf[cmd_cursor++] = OPAL_TOKEN::ENDOFSESSION;
    //{
    //    LOG(D1) << "Entering DtaCommand::complete(uint8_t EOD)";
    //    if (EOD) {
    //        cmdbuf[bufferpos++] = OPAL_TOKEN::ENDOFDATA;
    //        cmdbuf[bufferpos++] = OPAL_TOKEN::STARTLIST;
    //        cmdbuf[bufferpos++] = 0x00;
    //        cmdbuf[bufferpos++] = 0x00;
    //        cmdbuf[bufferpos++] = 0x00;
    //        cmdbuf[bufferpos++] = OPAL_TOKEN::ENDLIST;
    //    }
    //    /* fill in the lengths and add the modulo 4 padding */
    //    OPALHeader* hdr;
    //    hdr = (OPALHeader*)cmdbuf;
    //    hdr->subpkt.length = SWAP32(bufferpos - (sizeof(OPALHeader)));
    //    while (bufferpos % 4 != 0) {
    //        cmdbuf[bufferpos++] = 0x00;
    //    }
    //    hdr->pkt.length = SWAP32((bufferpos - sizeof(OPALComPacket))
    //        - sizeof(OPALPacket));
    //    hdr->cp.length = SWAP32(bufferpos - sizeof(OPALComPacket));
    //    if (bufferpos > MAX_BUFFER_LENGTH) {
    //        LOG(D1) << " Standard Buffer Overrun " << bufferpos;
    //        exit(EXIT_FAILURE);
    //    }
    //}


}

bool EnableGlobalLockingRange_NVMe(IN tstring& diskname, IN string &adm1_pwd)
{
    //START session
    bool ok = StartOpalSession(diskname, adm1_pwd);
    if(!ok)
    {
        _tprintf(_T("StartOpalSession() failed.\n"));
        return false;
    }
    //send command
    //END session
}