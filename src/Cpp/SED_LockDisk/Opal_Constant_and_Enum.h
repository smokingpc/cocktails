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

#define DEVICE_SN_LEN       20
#define DEVICE_REV_LEN      8
#define DEVICE_MODEL_LEN    40

