#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// nanomsg
#include <pair.h>
#include <nn.h>



int
send_recv(int sock)
{
    int to = 100;
    int ret;
    char * recvbuf = 0;
    int count = 0;
    enum { send_size = 0x100, };
    char sendbuf[send_size] = { 0 };

    ret = nn_setsockopt(sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to));
    if (ret != 0) return ret;



    for (;;)
    {
        recvbuf = 0;
        ret = nn_recv(sock, &recvbuf, NN_MSG, 0);
        printf("client recv ret %d for(%d)\n", ret, count);
        if (ret > 0)
        {
            printf("client recv %s\n", recvbuf);
            nn_freemsg(recvbuf);
        }

        ret = snprintf(sendbuf, send_size, "client send %d", count);
        sendbuf[ret] = 0;
        ret = nn_send(sock, sendbuf, ret, NN_DONTWAIT);
        printf("client send ret %d for(%d)\n", ret, count);

        count += 1;
    }

}


int
main()
{
    int sock = 0;
    int ret;
    const char * url1 = "ipc:///tmp/pair.ipc";
    const char * url = "tcp://127.0.0.1:10000";


    for (;;)
    {
        sock = nn_socket(AF_SP, NN_PAIR);
        if (sock < 0) break;

        ret = nn_connect(sock, url);
        if (ret < 0) break;

        send_recv(sock);

        break;
    }

    if (sock >= 0)
    {
        nn_close(sock);
    }

    return 0;
}
