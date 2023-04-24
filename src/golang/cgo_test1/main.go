package main

/*
#include "test.h"
*/
import "C"

func main() {
	C.TestMsg(C.CString("28825252"))
}
