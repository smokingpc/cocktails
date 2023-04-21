package mycgo

/*
#cgo CFLAGS: -I ${SRCDIR}/../c-code
#cgo LDFLAGS: -L ${SRCDIR}/../c-code/test.o
#include "test.h"
*/
import "C"

func MyMessageBox(msg string) {
	var data C.MY_DATA
	data.id = 123
	//msghandle := cgo.NewHandle(msg)
	//C.TestMsg(msghandle)
}
