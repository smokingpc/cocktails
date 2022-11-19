#pragma once


typedef struct _ATA_IDENTIFY_RESP {
    uint8_t Reserved0[20];
    uint8_t SN[20];
    uint8_t Reserved1[6];
    uint8_t FirmwareRev[8];
    uint8_t Model[40];
} ATA_IDENTIFY_RESP, *PATA_IDENTIFY_RESP;


typedef struct _OPAL_DISKINFO
{
    FEATURE_DESC_TPer               TPer;
    FEATURE_DESC_LOCKING            Locking;
    FEATURE_DESC_GEOMETRY           Geometry;
    FEATURE_DESC_ENTERPRISE_SSC     Enterprise;
    FEATURE_DESC_SINGLE_USER_MODE   SingleUserMode;
    FEATURE_DESC_OPAL_V100          OpalV100;
    FEATURE_DESC_OPAL_V200          OpalV200;
    FEATURE_DESC_DATASTORE          Datastore;

    char    SN[20] = {0};
    char    NullTerm1 = '\0';
    char    *FirmwareRev[8] = {0};
    char    NullTerm2 = '\0';
    char    Model[40] = {0};
    char    NullTerm3 = '\0';
}OPAL_DISKINFO, *POPAL_DISKINFO;

void ShowStructureSizes();
bool Discovery0_NVMe(IN OUT OPAL_DISKINFO& info, IN tstring& diskname);
bool Discovery0_SATA(IN OUT OPAL_DISKINFO& info, IN tstring& diskname);
bool Identify_NVMe(IN OUT OPAL_DISKINFO& info, IN tstring& diskname);
bool Identify_SATA(IN OUT OPAL_DISKINFO& info, IN tstring& diskname);
