package main

import (
	"fmt"
	"os"

	win32 "win32_unbind_driver/win32_lib"
)

func main() {
	var argc int = len(os.Args)
	if argc < 2 {
		fmt.Printf("Please input the keyword of driver description you want unbind...\n")
		return
	}
	keyword := os.Args[1]
	fmt.Printf("keyword=%s\n", keyword)
	win32.FindAndUnbindDriver(keyword)
}
