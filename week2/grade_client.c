// 2019101421 황서희
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define _CRT_SECURE_NO_WARNINGS
void error_handling(char *sum);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    int math, english, korean, sum = 0;
    char grade;

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

    printf("국어 성적을 입력해 주세요.\n");
    scanf("%d", &korean);

    sum += korean;

    printf("영어 성적을 입력해 주세요.\n");
    scanf("%d", &english);

    sum += english;

    printf("수학 성적을 입력해 주세요.\n");
    scanf("%d", &math);

    sum += math;

    write(sock, &sum, sizeof(sum));
    read(sock, &grade, 1);

    printf("학점: %c", grade);
    close(sock);
    return 0;
}

void error_handling(char *sum)
{
    fputs(sum, stderr);
    fputc('\n', stderr);
    exit(1);
}