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
void shuffle(char *arr);
int CheckBingo(char arr[5][5]);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    char arr[25], bingo[5][5];
    int count, plbc, cpbc = 0;
    int select, number, row, column;

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
    }

    printf("\n빙고를 시작합니다. \n");

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%d\t", bingo[i][j]);
        }
        printf("\n");
    }

    while (1)
    {
        printf("\n상대방이 번호를 선택 중 입니다. 잠시만 기다려 주세요...\n");

        read(clnt_sock, &number, sizeof(number));
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

        cpbc = CheckBingo(bingo);
        write(clnt_sock, &cpbc, sizeof(cpbc));
        read(clnt_sock, &plbc, sizeof(plbc));

        if (plbc >= 5)
        {
            printf("\n상대방이 먼저 빙고를 달성했습니다.\n");
            break;
        }
        else if (cpbc >= 5)
        {
            printf("\n승리했습니다. 축하합니다!\n");
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

        printf("\n선택하고자 하는 숫자의 값을 입력하려면 1, 위치를 입력하려면 2를 입력해 주세요.\n");
        scanf("%d", &select);

        if (select == 1)
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
        }
        else if (select == 2)
        {
            printf("위치를 2 2와 같이 입력해 주세요.(1~5)\n");
            scanf("%d %d", &row, &column);
            number = bingo[row - 1][column - 1];
            bingo[row - 1][column - 1] = 'X';
        }
        else
        {
            printf("제대로 된 값을 입력해 주세요.\n");
            continue;
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

        write(clnt_sock, &number, sizeof(number));

        cpbc = CheckBingo(bingo);
        write(clnt_sock, &cpbc, sizeof(cpbc));
        read(clnt_sock, &plbc, sizeof(plbc));

        if (plbc >= 5)
        {
            printf("\n상대방이 먼저 빙고를 달성했습니다.\n");
            break;
        }
        else if (cpbc >= 5)
        {
            printf("\n승리했습니다. 축하합니다!\n");
            break;
        }
        else if (plbc >= 5 && cpbc >= 5)
        {
            printf("\n비겼습니다.\n");
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
    int hor, vert, diagLeft, diagRight, cpbc = 0;

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
            cpbc++;
        }
        if (vert == 5)
        {
            cpbc++;
        }
    }

    for (int i = 0; i < 5; i++)
    {
        if (arr[i][i] == 'X')
            ++diagLeft;
    }
    if (diagLeft == 5)
    {
        ++cpbc;
    }
    for (int i = 0; i < 5; i++)
    {
        if (arr[4 - i][i] == 'X')
            ++diagRight;
    }
    if (diagRight == 5)
    {
        ++cpbc;
    }

    return cpbc;
}
