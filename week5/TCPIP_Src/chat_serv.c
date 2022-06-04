#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100
#define MAX_CLNT 10

// 클라이언트는 읽기만 하면 되기 때문에 메인 쓰레드를 건드릴 일이 없지만, 채팅 서버는 건들여야 하므로 잘 만들어야 한다.

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT]; // 10개 클라이언트 지원
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if (argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}

	pthread_mutex_init(&mutx, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while (1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz); // 클라이언트가 들어올 때마다 새로운 소켓을 만들어 clnt_sockets 배열에 넣는다.

		pthread_mutex_lock(&mutx);
		clnt_socks[clnt_cnt++] = clnt_sock; // 배열에 clnt_sock 값을 넣는다. 하지만 공유변수이므로 쓰레드를 넣어야 함
		pthread_mutex_unlock(&mutx);

		pthread_create(&t_id, NULL, handle_clnt, (void *)&clnt_sock);		// 쓰레드 하나를 만듦
		pthread_detach(t_id);												// join은 무작정 기다리므로 detach를 사용해 종료되지 않더라도 기다리지 않고 넘어가도록 한다.
		printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr)); // 클라이언트 IP 찍어줌
	}
	close(serv_sock);
	return 0;
}

void *handle_clnt(void *arg) // 소켓 번호가 넘어온다.
{
	int clnt_sock = *((int *)arg); // clnt_sock 에 소켓 번호를 넣는다(3카피)
	int str_len = 0, i;
	char msg[BUF_SIZE];

	// 반복(클라이언트가 접속 종료 시까지 계속 반복)
	while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
		send_msg(msg, str_len); // 함수 호출

	pthread_mutex_lock(&mutx);
	for (i = 0; i < clnt_cnt; i++) // 클라이언트 연결 종료시 처리 부분
	{
		if (clnt_sock == clnt_socks[i])
		{
			for (; i < clnt_cnt; i++)
				clnt_socks[i] = clnt_socks[i + 1];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;
}

//모든 클라이언트에게 메시지 전송하는 일반 함수
void send_msg(char *msg, int len) // send to all
{
	int i;
	pthread_mutex_lock(&mutx);
	for (i = 0; i < clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}
void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}