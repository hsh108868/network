// 2019101421 황서희
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> //sleep()함수
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#define BUF_SIZE 3;
void error_handling(char *bingo);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    int pipe1[2], pipe2[2], pipe3[2], pipe4[2], user[2];
    int number, state;
    int clnt_cnt = 0; // 접속된 클라이언트 수
    char wait[] = "상대방의 접속을 기다리는 중입니다.";
    pid_t pid;
    struct sigaction act;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(0);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("bind error");

    if (listen(serv_sock, 5) == -1)
        error_handling("listen error");

    pipe(pipe1), pipe(pipe2), pipe(pipe3), pipe(pipe4);

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1)
            error_handling("accept error");

        if (clnt_sock == -1)
            continue;
        else
            puts("new client connected...");

        ++clnt_cnt;
        answ = rand() % 24 + 1;

        pid = fork();

        if (pid == -1)
        {
            close(clnt_sock);
            continue;
        }

        if (pid > 0)
        {
            if (clnt_cnt < 2)
            {
                user[clnt_cnt - 1] = clnt_sock;
                if (clnt_cnt == 1)
                {
                    write(pipe1[1], wait, sizeof(wait));
                }
                continue;
            }
            close(clnt_sock);
            while (1) // 게임 시작
            {
                read(pipe1[0], &number, sizeof(number));
                write(pipe4[1], &number, sizeof(number)); // 자식1 -> 자식2
                read(pipe3[0], &number, sizeof(number));
                write(pipe2[1], &number, sizeof(number)); //자식2 -> 자식1            }
            }
        }
        else if (pid == 0) // 자식 프로세스
        {
            close(serv_sock);
            while (1)
            {
                if (clnt_cnt % 2 == 1)
                {
                    read(clnt_sock, &number, sizeof(number));
                    write(pipe1[1], &number, sizeof(number)); // 부모에게 보내기
                    read(pipe2[0], &number, sizeof(number));
                    write(clnt_sock, &number, sizeof(number));
                }
                else if (clnt_cnt % 2 == 0)
                {
                    read(clnt_sock, &number, sizeof(number));
                    write(pipe3[1], &number, sizeof(number)); // 부모에게 보내기
                    read(pipe4[0], &number, sizeof(number));
                    write(clnt_sock, &number, sizeof(number));
                }
            }
            close(clnt_sock);
            return 0;
        }
    }

    close(clnt_sock);
    close(serv_sock);

    return 0;
}

void error_handling(char *bingo)
{
    fputs(bingo, stderr);
    fputc('\n', stderr);
    exit(1);
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
} // 좀비 프로세스 생성 막기