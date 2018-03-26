#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
void socket_init(void)
{
 	int sockfd =socket(PF_INET, SOCK_DGRAM, 0);

 	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

	struct sockaddr_in cli;
	socklen_t len=sizeof(cli);

	while (1) {
		char buf = 0;
 		recvfrom(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&cli, &len);
		printf("num = %hhd\n", buf);
		buf = 66;
		sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&cli, len);
	}
 	close(sockfd);
}

int main(void)
{
	socket_init();
	return 0;
}

