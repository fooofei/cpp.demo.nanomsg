
#include <stdio.h>


// nanomsg
#include <pubsub.h>
#include <nn.h>
#include <utils/sleep.h>

// 遇到的问题:
//   先启动 sub， 然后启动 pub， pub 发送 4 个消息，sub 只能接收到后面 3 个
//   继续不停反复测试， 后续 pub 启动后， sub 不再接收消息


void recv(int sock)
{
    int to = 100;
    int ret;
    int count = 0;
    char * msg;

    for (;;)
    {
        msg = 0;
        ret = nn_recv(sock, &msg, NN_MSG, NN_DONTWAIT);

        printf("[+] check %d suber recv bytes %d:%.*s\n", count, ret, ret, (msg ? msg : ""));

        if (msg) {
            nn_freemsg(msg);
        }
        count += 1;

        nn_sleep(1000);//milli
    }
}

int main()
{
    int sock = 0;
    int ret;
    const char * addr = "tcp://127.0.0.1:1200";


    for (;;)
    {
        sock = nn_socket(AF_SP, NN_SUB);
        if (sock < 0) break;

        // "" means mathes any message 
        // nn_setsockopt (s, NN_SUB, NN_SUB_SUBSCRIBE, "Hello", 5); means match message begin with "Hello"
        ret = nn_setsockopt(sock, NN_SUB, NN_SUB_SUBSCRIBE, "",0);
        if (ret < 0) {
            fprintf(stderr, "[!] nn_setsockopt ret=%d\n", ret);
            break;
        }

        //ret = nn_bind(sock, addr);
        ret = nn_connect(sock, addr);
        if (ret < 0) {
            fprintf(stderr, "[!] nn_connect ret=%d\n", ret);
            break;
        }

        recv(sock);

        break;
    }

    if (sock < 0) {
        fprintf(stderr, "[!] sock invalid\n");
    }

    if (sock >= 0)
    {
        nn_close(sock);
    }

    return 0;
}
