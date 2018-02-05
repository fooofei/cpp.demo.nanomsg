package main

/*
#cgo CFLAGS: -I../src -DNN_STATIC_LIB
#cgo LDFLAGS: libnanomsg.a -lanl
#include <pair.h>
#include <nn.h>
*/
import "C"

import (
	"fmt"
	"unsafe"
)

/*
the library libnanomsg.a is build by `static_build.sh`

两个错误解决不了
# command-line-arguments
could not determine kind of name for C.free
could not determine kind of name for C.sizeof

*/

func send_recv(sock int) {
	var to C.int
	var ret = C.nn_setsockopt(sock, C.NN_SOL_SOCKET, C.NN_RCVTIMEO, &to, C.sizeof(to))

	if ret != 0 {
		fmt.Println("[!] error of nn_setsockopt")
		return
	}
	var size = 1024
	var recvbuf = make([]byte, size)
	for {
		break
	}
}

func main() {
	var sock = C.nn_socket(C.AF_SP, C.NN_PAIR)
	var url = "tcp://127.0.0.1:10000"
	var c_url = C.CString(url)
	defer C.free(unsafe.Pointer(c_url))
	C.nn_connect(sock, c_url)

	C.nn_close(sock)
	fmt.Println("main end in go")
}
