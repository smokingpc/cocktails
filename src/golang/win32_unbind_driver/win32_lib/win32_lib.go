package win32lib

import (
	"fmt"
	"strings"

	native "golang.org/x/sys/windows"
)

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

		if !strings.EqualFold(keyword, drv_info.Description()) {
			err := native.SetupDiSetSelectedDriver(infoset, dev_info, drv_info)
			if nil != err {
				fmt.Printf("SetupDiSetSelectedDriver() failed, err=%s\n", err.Error())
			}
			break
		}

		index++
	}
	native.SetupDiDestroyDriverInfoList(infoset, dev_info, native.SPDIT_COMPATDRIVER)
}

func FindAndUnbindDriver(keyword string) {
	var storport_guid native.GUID = native.GUID{0x2accfe60, 0xc130, 0x11d2, [8]byte{0xb0, 0x82, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b}}
	var err error
	var infoset native.DevInfo
	var devid int

	infoset, err = native.SetupDiGetClassDevsEx(
		&storport_guid, "", uintptr(0),
		native.DIGCF_PRESENT, native.DevInfo(0), string(""))
	if nil != err {
		fmt.Printf("SetupDiGetClassDevsEx() failed, err=%s\n", err.Error())
		return
	}

	devid = 0
	for {
		dev_info, err := native.SetupDiEnumDeviceInfo(infoset, devid)
		if nil == dev_info || err != nil {
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
		}

		devid++
	}
}
