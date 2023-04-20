package main

//該死的package name不可以帶底線 @#$@#%#$%^$
//e.g."win32_api_writefile/test" => "<%workspace name%>/<%package name%>"
import (
	test "win32_api_writefile/test"
)

func main() {
	var filepath string = "c:\\1\\test.txt"
	var testdata string = "abcdefghijklmnopqrstuvwxyz"

	handle := test.MyOpenFile(filepath)

	datablob := []byte(testdata)
	test.MyWriteFile(handle, datablob)
	test.MyCloseFile(handle)
}
