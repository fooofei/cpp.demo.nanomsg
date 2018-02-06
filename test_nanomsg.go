package main

/*
#cgo CFLAGS: -I../src -DNN_STATIC_LIB
#cgo LDFLAGS: libnanomsg.a -lanl
#include <pair.h>
#include <nn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
could not determine kind of name for C.free    should #include <stdlib.h>
could not determine kind of name for C.sizeof  should unsafe.Sizeof

20180206 still not worked, the sock is 0
cgo 只能在 unix 运行， Windows 还需要 MINGW 环境 不适合
 */

// unused
type Socket struct{
	socket C.int
}

func send_recv(sock C.int) {
	var to C.int
	var ret = C.nn_setsockopt(sock, C.NN_SOL_SOCKET,
		C.NN_RCVTIMEO,
			unsafe.Pointer(&to),
			C.size_t(unsafe.Sizeof(to)))

	if ret != 0 {
		fmt.Println("[!] error of nn_setsockopt")
		return
	}
	var size = 1024
	var recvbuf = make([]byte, size)
	for {
		ret = C.nn_recv(sock,unsafe.Pointer((&recvbuf)), C.size_t(size),0)
		if(ret>0){
			fmt.Println("recv %s", recvbuf)
		}

	}
}

func main() {
	var sock C.int
	sock = C.nn_socket(C.AF_SP, C.NN_PAIR)
	if(sock<=0){
		panic("the sock <=0")
	}
	defer C.nn_close(sock)



	var url = "tcp://127.0.0.1:10000"
	var c_url = C.CString(url)
	defer C.free(unsafe.Pointer(c_url))
	send_recv(sock)
	C.nn_connect(sock, c_url)



	fmt.Println("main end in go")
}
