#pragma once
//Basic IOCTL_STORAGE_QUERY_PROPERTY querying is supported since WinXP.

BOOL QueryDeviceProperty(tstring& devpath);
BOOL QueryAdapterProperty(tstring& devpath);

//returned data is SCSI3 SCSIOP_INQUIRY with VPD enable VPD_DEVICE_IDENTIFIERS
//returned information.
BOOL QueryDeviceIdProperty(tstring& devpath);

#if 0       //for Driver Use Only
//Supported since WinVista and Win2008
BOOL QueryDeviceUniqueIdProperty(tstring& devpath);
#endif

//Supported since WinVista and Win2008
BOOL QueryDeviceWriteCacheProperty(tstring& devpath);

#if 0       //reserved for System Use
BOOL QueryMiniportProperty(tstring& devpath);
#endif

//Supported since WinVista and Win2008
BOOL QueryAccessAlignmentProperty(tstring& devpath);

//Supported since Win7 and Win2008 R2
BOOL QueryDeviceSeekPenaltyProperty(tstring& devpath);

//Supported since Win7 and Win2008 R2
BOOL QueryDeviceTrimProperty(tstring& devpath);


#if 0       //reserved for System Use
BOOL QueryDeviceWriteAggregationProperty(tstring& devpath);
BOOL QueryDeviceDeviceTelemetryProperty(tstring& devpath);
#endif

//Supported since Win8 and Win2012
BOOL QueryDeviceLBProvisioningProperty(tstring& devpath);
//Supported since Win8 and Win2012
BOOL QueryDevicePowerProperty(tstring& devpath);
//Supported since Win8 and Win2012
BOOL QueryDeviceCopyOffloadProperty(tstring& devpath);

#if 0       //reserved for System Use
BOOL QueryDeviceResiliencyProperty(tstring& devpath);
#endif

BOOL QueryDeviceMediumProductType(tstring& devpath);
BOOL QueryAdapterRpmbProperty(tstring& devpath);
BOOL QueryAdapterCryptoProperty(tstring& devpath);
BOOL QueryDeviceIoCapabilityProperty(tstring& devpath);
BOOL QueryAdapterProtocolSpecificProperty(tstring& devpath);
BOOL QueryDeviceProtocolSpecificPropert(tstring& devpath);
BOOL QueryAdapterTemperatureProperty(tstring& devpath);
BOOL QueryDeviceTemperatureProperty(tstring& devpath);
BOOL QueryAdapterPhysicalTopologyProperty(tstring& devpath);
BOOL QueryDevicePhysicalTopologyProperty(tstring& devpath);

#if 0       //reserved for System Use
BOOL QueryDeviceAttributesProperty(tstring& devpath);
#endif
BOOL QueryDeviceManagementStatus(tstring& devpath);
BOOL QueryAdapterSerialNumberProperty(tstring& devpath);

#if 0       //reserved for System Use
BOOL QueryDeviceLocationProperty(tstring& devpath);
#endif

BOOL QueryDeviceNumaProperty(tstring& devpath);

#if 0       //reserved for System Use
BOOL QueryDeviceZonedDeviceProperty(tstring& devpath);
#endif

BOOL QueryDeviceUnsafeShutdownCount(tstring& devpath);
BOOL QueryDeviceEnduranceProperty(tstring& devpath);
BOOL QueryDeviceLedStateProperty(tstring& devpath);

#if 0       //reserved for System Use
BOOL QueryDeviceSelfEncryptionProperty(tstring& devpath);
#endif

BOOL QueryFruIdProperty(tstring& devpath);
