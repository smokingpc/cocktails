#pragma once

#include <intrin.h>
typedef enum _ATACOMMAND {
    IF_RECV = 0x5c,
    IF_SEND = 0x5e,
    IDENTIFY = 0xec,
} ATACOMMAND;

enum FEATURE_CODE : UINT16 {
    NO_FEATURE = 0,
    TPer = 0x0001,
    LOCKING = 0x0002,
    GEOMETRY = 0x0003,
    ENTERPRISE = 0x0100,
    OPAL_V100 = 0x0200,
    SINGLE_USER = 0x0201,
    DATASTORE = 0x0202,
    OPAL_V200 = 0x0203,
};

enum OPAL_ATOM_TOKEN : UINT16
{
    NO_TOKEN = 0,               //if data is only 1 bytes, don't push TOKEN in data payload

    //all type tokens are greater than 0x80.
    //values smaller than 0x80 should be TinyAtom. TinyAtom doesn't have token.


    //short token only output 1 byte to opal buffer.
    //set it to UINT16 is just for combine all toekn into one enum definition.
    SHORT_UINT = 0x0080,           //OPAL_ATOM_TOKEN::Type == "UINT"
    SHORT_UINT1 = 0x0081,          //1 byte unsigned int (same as UINT8)
    SHORT_UINT2 = 0x0082,          //2 bytes unsigned int (same as UINT16)
    SHORT_UINT3 = 0x0083,          //3 bytes unsigned int
    SHORT_UINT4 = 0x0084,          //4 bytes unsigned int (same as UINT32)
    SHORT_UINT5 = 0x0085,          //5 bytes unsigned int
    SHORT_UINT6 = 0x0086,          //6 bytes unsigned int
    SHORT_UINT7 = 0x0087,          //7 bytes unsigned int
    SHORT_UINT8 = 0x0088,          //8 bytes unsigned int (same as UINT64)
    SHORT_UINT9 = 0x0089,          //9 bytes unsigned int
    SHORT_UINT10 = 0x008A,         //10 bytes unsigned int

    //short token only output 1 byte to opal buffer.
    //set it to UINT16 is just for combine all toekn into one enum definition.
    SHORT_BYTES = 0x00A0,       //OPAL_SHORT_TOKEN::Type == "Byte Sequence"
    SHORT_BYTES_1 = 0x00A1,        //1 byte 
    SHORT_BYTES_2 = 0x00A2,        //2 bytes array => BYTE[2]
    SHORT_BYTES_3 = 0x00A3,        //3 bytes array => BYTE[3]
    SHORT_BYTES_4 = 0x00A4,        //4 bytes array => BYTE[4]
    SHORT_BYTES_5 = 0x00A5,        //5 bytes array => BYTE[5]
    SHORT_BYTES_6 = 0x00A6,        //6 bytes array => BYTE[6]
    SHORT_BYTES_7 = 0x00A7,        //7 bytes array => BYTE[7]
    SHORT_BYTES_8 = 0x00A8,        //8 bytes array => BYTE[8]
    SHORT_BYTES_9 = 0x00A9,        //9 bytes array => BYTE[9]
    SHORT_BYTES_10 = 0x00AA,        //10 bytes array => BYTE[10]
    SHORT_BYTES_11 = 0x00AB,        //11 bytes array => BYTE[11]
    SHORT_BYTES_12 = 0x00AC,        //12 bytes array => BYTE[12]
    SHORT_BYTES_13 = 0x00AD,        //13 bytes array => BYTE[13]
    SHORT_BYTES_14 = 0x00AE,        //14 bytes array => BYTE[14]
    SHORT_BYTES_15 = 0x00AF,        //15 bytes array => BYTE[15]

    MID_BYTES = 0xD000,         //MID_BYTES begin from 0xD010. there is no value smaller than MID_BYTES_16
    MID_BYTES_16 = 0xD010,      //16 bytes array => BYTE[16]
    MID_BYTES_24 = 0xD018,      //24 bytes array => BYTE[24]
    MID_BYTES_32 = 0xD020,      //32 bytes array => BYTE[32]
};

enum OPAL_DATA_TOKEN : BYTE {
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

    //session related
    HOST_CHALLENGE = 0x00,
    HOST_SIGN_AUTH = 0x03,      //HostSigningAuthority in StartSession

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
};

////priority to use OPAL feature is :
////OPAL_V200 first=> OPAL_V100 => OPAL_ENT
////Set these values same as feature code, it's tricky way to compare priority.
//enum OPAL_VER
//{
//    NO_OPAL = 0,
//    VER_ENT = 0x100,
//    VER_V100 = 0x200,
//    VER_V200 = 0x203,
//};

enum OPAL_UID_TAG {
    // users
    SMUID,                //< session manager
    THISSP,                     //< special "thisSP" syntax (SP==Security Provider)
    ADMINSP,                    //< Administrative SP (SP==Security Provider)
    LOCKINGSP,                  //< Locking SP (SP==Security Provider)
    
    ENT_LOCKINGSP,              //< ENTERPRISE Locking SP  (SP==Security Provider)
    ANYBODY,                    //<anybody
    SECURITY_ID,                        //< SID
    ADMIN1,                     //< ADMIN1
    USER1,                      //< USER1
    USER2,                      //< USER2
    PSID_USER,                       //< PSID user
    ENT_BANDMASTER0,            //< BandMaster 0
    ENT_ERASEMASTER,            //< EraseMaster

    // tables
    LOCKRANGE_GLOBAL,           //< Locking_GlobalRange
    LOCKRANGE_1,                //< Locking_Range 1
    LOCKRANGE_2,                //< Locking_Range 2
    LOCKRANGE_3,                //< Locking_Range 3
    LOCKRANGE_4,                //< Locking_Range 4


    LOCKRANGE_ACE_READDLOCK,    //< ACE_Locking_Range_Set_ReaddLocked UID
    LOCKRANGE_ACE_WRITELOCK,    //< ACE_Locking_Range_Set_WrLocked UID
    LOCKRANGE_GLOBAL_ACE_READLOCK,  //< ACE_Locking_Range_GLOBAL_Set_RdLocked UID
    LOCKRANGE_GLOBAL_ACE_WRITELOCK, //< ACE_Locking_Range_GLOBAL_Set_WrLocked UID
    MBRCONTROL_SET_DONE_TO_DOR, //< ACE_MBRControl_Set_DoneToDOR
    MBR_CONTROL,                //< MBR Control
    SHADOW_MBR,
    AUTHORITY_TABLE,
    C_PIN_TABLE,
    OPAL_LOCKING_INFO_TABLE,
    ENT_LOCKING_INFO_TABLE,
    //C_PIN_TABLE object ID's
    C_PIN_MSID,
    C_PIN_SID,
    C_PIN_ADMIN1,
    //half UID's (only first 4 bytes used)
    HALF_UID_AUTHORITY_OBJ_REF,
    HALF_UID_BOOLEAN_ACE,
    
    // omitted optional parameter
    HEXFF_OMITTER,
};

enum OPAL_METHOD_TAG
{
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
};

enum OPAL_STATUS {
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
};


#define DEVICE_SN_LEN       20
#define DEVICE_REV_LEN      8
#define DEVICE_MODEL_LEN    40

#define OPAL_UID_LEN        8
#define DISCOVERY0_COMID    (UINT16)0x0001
#define DEFAULT_HOST_SID     0x0101               //hardcode my host session id...