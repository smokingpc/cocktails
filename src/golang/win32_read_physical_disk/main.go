package main

import (
	"fmt"

	native "golang.org/x/sys/windows"
)

func main() {
	var diskpath string = "\\\\.\\PhysicalDrive0"
	var buffer []byte = make([]byte, 4096)
	path, _ := native.UTF16PtrFromString(diskpath)
	handle, _ := native.CreateFile(
		path,
		native.GENERIC_READ,
		native.FILE_SHARE_READ|native.FILE_SHARE_WRITE,
		nil,
		native.OPEN_EXISTING,
		0,
		0)

	native.ReadFile(handle, buffer, nil, nil)
	fmt.Printf("[1st]\n%v\n", buffer)
	native.ReadFile(handle, buffer, nil, nil)
	fmt.Printf("[2nd]\n%v\n", buffer)
	native.ReadFile(handle, buffer, nil, nil)
	fmt.Printf("[3rd]\n%v\n", buffer)

	var high int32 = 0x00000009
	var low uint32 = 0xFC000000

	_, _ = native.SetFilePointer(handle,
		int32(low),
		&high,
		native.FILE_BEGIN)

	native.ReadFile(handle, buffer, nil, nil)
	fmt.Printf("[4th]\n%v\n", buffer)
	native.ReadFile(handle, buffer, nil, nil)
	fmt.Printf("[5th]\n%v\n", buffer)

	native.CloseHandle(handle)
}
