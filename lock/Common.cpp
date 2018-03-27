#include "Common.h"

extern char door_open[MAX_CAMERA_NUM];
extern char door_close[MAX_CAMERA_NUM];

/*
char recv_door_stauts(char *msg)
{
    int i;

    if((access("/tmp/data/open",F_OK))!=-1)
    {
        for(i=0;i<PLANT_CAMERA_NUM;i++)
        {
            door_open[i] = 1;
        }
        printf("鏂囦欢 open 瀛樺湪.\n");
        return 1;
    }
    else if((access("/tmp/data/close",F_OK))!=-1)
    {
        for(i=0;i<PLANT_CAMERA_NUM;i++)
        {
            door_close[i] = 1;
        }
        printf("鏂囦欢 close 瀛樺湪.\n");
        return 1;
    }
    return 0;
}
*/

int connectLinuxSocket(const char* unixsockaddr, char* msginfo)
{
    int len;
    int clientfd = socket (AF_UNIX, SOCK_STREAM, 0); //创建本地套接字
    if(!unixsockaddr || !msginfo)
    {
        return -1;
    }
    
    if(clientfd < 0)
    {
        return -1;
    }
    struct sockaddr_un myaddr;
    printf("connectUnixSocket, clientfd = %d\n", clientfd);
    memset (&myaddr, 0, sizeof(myaddr));
    myaddr.sun_family = AF_UNIX;
    strncpy (myaddr.sun_path, unixsockaddr,strlen(unixsockaddr)); //套接字的名字，任意取
    myaddr.sun_path[0] = 0;
    if(connect (clientfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1)
    {
        printf("connect to master socket fail!!!\n");
        goto EXIT_CONN;
    }

    len = send(clientfd, msginfo, 16,0);
    if(len <= 0 )
    {
        printf("send failed to master socket !!\n");
        goto EXIT_CONN;
    }
    printf("send msg succee!\n");
    close(clientfd);
    return 0;
    EXIT_CONN:
    close(clientfd);
    return -1;
}

void* listenconnect(void* param)
{
    //char cmd[128] = {NULL};
    char cmd[128] = {0};
    pthread_detach(pthread_self());
    int listening_sockfd = socket (AF_UNIX, SOCK_STREAM, 0); //创建本地套接字
    printf("listening_sockfd = %d\n", listening_sockfd);
    fcntl(listening_sockfd, F_SETFL, O_NONBLOCK);
    struct sockaddr_un myaddr;
    memset (&myaddr, 0, sizeof(myaddr));
    myaddr.sun_family = AF_UNIX;
    strcpy (myaddr.sun_path, CAM_SERV); //套接字的名字，任意取
    //myaddr.sun_path[0] = 0;
    sprintf(cmd,"rm %s",CAM_SERV);
    system(cmd);
    bind (listening_sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)); //将套接字与它的名字绑定
    listen(listening_sockfd, 50);
    fd_set watchset, listenset, clientset;
    FD_ZERO(&clientset);
    FD_ZERO(&watchset);
    FD_ZERO(&listenset);
    //FD_SET(listening_sockfd, &listenset);
    FD_SET(listening_sockfd, &clientset);
    int max_listen = listening_sockfd + 1;
    int max_client = 0;
    int listen;
    unsigned long connect_restricted = 1000;
    unsigned long long live_conn = 0;
    struct sockaddr addr;
    socklen_t len = sizeof(addr);
    int clientfd;
    int msglen;
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
            printf("recv msg %s  clientfd %d,max_client %d\n",msginfo,clientfd,max_client);
            close(clientfd);
        }
        /*struct timeval tv;
        watchset = clientset;
        FD_SET(listening_sockfd, &watchset);
        tv.tv_sec = 3;
        tv.tv_usec = 0;
        listen = max_listen > max_client ? max_listen:max_client;
        int ret = select(listen, &watchset, NULL, NULL, &tv);
        if (ret > 0)
        {
            if (FD_ISSET(listening_sockfd, &watchset))
            {
                FD_CLR(listening_sockfd, &watchset);
                struct sockaddr addr;
                socklen_t len = sizeof(addr);
                int clientfd = accept(listening_sockfd, &addr, &len);
                if (clientfd < 0)
                {
                    perror("dh_serv_accept");
                }
                else if (connect_restricted && live_conn >= connect_restricted)
                {
                    close(clientfd);
                }
                else
                {
                    ++live_conn;
                    if (clientfd >= max_client)
                        max_client = clientfd + 1;
                    FD_SET(clientfd, &clientset);
                }
                if (ret == 1)
                    continue;
            }
            int clientfd = max_client - 1;
            for (; clientfd > 0; clientfd--)
            {

                int flags = 0;
                int len;
                char msginfo[16] {0};
                len = recv(clientfd, &msginfo, sizeof(msginfo), flags);

                if(len <= 0)
                {
                    //printf("recv failed[%d]\n", len);
                    //close(clientfd);
                    //goto EXIT_CLIENTFD;
                }
                else
                {
                printf("recv msg %s  clientfd %d,max_client %d\n",msginfo,clientfd,max_client);
                }
EXIT_CLIENTFD:
                FD_CLR(clientfd, &clientset);
                if (clientfd == max_client - 1)
                    max_client --;
                close(clientfd);
                --live_conn;
            }
        }
        else if(ret == 0)
        {
            printf("select time out\n");
            sleep(1);//出现select不阻塞线程的情况，加sleep防止死循环--by nqx
        }
        else
        {
            printf("%dselect error===%d,max_client%d, max_listen%d\n",errno,ret,max_client, max_listen);
            sleep(2);
        }*/
    }
    close(listening_sockfd);
    return 0;
}

