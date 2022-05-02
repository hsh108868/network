// 2019101421 황서희
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 3;
void error_handling(char *score);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    int result, score = 0;
    char grade;

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

    read(clnt_sock, &score, sizeof(score));

    result = score / 3;

    if (result >= 90)
    {
        grade = 'A';
    }
    else if (result >= 80)
    {
        grade = 'B';
    }
    else if (result >= 70)
    {
        grade = 'C';
    }
    else if (result >= 60)
    {
        grade = 'D';
    }
    else
    {
        grade = 'F';
    }
    write(clnt_sock, &grade, sizeof(grade));
    close(clnt_sock);
    close(serv_sock);
    return 0;
}

void error_handling(char *score)
{
    fputs(score, stderr);
    fputc('\n', stderr);
    exit(1);
}
