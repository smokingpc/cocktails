package main

//該死的package name不可以帶底線 @#$@#%#$%^$
//該死的package name不可以帶underscore, space, dash
//但是目錄名與檔名可以
//e.g."win32_api_writefile/test" => "<%workspace name%>/<%package name%>"
import (
	test2 "win32_api_writefile/test-2"
)

func main() {
	var filepath string = "c:\\1\\test.txt"
	var testdata string = "abcdefghijklmnopqrstuvwxyz"

	handle := test2.MyOpenFile(filepath)

	datablob := []byte(testdata)
	test2.MyWriteFile(handle, datablob)
	test2.MyCloseFile(handle)
}
