package win32test

//該死的package name不可以帶underscore, space, dash
//但是目錄名與檔名可以
//package關鍵字帶的名字可以跟目錄以及檔名不同

//1.CGO出錯時，到terminal去跑 go list -json -compiled -e  會看到問題提示
//2.CGO需要跟CMAKE一樣的compiler 變數 (CC = 或 CXX = )
//3.CGO 的編譯參數與 include file都放在 import "C" 前面的廣域註解區塊內

import "golang.org/x/sys/windows"

func MyOpenFile(path string) windows.Handle {
	//  _  is "skip this return value field"
	filepath, _ := windows.UTF16PtrFromString(path)

	handle, _ := windows.CreateFile(
		filepath,
		windows.GENERIC_ALL,
		windows.FILE_SHARE_READ|windows.FILE_SHARE_WRITE,
		nil,
		windows.CREATE_NEW,
		windows.FILE_ATTRIBUTE_NORMAL,
		0)
	return handle
}

// ...int8 => 不定長度int8 array
func MyWriteFile(handle windows.Handle, data []byte) bool {
	var done_size uint32 = 0
	err := windows.WriteFile(handle, data, &done_size, nil)
	return (err == nil)
}

func MyCloseFile(handle windows.Handle) {
	windows.CloseHandle(handle)
}
