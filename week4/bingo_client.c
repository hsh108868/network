// 2019101421 황서희
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define _CRT_SECURE_NO_WARNINGS
void error_handling(char *answer);
void shuffle(char *arr);
int CheckBingo(char arr[5][5]);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char arr[25], bingo[5][5];
    int count, plbc, cpbc = 0;
    int row, column, number;

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

    for (int i = 0; i < 25; i++)
    {
        arr[i] = i + 1;
    }

    shuffle(arr);

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            bingo[i][j] = arr[count];
            count++;
        }
    } //빙고 판 초기화

    printf("\n빙고를 시작합니다. \n");

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%d\t", bingo[i][j]);
        }
        printf("\n");
    } // 빙고 판 출력

    while (1)
    {

        printf("값을 입력해 주세요.\n");
        scanf("%d", &number);
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (bingo[i][j] == number)
                {
                    bingo[i][j] = 'X';
                }
            }
        }

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (bingo[i][j] == 'X')
                {
                    printf("%c\t", bingo[i][j]);
                }
                else
                {
                    printf("%d\t", bingo[i][j]);
                }
            }
            printf("\n");
        }

        write(sock, &number, sizeof(number));

        plbc = CheckBingo(bingo);
        write(sock, &plbc, sizeof(plbc));
        read(sock, &cpbc, sizeof(cpbc));

        if (plbc >= 5)
        {
            printf("\n승리했습니다. 축하합니다!\n");
            break;
        }
        else if (cpbc >= 5)
        {
            printf("\n상대방이 먼저 빙고를 달성했습니다.\n");
            break;
        }
        else if (plbc >= 5 && cpbc >= 5)
        {
            printf("\n비겼습니다.\n");
            break;
        }

        printf("\n상대방이 번호를 선택 중 입니다. 잠시만 기다려 주세요...\n");

        read(sock, &number, sizeof(number));

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (bingo[i][j] == number)
                {
                    bingo[i][j] = 'X';
                }
            }
        }

        plbc = CheckBingo(bingo);
        write(sock, &plbc, sizeof(plbc));

        if (plbc >= 5)
        {
            printf("\n승리했습니다. 축하합니다!\n");
            break;
        }
        else if (cpbc >= 5)
        {
            printf("\n상대방이 먼저 빙고를 달성했습니다.\n");
            break;
        }
        else if (plbc >= 5 && cpbc >= 5)
        {
            printf("\n비겼습니다.\n");
            break;
        }

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 5; j++)
            {
                if (bingo[i][j] == 'X')
                {
                    printf("%c\t", bingo[i][j]);
                }
                else
                {
                    printf("%d\t", bingo[i][j]);
                }
            }
            printf("\n");
        }
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

void shuffle(char *arr)
{
    srand(time(NULL));
    int temp;
    int random;
    for (int i = 0; i < 24; i++)
    {
        random = rand() % 24 + 1;
        temp = arr[i];
        arr[i] = arr[random];
        arr[random] = temp;
    }
}

int CheckBingo(char arr[5][5])
{
    int hor, vert, diagLeft, diagRight, plbc = 0;

    for (int i = 0; i < 5; i++)
    {
        hor = 0;
        vert = 0;

        for (int j = 0; j < 5; j++)
        {
            if (arr[i][j] == 'X')
            {
                hor++;
            }
            if (arr[j][i] == 'X')
            {
                vert++;
            }
        }

        if (hor == 5)
        {
            plbc++;
        }
        if (vert == 5)
        {
            plbc++;
        }
    }

    for (int i = 0; i < 5; i++)
    {
        if (arr[i][i] == 'X')
            ++diagLeft;
    }
    if (diagLeft == 5)
    {
        ++plbc;
    }
    for (int i = 0; i < 5; i++)
    {
        if (arr[4 - i][i] == 'X')
            ++diagRight;
    }
    if (diagRight == 5)
    {
        ++plbc;
    }

    return plbc;
}