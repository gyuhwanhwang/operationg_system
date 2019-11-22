#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define SIZE 1024
int background = 0;
int work_num = 1;

main()
{
    int chpid, exitcode;
    int i = 0, n = 0;
    int j = 0;
    int buff_count = 1;

    char com[SIZE];
    char *pri[10];
    char com_tmp[SIZE];
    char buff[100];
    char buff_tmp[100];

    while (1)
    {
        bzero(com, SIZE);
        bzero(pri, sizeof(pri));
        printf("shell > ");
        fgets(com, sizeof(com), stdin);
        com[strlen(com) - 1] = '\0'; // 개행 제거
        if (!strcmp(com, "exit"))
        {
            exit(0);
        }

        // com를 com_tmp로 복사
        strcpy(com_tmp, com);

        char *ptr = strtok(com_tmp, ";");
        int index = 0; // 저장된 명령어 갯수
        while (ptr != NULL)
        {
            index++;
            ptr = strtok(NULL, ";");
        }

        // 명령어 갯수만큼 실행
        for (i = 0; i < index; i++)
        {
            strcpy(com_tmp, com);
            ptr = strtok(com_tmp, ";");

            // i+1 번째 명령어
            for (n = 0; n < i; n++)
                ptr = strtok(NULL, ";");

            strcpy(buff, ptr);
            strcpy(buff_tmp, ptr);

            if (strtok(buff_tmp, " ") != NULL)
                for (buff_count = 1; strtok(NULL, " ") != NULL; buff_count++)
                    ;

            pri[0] = strtok(buff, " ");
            for (n = 1; n < buff_count; n++)
            {
                pri[n] = strtok(NULL, " ");

                if (strcmp(pri[n], "&") == 0)
                {
                    background = 1;
                    buff_count -= 1;
                }
            }
            pri[buff_count] = NULL;

            //실행부
            if (j = redirction(pri) == 1)
                continue;
            else if (j != 0)
                break;

            if (forkcom(pri) == 0)
                continue;
            else
                break;
        }
    }
}

int forkcom(char **pri)
{
    int chpid;
    int exitcode;

    if ((chpid = fork()) < 0)
    {
        perror("Can't fork process");
        return -1;
    }
    else if (chpid == 0)
    {
        if (background == 1)
        {
            signal(SIGINT, SIG_IGN);
            printf("[%d] %d\n", work_num++, getpid());
        }
        else
        {
            signal(SIGINT, SIG_DFL);
        }
        execvp(pri[0], pri);
        exit(0);
    }
    else
    {
        if (background == 1)
        {
            background = 0;
            return 0;
        }
        else
        {
            chpid = waitpid(chpid, &exitcode, 0);
            usleep(10);
            return 0;
        }
    }
}

int redirction(char **pri)
{
    int i = 0;
    int fd;
    int out = 0, in = 0;
    int savstdout = dup(1);
    int savstdin = dup(0);

    for (i = 1; pri[i] != NULL; i++)
    {
        if (!strcmp(pri[i], ">"))
        {
            out = 1;
            break;
        }
        else if (!strcmp(pri[i], "<"))
        {
            in = 1;
            break;
        }
    }

    if (out == 1)
    {
        if (pri[i + 1] == NULL)
        {
            return -1;
        }
        else
        {
            if ((fd = open(pri[i + 1], O_RDWR | O_CREAT | S_IROTH, 0644)) == -1)
            {
                perror(pri[i + 1]);
                return -1;
            }
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
        pri[i] = NULL;
        pri[i + 1] = NULL;

        forkcom(pri);
        dup2(savstdout, 1);
        return 1;
    }

    if (in == 1)
    {
        if (!pri[i + 1])
        {
            return -1;
        }
        else
        {
            if ((fd = open(pri[i + 1], O_RDONLY)) == -1)
            {
                perror(pri[i + 1]);
                return -1;
            }
        }
        dup2(fd, STDIN_FILENO);
        close(fd);

        pri[i] = NULL;
        pri[i + 1] = NULL;
        forkcom(pri);
        dup2(savstdin, 0);
        return 1;
    }

    return 0;
}
