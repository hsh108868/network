// 2019101421 황서희
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 3;
void error_handling(char *bingo);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    int answer, check, select;
    int closeCount = 0;
    char start[] = "Game Start";
    char high[] = "high.";
    char low[] = "low";
    char right[] = "정답! 축하합니다 승리했습니다.";

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(0);
    }
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

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
        error_handling("accept error");

    srand(time(NULL));
    answer = rand() % 50 + 1;
    printf("%d\n", answer);

    write(clnt_sock, start, strlen(start));

    while (1)
    {
        read(clnt_sock, &select, sizeof(select));

        if (answer > select)
        {
            write(clnt_sock, high, strlen(start));
        }
        else if (answer < select)
        {
            write(clnt_sock, low, strlen(low));
        }
        else if (answer = select)
        {
            write(clnt_sock, right, strlen(right));
            break;
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