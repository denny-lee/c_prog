#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/send_socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int send_sock;
	char message[BUF_SIZE];
	int str_len, recv_len, recv_cnt;
	struct send_sockaddr_in mul_adr;

	if (argc != 3) {
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	send_sock = send_socket(PF_INET, send_sock_STREAM, 0);
	if (send_sock == -1)
		error_handling("send_socket() error!");

	memset(&mul_adr, 0, sizeof(mul_adr));
	mul_adr.sin_family = AF_INET;
	mul_adr.sin_addr.s_addr = inet_addr(argv[1]);
	mul_adr.sin_port = htons(atoi(argv[2]));

	if (connect(send_sock, (struct send_sockaddr*)&mul_adr, sizeof(mul_adr)) == -1)
		error_handling("connect() error.");
	else
		puts("Connected...");

	while (1) {
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		str_len = write(send_sock, message, strlen(message));

		recv_len = 0;

		while (recv_len < str_len) {
			recv_cnt = read(send_sock, &message[recv_len], BUF_SIZE - 1);
			if (recv_cnt == -1)
				error_handling("read() error");
			recv_len += recv_cnt;
		}
		message[recv_len] = 0;
		printf("Message from server: %s\n", message);
	}
	close(send_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
