#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

void socket_init(void)
{
	int sockfd =socket(PF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	struct sockaddr_in cli;
	socklen_t len=sizeof(cli);
	while (1) {
		char buf = 0;
		scanf("%hhd", &buf);
		sendto(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&addr, sizeof(addr));

		socklen_t len=sizeof(addr);
		recvfrom(sockfd, &buf, sizeof(buf), 0, (struct sockaddr *)&addr, &len);
		if (66==buf)
			printf("success\n");
		else
			printf("fialure\n");
	}
	close(sockfd);
}

int main(void)
{
	socket_init();
	return 0;
}

