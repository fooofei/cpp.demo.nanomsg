
#include <stdio.h>


// nanomsg
#include <pubsub.h>
#include <nn.h>
#include <utils/sleep.h>
#include <utils/random.h>

// http://nanomsg.org/v0.5/nn_pubsub.7.html
// https://stackoverflow.com/questions/44456659/how-to-setup-a-pub-sub-in-nanomsg-between-a-c-and-python-sides
// https://github.com/nanomsg/nanomsg/blob/master/demo/pubsub_demo.c

// https://github.com/nanomsg/nanomsg/issues/283 作者说了 pub/sub 模式就是不可靠 类似 UDP，或者广播
// 要想可靠 就需要使用 REQ/REP
//  https://github.com/nanomsg/nanomsg/issues/283

void 
send(int sock)
{
    int to = 100;
    int ret;
    int count = 0;
    enum { send_size = 0x100, };
    char sendbuf[send_size] = { 0 };
    int retsize;
    int nrandom;

//     ret = nn_setsockopt(sock, NN_SOL_SOCKET, NN_RCVTIMEO, &to, sizeof(to));
//     if (ret < 0) {
//         fprintf(stderr, "nn_setsockopt ret=%d\n", ret);
//         return;
//     }


    nrandom = 0;
    nn_random_generate(&nrandom, sizeof(nrandom));
    for (;;)
    {
        retsize = snprintf(sendbuf, send_size, "from puber %d:%d", nrandom,count);

        // 应该调用了 nanomsg\src\protocols\pubsub\xpub.c  nn_xpub_add() 才有了订阅者 才可以被 sub 接收到
        // 但是这个过程是用线程做的 所以 nn_send() 调用返回了 但是不一定被 suber 接收到了
        //  有个好消息是 NN_STAT_CURRENT_CONNECTIONS 一定会 +1 ，可以使用这个值
        ret = nn_send(sock, sendbuf, retsize, 0);

        printf("[+] check %d puber send bytes %d/%d %s\n", count, ret,retsize,sendbuf);
        break;

        count += 1;
    }
}

void test_puber()
{
    int sock = 0;
    int ret;
    const char * url1 = "ipc:///tmp/pair.ipc";
    const char * addr = "tcp://127.0.0.1:1200";
    uint64_t cur_connections = 0;

    for (;;)
    {
        sock = nn_socket(AF_SP, NN_PUB);
        // called nn_pool_init() -> nn_worker_init()

        if (sock < 0) break;

        // puber bind ? or suber bind ?
        ret = nn_bind(sock, addr);
        //ret = nn_connect(sock, addr);
        if (ret < 0) {
            fprintf(stderr, "[!] nn_bind ret=%d\n", ret);
            break;
        }

        for (ret = 0; ret < 100; ret += 1)
        {
            cur_connections = nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS);
            if (cur_connections > 0) {
                break;
            }
            // 虽然有了这个判断 但是如果有多个 suber ，还是无法都发送到他们

            nn_sleep(1000);
        }
        if (cur_connections <= 0) {
            fprintf(stderr, "[!] not get subers\n");
            break;
        }
        send(sock);

        break;
    }

    if (sock >= 0)
    {
        nn_close(sock);
    }

}

int main()
{
    for (;;)
    {
        test_puber();
        nn_sleep(3000);
        printf("\n\n");
    }
       
    return 0;
}
