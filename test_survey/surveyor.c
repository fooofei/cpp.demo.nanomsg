
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

typedef struct surveyor
{
    int sock;
    uint64_t respondents_count;
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
    v = 10 * 1000;
    ret = nn_setsockopt(sock, NN_SURVEYOR, NN_SURVEYOR_DEADLINE, &v, sizeof(v));
    if (ret < 0)
        goto err;
    ret = nn_bind(sock, addr);
    if (ret < 0)
        goto err;
    self->sock = sock;
    self->respondents_count = 0;
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

// Return 0 on success.
int surveyor_pub(surveyor_t * self, const void * buf, size_t size)
{
    int i;
    uint64_t resp_count;

    for (i = 0; i < 10; i += 1)
    {
        resp_count = nn_get_statistic(self->sock, NN_STAT_CURRENT_CONNECTIONS);
        if (resp_count > 0) {
            break;
        }
        nn_sleep(1000);
    }

    if (resp_count == 0)
        return -1;
    self->respondents_count = resp_count;

    return nn_send(self->sock, buf, size, 0);
}

// Return reply msg count
// Every respondent send one msg, the msg count repsents 
int surveyor_reply(surveyor_t * self)
{

    int rc;
    uint64_t i = 0;
    char * msg;
    int reply_count = 0;

    for (i = 0;;)
    {
        msg = 0;
        rc = nn_recv(self->sock, &msg, NN_MSG, 0);
        // printf("[+] recv ret=%d\n", rc);
        if (rc > 0) {
            printf("[+] recv %d:%.*s\n", rc, rc, msg);
            nn_freemsg(msg);
        }
        if (rc == -EAGAIN) {
            continue;
        }

        i += 1;
        if(i>self->respondents_count)
            break;
    }

    return 0;
}


void test_surveyor()
{
    surveyor_t sur;
    memset(&sur, 0, sizeof(sur));
    int rc;

    rc = surveyor_init(&sur);


    enum { send_size = 0x100, };
    char sendbuf[send_size] = { 0 };
    int nrandom;

    nrandom = 0;
    nn_random_generate(&nrandom, sizeof(nrandom));
    rc = snprintf(sendbuf, send_size, "from surveyor %d", nrandom);
    rc = surveyor_pub(&sur, sendbuf, rc);

    if (rc > 0)
    {
        rc = surveyor_reply(&sur);
    }

    surveyor_term(&sur);
}

int main()
{
    for (;;)
    {
        test_surveyor();
        break;
        nn_sleep(3000);
        printf("\n\n");
    }
       
    return 0;
}
