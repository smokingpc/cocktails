package winapi

import (
	"fmt"
	"unsafe"

	native "golang.org/x/sys/windows"
)

type STORAGE_DEVICE_NUMBER struct {
	DeviceType      uint32
	DeviceNumber    uint32
	PartitionNumber uint32
}

// IOCTL_STORAGE_GET_DEVICE_NUMBER
const (
	FILE_DEVICE_MASS_STORAGE uint32 = 0x0000002d
	IOCTL_STORAGE_BASE       uint32 = FILE_DEVICE_MASS_STORAGE
)
const (
	METHOD_BUFFERED   uint8 = 0
	METHOD_IN_DIRECT  uint8 = 1
	METHOD_OUT_DIRECT uint8 = 2
	METHOD_NEITHER    uint8 = 3
)
const (
	FILE_ANY_ACCESS     uint16 = 0
	FILE_SPECIAL_ACCESS uint16 = FILE_ANY_ACCESS
	FILE_READ_ACCESS    uint16 = 0x0001
	FILE_WRITE_ACCESS   uint16 = 0x0002
)
const (
	IOCTL_STORAGE_GET_DEVICE_NUMBER uint32 = (IOCTL_STORAGE_BASE << 16) | uint32(FILE_ANY_ACCESS<<14) | uint32(0x0420<<2) | uint32(METHOD_BUFFERED)
)

const (
	PHYDISK_PATH_FORMAT string = "\\\\?\\PhysicalDrive%d"
)

func EnumDiskInterfaces() []string {
	//GUID_DEVINTERFACE_DISK
	var disk_guid native.GUID = native.GUID{0x53f56307, 0xb6bf, 0x11d0, [8]byte{0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}}
	// infoset, err := native.SetupDiGetClassDevsEx(&disk_guid, "", uintptr(0),
	// 	native.DIGCF_DEVICEINTERFACE|native.DIGCF_PRESENT, native.DevInfo(0), "")

	disklist, _ := native.CM_Get_Device_Interface_List("", &disk_guid, native.CM_GET_DEVICE_INTERFACE_LIST_PRESENT)
	return disklist
}

func QueryPhysicalDriveName(devpath string) string {
	var drive_name string = ""
	var err error
	filepath, _ := native.UTF16PtrFromString(devpath)

	fmt.Printf("CreateFile() got devpath=%s\n", devpath)
	handle, err := native.CreateFile(
		filepath,
		native.GENERIC_READ,
		native.FILE_SHARE_READ|native.FILE_SHARE_WRITE,
		nil,
		native.OPEN_EXISTING,
		0,
		0)
	fmt.Printf("CreateFile() ret Handle = %X\n", handle)
	if ^uintptr(0) == uintptr(handle) {
		fmt.Printf("CreateFile() failed, errmsg = %s\n", err.Error())
		return ""
	}

	var disk_num STORAGE_DEVICE_NUMBER
	var size uint32 = uint32(unsafe.Sizeof(disk_num))
	var ret_size uint32 = 0
	var outbuf *byte = (*byte)(unsafe.Pointer(&disk_num))
	// var overlap native.Overlapped
	// var inbuf [16]byte
	err = native.DeviceIoControl(
		handle,
		IOCTL_STORAGE_GET_DEVICE_NUMBER,
		nil, 0,
		outbuf, size,
		&ret_size, nil)

	if err == nil {
		drive_name = fmt.Sprintf(PHYDISK_PATH_FORMAT, disk_num.DeviceNumber)
	}

	native.CloseHandle(handle)
	return drive_name
}
