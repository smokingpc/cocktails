#pragma once

#include <intrin.h>

enum FEATURE_CODE : UINT16 {
    UNKNOWN = 0,
    TPer = 0x0001,
    LOCKING = 0x0002,
    GEOMETRY = 0x0003,
    SSC_V2 = 0x0203,
};



//The Discovery 0 Header. defined in Opal SSC Documentation
//WARN: ParamLength and Revision are BIG ENDIAN!!
typedef struct _DISCOVERY0_HEADER {
    UINT32 ParamLength;   /**< the lenght of the header 48 in 2.00.100*/
    UINT32 Revision;      /**< revision of the header 1 in 2.00.100 */
    UINT64 Reserved;
    UINT64 VendorSpecific[4];
} DISCOVERY0_HEADER, *PDISCOVERY0_HEADER;

typedef struct _FEATURE_DESC_HEADER{
    UINT16 Code;        //note: this is Big Endian value.
    UINT8 NotUSed : 4;
    UINT8 Version : 4;      //shall be 0x01 in spec 2.0.1
    UINT8 Length;           //shall be 0x0C in spec 2.0.1

    inline FEATURE_CODE GetFeatureCode(){ return (FEATURE_CODE)_byteswap_ushort(Code);}
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
}FEATURE_DESC_TPer, * PFEATURE_DESC_TPer;

typedef struct _FEATURE_DESC_GEOMETRY{
    UINT8 Reserved1[7];
    UINT32 LogicalBlockSize;
    UINT64 AlignmentGranularity;
    UINT64 LowestAlignedLBA;
}FEATURE_DESC_GEOMETRY, *PFEATURE_DESC_GEOMETRY;

typedef struct _FEATURE_DESCRIPTOR
{
    FEATURE_DESC_HEADER Header;
    union{

    }DUMMYSTRUCTNAME;
}FEATURE_DESCRIPTOR, *PFEATURE_DESCRIPTOR;
