#pragma once

#include <intrin.h>
typedef enum _ATACOMMAND {
    IF_RECV = 0x5c,
    IF_SEND = 0x5e,
    IDENTIFY = 0xec,
} ATACOMMAND;

enum FEATURE_CODE : UINT16 {
    UNKNOWN = 0,
    TPer = 0x0001,
    LOCKING = 0x0002,
    GEOMETRY = 0x0003,
    ENTERPRISE = 0x0100,
    OPAL_V100 = 0x0200,
    SINGLE_USER = 0x0201,
    DATASTORE = 0x0202,
    OPAL_V200 = 0x0203,
};

enum OPAL_UID_TAG {
    // users
    SESSION_MGMT,               //< session management 
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
} ;

#define DEVICE_SN_LEN       20
#define DEVICE_REV_LEN      8
#define DEVICE_MODEL_LEN    40

#define OPAL_UID_LEN        8
