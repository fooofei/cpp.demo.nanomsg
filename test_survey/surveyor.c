
#include <stdio.h>
#include <string.h>

// nanomsg
#include <survey.h>
#include <nn.h>
#include <utils/sleep.h>
#include <utils/random.h>

// https://stackoverflow.com/questions/33363584/how-to-use-nanomsg-survey-architecture-without-while-loop

// https://github.com/nanomsg/nanomsg/issues/194

// add respondents D:\source\GitHub\c_nanomsg_demo\nanomsg\src\protocols\utils\dist.c 
//    void nn_dist_out (struct nn_dist *self, struct nn_dist_data *data)

// [nn_send silently drops message if socket is not yet connected] https://github.com/nanomsg/nanomsg/issues/878

// [Distributed Computing: The Survey Pattern] http://250bpm.com/blog:5
// [Using Survey Protocol for High Availability] http://250bpm.com/blog:20


// 尝试过 surveyor respondents  不固定 nn_bind 谁先起来谁 nn_bind 后起来的  nn_connect 结果还是失败
//   即便增加过段时间会 recreate 也不行


// 这里有说不稳定 http://gmd20.github.io/blog/nanomsg%E7%9A%84%E5%87%A0%E4%B8%AA%E6%B3%A8%E6%84%8F%E4%BA%8B%E9%A1%B9/


// 如果 surveyor respondent 都是 nn_connect 没有 nn_bind ，surveyor send 的消息 respondent recv 不到


// 如果 surveyor 永远是 nn_connect, respondent 第一个 nn_bind 接下来的都是 nn_connect，
//    这样通信 surveyor send  只有第一个即 nn_bind 的那个 respondent recv 接到消息

typedef struct surveyor
{
    int sock;
}surveyor_t;

// Return 0 on success.
int surveyor_init(surveyor_t * self)
{
    int ret;
    const char * addr = "tcp://127.0.0.1:1200";
    int sock;

    sock = nn_socket(AF_SP, NN_SURVEYOR);
    if (sock < 0)
        goto err;
    int v = 3 * 1000;
    ret = nn_setsockopt(sock, NN_SOL_SOCKET, NN_RCVTIMEO, &v, sizeof(v));
    if (ret < 0)
        goto err;
    v = 20 * 1000;
    ret = nn_setsockopt(sock, NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &v, sizeof(v));
    if (ret < 0)
        goto err;
    ret = nn_connect(sock, addr);
    if (ret < 0)
        goto err;
       
    nn_sleep(100);
    if (ret < 0)
        goto err;
    self->sock = sock;
    return 0;
err:
    if(sock>=0)
        nn_close(sock);
    return -1;
}

void surveyor_term(surveyor_t * self)
{
    nn_close(self->sock);
}



// Return reply msg count
// Every respondent send one msg, the msg count repsents 
uint64_t surveyor_reply(surveyor_t * self)
{

    int rc;
    char * msg;
    uint64_t replys = 0;
    int rc2;

    for (;;)
    {
        msg = 0;
        rc = nn_recv(self->sock, &msg, NN_MSG, 0);
        // printf("[+] recv ret=%d\n", rc);
        if (rc > 0) {
            printf("[+] recv msg (%d)%.*s\n", rc, rc, msg);
            nn_freemsg(msg);
            replys += 1;
        }
        rc2 = errno;
        if(rc<0 && errno != -EAGAIN)
            break;
    }

    return replys;
}

// Return 0 on success.
int surveyor_pub(surveyor_t * self, const void * buf, size_t size)
{
    int i;
    uint64_t resps;
    uint64_t replys;
    int rc;

    for (i = 0; i < 1000; i += 1)
    {
        resps = nn_get_statistic(self->sock, NN_STAT_CURRENT_CONNECTIONS);
        if (resps > 0) {
            break;
        }
        printf("try\n");
        nn_sleep(10);

    }

    if (resps == 0)
        return -1;

    for (i = 0; i < 5; i += 1)
    {
        rc = nn_send(self->sock, buf, size, 0);
        if (rc < 0)
            return rc;

        replys = surveyor_reply(self);
        if(replys>=resps)
            break;
        printf("[!] surveyor retry pub(%d)\n", i);
    }

    return replys>= resps ? 0 : -1;
}

void test_surveyor()
{
    surveyor_t sur;
    memset(&sur, 0, sizeof(sur));
    int rc;

    rc = surveyor_init(&sur);

    if (!(rc<0)) {
        enum { send_size = 0x100, };
        char sendbuf[send_size] = { 0 };
        int nrandom;

        nrandom = 0;
        nn_random_generate(&nrandom, sizeof(nrandom));
        rc = snprintf(sendbuf, send_size, "from surveyor %d", nrandom);
        //rc = surveyor_pub(&sur, sendbuf, rc);
        rc = nn_send(sur.sock, sendbuf, rc, 0);
        surveyor_reply(&sur);
    }

    

    if (rc < 0) {
        printf("[!] surveyor pub faield\n");
    }

    surveyor_term(&sur);
}

int main()
{
    for (;;)
    {
        test_surveyor();
        // nn_sleep(3000);
        printf("\n\n");
    }
       
    return 0;
}
