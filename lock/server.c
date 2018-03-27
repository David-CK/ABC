#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_TEST	"./server_test"

int main(int argc, char *argv[])
{
    char cmd[128] = {0};

    struct sockaddr_un myaddr;

    int listening_sockfd = socket (AF_UNIX, SOCK_STREAM, 0);
    //printf("test_server_sockfd = %d\n", listening_sockfd);

    fcntl(listening_sockfd, F_SETFL, O_NONBLOCK);

    memset (&myaddr, 0, sizeof(myaddr));

    myaddr.sun_family = AF_UNIX;
    strcpy (myaddr.sun_path, SERVER_TEST);
    //myaddr.sun_path[0] = 0;

    sprintf(cmd,"rm %s",SERVER_TEST);
    system(cmd);

    bind (listening_sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr));
    listen(listening_sockfd, 50);

    fd_set watchset, listenset, clientset;
    FD_ZERO(&clientset);
    FD_ZERO(&watchset);
    FD_ZERO(&listenset);
    FD_SET(listening_sockfd, &clientset);

    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    int clientfd;

    char msginfo[16]={0};

    while (1)
    {
        clientfd = accept(listening_sockfd, &addr, &len);
        if (clientfd <0 )
        {
            usleep(100*1000);
            continue;
        }
        len = recv(clientfd, &msginfo, sizeof(msginfo), 0);

        if(len > 0)
        {
            //printf("recv msg %s  clientfd %d\n",msginfo,clientfd);
            printf("LOCK_STATUS: %s\n",msginfo);
            close(clientfd);
        }
    }

    close(listening_sockfd);
    return 0;
}

