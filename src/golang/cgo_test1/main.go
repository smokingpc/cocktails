package main

/*
#include "test.h"
*/
import "C"

//目前cgo不支援 MSVC，必須去抓 MingW-Win64 套件來用，裡面有包含gcc.exe與g++.exe
//https://github.com/niXman/mingw-builds-binaries/releases
//Build Machine要注意在script裡切換 CC 與 CXX 環境變數，免得干擾到其他MSVC project編譯

func main() {
	C.TestMsg(C.CString("12345678"))
}
