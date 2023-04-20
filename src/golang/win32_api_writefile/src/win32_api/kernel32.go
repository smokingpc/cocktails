package win32_api

//CGO 的編譯參數與 include file都放在 import "C" 前面的廣域註解區塊內

/*
#cgo CFLAGS: -I$INCLUDE
#cgo LDFLAGS: -L$LIB -lKernel32.lib
#include <fileapi.h>
*/
import "C"

func OpenFile(path string) uintptr {
	return 0
}

//...int8 => 不定長度int8 array
func WriteFile(data ...int8) bool {
	var ok bool = false

	return ok
}

func CloseFile(handle uintptr) {
}
