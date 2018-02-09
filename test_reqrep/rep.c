
#include <stdio.h>


// nanomsg
#include <reqrep.h>
#include <nn.h>
#include <utils/sleep.h>


#ifdef WIN32
#include <Windows.h>
#endif

uint64_t get_current_proc_id()
{
    uint64_t pid;
#ifdef WIN32
    pid = (uint64_t)GetCurrentProcessId();
#else
    pid = (uint64_t)getpid();
#endif
    return pid;
}

void nrecv(int sock)
{
    int to = 100;
    int ret;
    int count = 0;
    char * msg;

    enum{_send_size=0x100,};
    char sendbuf[_send_size];

    for (;;)
    {
        msg = 0;
        ret = nn_recv(sock, &msg, NN_MSG, NN_DONTWAIT);

        //printf("[+] check %d reper recv bytes %d:%.*s\n", count, ret, ret, (msg ? msg : ""));

        if (msg) {
            
            ret = snprintf(sendbuf, _send_size, "ack by %llu recv (%d)%.*s"
                , (unsigned long long)get_current_proc_id()
            , ret ,ret ,msg );
            ret = nn_send(sock, sendbuf, ret, NN_DONTWAIT);
            printf("[+] send ack %s ret=%d\n", sendbuf, ret);

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
        sock = nn_socket(AF_SP, NN_REP);
        if (sock < 0) break;
       

        //ret = nn_bind(sock, addr);
        ret = nn_connect(sock, addr);
        if (ret < 0) {
            fprintf(stderr, "[!] nn_connect ret=%d\n", ret);
            break;
        }

        nrecv(sock);

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
