#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define  LEFT_STAR 1
#define  MIDDLE_STAR 2
#define  RIGHT_STAR 3

#define  ATIME 1
#define  CTIME 2
#define  MTIME 3

#define TYPE_D 1
#define TYPE_F 2

#define  ON 1
#define  OFF 0

time_t  now;
int option = 0;
int extention = 0;
int typeOpt = 0;

int main(int argc, char *argv[])
{
    void    check_match(DIR  *, char *, char *, char *);
    void    fatal(char *);
    char    path_name[BUFSIZ];
    char    object[BUFSIZ];
    char    user_time[10];
    struct  stat source_stat;
    DIR *dir;
    int count, i = 2;
    char * temp = NULL;

    if(argc < 3)
        {
        puts("사용법 : % a.out directory_name pattern");
        exit(0);
        }

    printf("argc = %d\n", argc); // test
    for(i; i < argc; i++){ // 옵션처리
        if(temp = strchr(argv[i], '-'))
            *temp++;
        else 
            continue;
        if(!strcmp("atime", temp ))
            option = ATIME;
        if(!strcmp("ctime", temp ))
            option = CTIME;
        if(!strcmp("mtime", temp ))
            option = MTIME;
    }

    i = 2;
    for(i; i < argc; i++){ // type처리
        if(temp = strchr(argv[i], '-'))
            *temp++;
        else 
            continue;
        if(!strcmp("type", temp )){
            if(!strcmp("d", argv[++i]))
                typeOpt = TYPE_D;
            else if (!strcmp("f", argv[i]))
                typeOpt = TYPE_F;
            else 
                fatal("type option error");
            break;
        }

    }

    time(&now);
    strcpy(object, argv[argc - 1]); // 마지막
    strcpy(user_time, argv[argc - 2]); // 마지막 전

    if(strchr(object, '*')) // * 마지막 인자에 * 존재하면
        {
        extention = LEFT_STAR;
        strcpy(object, (char *)strrchr(object, '*')+1); // *다음부터를 object에 다시 복사
        }

    *argv++; // argv[0] = 폴더 , [1] 시간 [2] 패턴
    for(count = 0;count < argc - 3;count++, *argv++)
    {
        if(stat(*argv, &source_stat) < 0)
            break;
    
        if(!S_ISDIR(source_stat.st_mode & S_IFMT)) {
            printf("%s is not directory\n", *argv);
            exit(0);
        }

        if((dir = opendir(*argv)) == NULL)
            fatal("opendir");
        strcpy(path_name, *argv);
        check_match(dir, object, path_name, user_time);
    } /* for */
}

void
check_match(DIR *dir, char *pattern, char *path_name, char *user_time)
{
    void    fatal(char *);
    struct  dirent  *directory;
    struct  stat    file_stat;
    char    path_name_buff[BUFSIZ];
    int source_size, pattern_size;
    int gab;
    int current, old;
    int pattern_found = OFF;

    while(directory = readdir(dir))
    {
        if(!strncmp(directory->d_name, ".", 1))
            continue;
        if(directory->d_ino == 0)
            continue;

        sprintf(path_name_buff, "%s/%s", path_name, directory->d_name);
        if(stat(path_name_buff, &file_stat) < 0)
            fatal("stat");
        

        if(S_ISDIR(file_stat.st_mode & S_IFMT)) {
            DIR *imsi_dir;

            if((imsi_dir = opendir(path_name_buff)) < 0)
    //      if((imsi_dir = opendir(directory->d_name)) < 0)
                fatal("opendir");
            check_match(imsi_dir, pattern, path_name_buff, user_time);
        }

        if(typeOpt == 1) { // only dir print
            if(S_ISREG(file_stat.st_mode & S_IFMT))
                continue;
        }
        else if (typeOpt == 2) { // only file print
            if(S_ISDIR(file_stat.st_mode & S_IFMT))
                continue;
        }

        switch(extention)
        {
        case    LEFT_STAR : 
            source_size = strlen(directory->d_name);
            pattern_size = strlen(pattern);

            gab = source_size - pattern_size;

            if(gab < 0) continue;

            if(!strcmp(directory->d_name + gab, pattern))
                {
                pattern_found = ON;
                break;
                }
            else    continue;
        case    MIDDLE_STAR : break;
        case    RIGHT_STAR  : break;
        }

        switch(option)
        {
            case    ATIME :
                current = now/86400;
                old = file_stat.st_atime/86400;

                if(strchr(user_time, '-'))
                {
                    if((current - old) <= atoi(strchr(user_time, '-')+1))
                        puts(path_name_buff);
                    
                } else  if((current - old) == atoi(user_time) &&
                    pattern_found == ON)
                    printf("%s\n", path_name_buff);
                break;
            
            case    CTIME :
                current = now/86400;
                old = file_stat.st_ctime/86400;

                if(strchr(user_time, '-'))
                {
                    if((current - old) <= atoi(strchr(user_time, '-')+1))
                        puts(path_name_buff);
                    
                } else  if((current - old) == atoi(user_time) &&
                    pattern_found == ON)
                    printf("%s\n", path_name_buff);
                break;
            
            case    MTIME : 
                current = now/86400;
                old = file_stat.st_mtime/86400;

                if(strchr(user_time, '-'))
                {
                    if((current - old) <= atoi(strchr(user_time, '-')+1))
                        puts(path_name_buff);
                    
                } else  if((current - old) == atoi(user_time) &&
                    pattern_found == ON)
                    printf("%s\n", path_name_buff);
                break;
        } /* switch */
    } /* while() */

    closedir(dir);
}

void
fatal(char *error_name)
{
    perror(error_name);
    exit(1);
}
