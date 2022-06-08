#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20
#define MAX_CLNT 10

void *send_msg(void *arg);
void *recv_msg(void *arg);
void error_handling(char *msg);

char name[NAME_SIZE] = "[DEFAULT]"; //전역 공유
char msg[BUF_SIZE];					// 두 쓰레드간 같이 쓰기 위해 전역변수로 선언함
char *userlist[MAX_CLNT];

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void *thread_return;
	if (argc != 4)
	{
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	sprintf(name, "[%s]", argv[3]); // 애초에 [name]으로 나오게 한다 (sprintf == 출력하는 값을 변수에 저장함)
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2])); // connect 정보 준비

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error"); // connect 는 메인 쓰레드에서 한다

	pthread_create(&snd_thread, NULL, send_msg, (void *)&sock);
	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock); // 송수신 스레드 만들기
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	close(sock);
	return 0;
}

void *send_msg(void *arg) // 송신하는 쓰레드가 입력할 함수 arg : 소켓번호값
{
	int sock = *((int *)arg);			 // 소켓번호값을  형변환 한다
	char name_msg[NAME_SIZE + BUF_SIZE]; // 최대 100 길이의 채팅 메시지 (20은 이름(NAME_SIZE))
	while (1)
	{

		sprintf(name_msg, "%s %s", name, msg);
		write(sock, name_msg, strlen(name_msg));

		fgets(msg, BUF_SIZE, stdin);					// 버프사이즈만큼 키보드로 입력받아 msg 배열(전역변수)에 넣는다
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) // 메시지 내용이 q라면 클라이언트 접속을 끊음
		{
			close(sock);
			exit(0);
		}
		// name msg == [LEE] 채팅메시지
	}
	return NULL; // 처음부터 [name] 채팅 형식으로 보내자
}

void *recv_msg(void *arg) // read thread main
{
	int sock = *((int *)arg); // 소켓번호값을 형변환 한다
	char name_msg[NAME_SIZE + BUF_SIZE];
	int str_len;
	while (1)
	{
		str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1); // read 값을 저장한다
		if (str_len == -1)										  // read의 값이 정상적이지 않으면 (== 1이 아니면)
			return (void *)-1;
		name_msg[str_len] = 0;	 // 0 = 문자의 끝을 의미한다
		fputs(name_msg, stdout); // 화면에 출력
	}
	return NULL;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
