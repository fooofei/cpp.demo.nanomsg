
#include <stdio.h>
#include <string.h>

// nanomsg
#include <reqrep.h>
#include <nn.h>
#include <utils/sleep.h>
#include <utils/random.h>


// 测试发现 同一时刻只能有 1 个 rep 接到响应 不符合我们的需求
// REQ-REP 跟 PAIR 结合的例子 https://github.com/filcuc/nanomsg-multi-client-server-example

// 个人使用体验： 能用来做负载均衡， req 发送多个 请求， 分摊到各个 rep 中，作回应
//  运行多个 rep 体会到的

// http://nanomsg.org/v0.8/nn_reqrep.7.html

void n_send(int sock)
{
    int rc;
    int nrandom;
    int count;
    enum{_send_size=0x100,};
    char sendbuf[_send_size] = {0};
    char * msg;
    int flags;
    uint64_t connections;
    
    nrandom = 0;
    nn_random_generate(&nrandom, sizeof(nrandom));
    count = 0;

    for (;;)
    {
        // wait for add 
        // D:\source\GitHub\c_nanomsg_demo\nanomsg\src\protocols\utils\lb.c 
        // int nn_lb_send (struct nn_lb *self, struct nn_msg *msg, struct nn_pipe **to)
        connections = 0;
        for (;;)
        {
            connections = nn_get_statistic(sock, NN_STAT_CURRENT_CONNECTIONS);
            if (connections > 0) {
                break;
            }
            break;
            nn_sleep(1000);
        }
        

        rc = snprintf(sendbuf, _send_size, "from req %d:%d", nrandom,count);
        rc = nn_send(sock, sendbuf, rc, 0);
        printf("[+] req send ret=%d, %s\n", rc, sendbuf);
        
        flags = 0; // we will wait
        for (;;)
        {
            msg = 0;
            rc = nn_recv(sock, &msg, NN_MSG, flags);
            if (rc > 0) {
                printf("[+] req recv %.*s\n", rc, msg);
                nn_freemsg(msg);
            }
            if (rc < 0 && rc != -EAGAIN) {
                break;
            }
            flags = NN_DONTWAIT;
            
        }

        

        //break;
        nn_sleep(2000);
        count += 1;
    }

}

void test_req()
{
    int sock;
    int rc;
    const char * addr = "tcp://127.0.0.1:1200";
    int v;

    sock = nn_socket(AF_SP, NN_REQ);
    if (sock < 0) 
        goto err;

    // 这个比等待连接更有用
    v = 100;
    rc = nn_setsockopt(sock, NN_REQ, NN_REQ_RESEND_IVL, &v, sizeof(v));
    
    rc = nn_bind(sock, addr);
    if (rc < 0)
        goto err;

    n_send(sock);

    err:
    if (sock >= 0)
        nn_close(sock);
}

int main()
{
    for (;;)
    {
        test_req();
        break;
        nn_sleep(3000);
        printf("\n\n");
    }
       
    return 0;
}
