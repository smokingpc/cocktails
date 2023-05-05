package win32lib

import (
	"fmt"
	"strings"
	"syscall"
	"unsafe"

	native "golang.org/x/sys/windows"
)

var (
	modnewdev           = native.NewLazySystemDLL("newdev.dll")
	procDiInstallDevice = modnewdev.NewProc("DiInstallDevice")

	DIURFLAG_NO_REMOVE_INF uint32 = (0x00000001) // Do not remove inf from the system
)

func DiInstallDevice(infoset native.DevInfo, dev_info *native.DevInfoData, drv_info *native.DrvInfoData, flags uint32) (bool, error) {
	var reboot int = 0
	_, _, errno := syscall.SyscallN(procDiInstallDevice.Addr(),
		uintptr(0),
		uintptr(infoset),
		uintptr(unsafe.Pointer(dev_info)),
		uintptr(unsafe.Pointer(drv_info)),
		uintptr(flags),
		uintptr(unsafe.Pointer(&reboot)))

	var err error = nil

	if 0 == errno {
		err = nil
	} else {
		err = errno
	}

	return (reboot != 0), err
}

func UnbindDriverByDevice(infoset native.DevInfo, dev_info *native.DevInfoData, keyword string) {
	var err error
	var index int = 0
	err = native.SetupDiBuildDriverInfoList(infoset, dev_info, native.SPDIT_COMPATDRIVER)
	if nil != err {
		fmt.Printf("SetupDiBuildDriverInfoList() failed, err=%s\n", err.Error())
		return
	}

	for {
		drv_info, err := native.SetupDiEnumDriverInfo(infoset, dev_info, native.SPDIT_COMPATDRIVER, index)
		if nil != err {
			break
		}
		fmt.Printf("SetupDiEnumDriverInfo() => found driver=%s\n", drv_info.Description())

		if !strings.EqualFold(keyword, drv_info.Description()) {
			need_reboot, err := DiInstallDevice(infoset, dev_info, drv_info, DIURFLAG_NO_REMOVE_INF)
			if nil != err {
				fmt.Printf("DiInstallDevice() failed. errmsg=%s\n", err.Error())
			} else {
				fmt.Printf("DiInstallDevice() complete. NeedReboot=%t\n", need_reboot)
			}
			break
		}

		index++
	}
	native.SetupDiDestroyDriverInfoList(infoset, dev_info, native.SPDIT_COMPATDRIVER)
}

func FindAndUnbindDriver(keyword string) {
	var storport_guid native.GUID = native.GUID{0x2accfe60, 0xc130, 0x11d2, [8]byte{0xb0, 0x82, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}}
	//native.GUID{0x2accfe60, 0xc130, 0x11d2, [8]byte{0xb0, 0x82, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}}
	var err error
	var infoset native.DevInfo
	var devid int

	infoset, err = native.SetupDiGetClassDevsEx(
		&storport_guid, "", uintptr(0),
		native.DIGCF_PRESENT|native.DIGCF_DEVICEINTERFACE, native.DevInfo(0), "")
	if nil != err {
		fmt.Printf("SetupDiGetClassDevsEx() failed, err=%s\n", err.Error())
		return
	}

	devid = 0
	for {
		fmt.Printf("devid=%d\n", devid)
		dev_info, err := native.SetupDiEnumDeviceInfo(infoset, devid)
		if err != nil {
			fmt.Printf("SetupDiEnumDeviceInfo() failed. err=%s\n", err.Error())
			break
		}
		var key native.DEVPROPKEY = native.DEVPROPKEY{
			native.DEVPROPGUID{0xa8b865dd, 0x2e3d, 0x4094, [8]byte{0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6}},
			4}
		property, err := native.SetupDiGetDeviceProperty(infoset, dev_info, &key)
		if nil == err {
			str := fmt.Sprintf("%v", property)
			if strings.EqualFold(str, keyword) {
				UnbindDriverByDevice(infoset, dev_info, keyword)
			}
		} else {
			fmt.Printf("SetupDiGetDeviceProperty() failed. err=%s\n", err.Error())
		}

		devid++
	}

}
