package main

//該死的package name不可以帶underscore, space, dash
//但是目錄名與檔名可以
//e.g."win32_api_writefile/test" => "<%workspace name%>/<%package name%>"

import (
	"fmt"
	winapi "win32_enum_disks/win32"
)

func main() {
	disklist := winapi.EnumDiskInterfaces()

	for index, element := range disklist {
		fmt.Printf("Disk[%d]=%s\n", index, element)
		name := winapi.QueryPhysicalDriveName(element)
		fmt.Printf("  DriveName=%s\n\n", name)
	}
}
