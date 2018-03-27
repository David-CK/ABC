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
#define LOCK_TEST	"./lock_test"

int connectUnixSocket(const char* unixsockaddr, char* msginfo)
{
	if(!unixsockaddr || !msginfo)
		return -1;

	int clientfd = socket (PF_UNIX, SOCK_STREAM, 0);
	if(clientfd < 0)
		return -1;

	struct sockaddr_un myaddr;

	//printf("connectUnixSocket, clientfd = %d\n", clientfd);

	memset (&myaddr, 0, sizeof(myaddr));
	myaddr.sun_family = PF_UNIX;
	strncpy (myaddr.sun_path, unixsockaddr,strlen(unixsockaddr));
	//myaddr.sun_path[0] = 0;

	if(connect (clientfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) == -1)
	{
		//printf("connect to cam servrer socket fail!!!\n");
		goto EXIT_CONN;
	}

	int len = send(clientfd, msginfo, 16,0);
	if(len <= 0 )
	{
		//printf("send failed to cam servrer  socket !!\n");
		goto EXIT_CONN;
	}
	//printf("send msg succee!\n");
	close(clientfd);
	return 0;
EXIT_CONN:
	close(clientfd);
	return -1;
}

void main(int argc, char *argv[])
{
	char msg[16];
	sprintf(msg,"hello_fridge!");

	//while(1)
	{
		//connectUnixSocket(SERVER_TEST, msg);
		connectUnixSocket(LOCK_TEST, msg);
		printf("LOCK_CMD: OPEN_LOCK\n");
//		sleep(1);
	}
}

