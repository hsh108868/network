// 2019101421 황서희
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define _CRT_SECURE_NO_WARNINGS
#define SIZE 1024
void error_handling(char *answer);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    int select, count, str_len, closeCount = 0;
    char high[SIZE], low[SIZE], right[SIZE], wrong[SIZE], start[SIZE];

    if (argc != 3)
    {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");

        while (1)
    {
        printf("\n1~50의 숫자 중 하나를 선택해 주세요.");
        scanf("%d", &select);

        write(sock, &select, sizeof(select));

        if (read(sock, low, SIZE))
        {
            printf("%s", low);
        }
        else if (read(sock, high, SIZE))
        {
            printf("%s", high);
        }
        else if (read(sock, right, SIZE))
        {
            printf("%d번쨰 시도입니다.", count);
            break;
        }
        else if (count > 20)
        {
            printf("20번이 넘었습니다. 실패.");
            break;
        }

        count++;
    }
    close(sock);
    return 0;
}

void error_handling(char *answer)
{
    fputs(answer, stderr);
    fputc('\n', stderr);
    exit(1);
}
