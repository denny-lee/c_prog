#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int acpt_sock, recv_sock;
	char message[BUF_SIZE];
	int str_len, state;

	struct sockaddr_in acpt_adr, recv_adr;
	socklen_t recv_adr_sz;

	if (argc != 2) {
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	acpt_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (acpt_sock == -1)
		error_handling("socket() error!");

	memset(&acpt_adr, 0, sizeof(acpt_adr));
	acpt_adr.sin_family = AF_INET;
	acpt_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	acpt_adr.sin_port = htons(atoi(argv[1]));

	if (bind(acpt_sock, (struct sockaddr*)&acpt_adr, sizeof(acpt_adr)) == -1)
		error_handling("bind() error.");

	if (listen(acpt_sock, 5) == -1)
		error_handling("listen() error");

	recv_adr_sz = sizeof(recv_adr);

    recv_sock = accept(acpt_sock, (struct sockaddr*)&recv_adr, &recv_adr_sz);
    while(1)
    {
        str_len = recv(recv_sock, message, sizeof(message)-1, MSG_PEEK|MSG_DONTWAIT);
        if(str_len==0)
            break;
    }
    message[str_len]=0;
    printf("Buffering %d bytes: %s \n", str_len, message);

    str_len = recv(recv_sock, message, sizeof(message)-1, 0);

    message[str_len]=0;
    printf("Read again %s \n", message);
	close(acpt_sock);
    close(recv_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
