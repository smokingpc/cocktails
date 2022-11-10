#pragma once

#include <intrin.h>

enum FEATURE_CODE : UINT16 {
    UNKNOWN = 0,
    TPer = 0x0001,
    LOCKING = 0x0002,
    GEOMETRY = 0x0003,
    ENTERPRISE = 0x0100,
    OPAL_V100 = 0x0200,
    SINGLE_USER = 0x0201,
    DATASTORE =  0x0202,
    OPAL_V200 = 0x0203,
};

#pragma pack(push)
#pragma pack(1)
//The Discovery 0 Header. defined in Opal SSC Documentation
//WARN: ParamLength and Revision are BIG ENDIAN!!
typedef struct _DISCOVERY0_HEADER {
    UINT32 ParamLength;   // total length of (DISCOVERY0_HEADER + following FEATURE_DESCRIPTORs)
    UINT32 Revision;      /**< revision of the header 1 in 2.00.100 */
    UINT64 Reserved;
    UINT64 VendorSpecific[4];

    inline UINT32 GetParamLength(){return SwapEndian(ParamLength);}
    inline UINT32 GetRevision() { return SwapEndian(Revision); }

} DISCOVERY0_HEADER, *PDISCOVERY0_HEADER;

typedef struct _FEATURE_DESC_HEADER{
    UINT16 Code;            //note: this is Little Endian value.
    UINT8 NotUSed : 4;
    UINT8 Version : 4;      //shall be 0x01 in spec 2.0.1
    UINT8 Length;           //shall be 0x0C in spec 2.0.1
}FEATURE_DESC_HEADER, *PFEATURE_DESC_HEADER;

typedef struct _FEATURE_DESC_TPer{
    UINT8 SyncSupport : 1;
    UINT8 AsyncSupport : 1;
    UINT8 AckNackSupport : 1;
    UINT8 BufferMgmtSupport : 1;
    UINT8 StreamingSupport : 1;
    UINT8 Reserved1 : 1;
    UINT8 ComIdMgmtSupport : 1;
    UINT8 Reserved2 : 1;
    UINT8 Reserved3[11];
}FEATURE_DESC_TPer, *PFEATURE_DESC_TPer;

typedef struct _FEATURE_DESC_LOCKING{
    UINT8 LockingSupport : 1;
    UINT8 LockingEnabled : 1;
    UINT8 IsLocked : 1;
    UINT8 MediaEncryption : 1;
    UINT8 MBREnabled : 1;
    UINT8 MBRDone : 1;
    UINT8 Reserved1 : 2;
    UINT8 Reserved2[11];
}FEATURE_DESC_LOCKING, * PFEATURE_DESC_LOCKING;

typedef struct _FEATURE_DESC_GEOMETRY{
    UINT8 Reserved1[7];
    UINT32 LogicalBlockSize;
    UINT64 AlignmentGranularity;
    UINT64 LowestAlignedLBA;
}FEATURE_DESC_GEOMETRY, *PFEATURE_DESC_GEOMETRY;


typedef struct _FEATURE_DESC_ENTERPRISE_SSC {
    UINT16 BaseComID;
    UINT16 NumberComIDs;
    /* big endian
    uint8_t reserved01 : 7;
    uint8_t rangeCrossing : 1;
     */
    UINT8 RangeCrossing : 1;
    UINT8 Reserved1 : 7;

    UINT8 Reserved2;
    UINT16 Reserved3;
    UINT32 Reserved4;
    UINT32 Reserved5;
} FEATURE_DESC_ENTERPRISE_SSC, *PFEATURE_DESC_ENTERPRISE_SSC;

typedef struct _FEATURE_DESC_OPAL_V100 {
    UINT16 BaseComID;
    UINT16 NumberComIDs;
} FEATURE_DESC_OPAL_V100, *PFEATURE_DESC_OPAL_V100;

typedef struct _FEATURE_DESC_OPAL_V200 {
    UINT16 BaseComID;
    UINT16 NumberComIDs;
    UINT8 RangeCrossing : 1;
    UINT8 Reserved1 : 7;

    UINT16 NumlockingAdminAuth;
    UINT16 NumlockingUserAuth;
    UINT8 InitialPIN;
    UINT8 RevertedPIN;
    UINT8 Reserved2;
    UINT32 Reserved3;
} FEATURE_DESC_OPAL_V200, *PFEATURE_DESC_OPAL_V200;

typedef struct _FEATURE_DESC_SINGLE_USER_MODE{
    UINT32 NumberLockingObjects;
    /* big endian
    uint8_t reserved01 : 5;
    uint8_t policy : 1;
    uint8_t all : 1;
    uint8_t any : 1;
     */
    UINT8 Any : 1;
    UINT8 All : 1;
    UINT8 Policy : 1;
    UINT8 Reserved1 : 5;

    UINT8 Reserved2;
    UINT16 Reserved3;
    UINT32 Reserved4;
} FEATURE_DESC_SINGLE_USER_MODE, *PFEATURE_DESC_SINGLE_USER_MODE;

typedef struct _FEATURE_DATASTORE {
    UINT16 Reserved1;
    UINT16 MaxTables;
    UINT32 MaxSizeTables;
    UINT32 TableSizeAlignment;
} FEATURE_DESC_DATASTORE, *PFEATURE_DESC_DATASTORE;

typedef struct _FEATURE_DESCRIPTOR
{
    FEATURE_DESC_HEADER Header;
    union{
        FEATURE_DESC_TPer               TPer;
        FEATURE_DESC_LOCKING            Locking;
        FEATURE_DESC_GEOMETRY           Geometry;
        FEATURE_DESC_ENTERPRISE_SSC     Enterprise;
        FEATURE_DESC_SINGLE_USER_MODE   SingleUserMode;
        FEATURE_DESC_OPAL_V100          OpalV100;
        FEATURE_DESC_OPAL_V200          OpalV200;
        FEATURE_DESC_DATASTORE          Datastore;
    }DUMMYSTRUCTNAME;
}FEATURE_DESCRIPTOR, *PFEATURE_DESCRIPTOR;
#pragma pack(pop)

