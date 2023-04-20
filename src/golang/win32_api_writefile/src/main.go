package main

import "win32_api"

func main() {
	var filepath string = "c:\\1\\test.txt"
	var handle uintptr = win32_api.OpenFile(filepath)
}
