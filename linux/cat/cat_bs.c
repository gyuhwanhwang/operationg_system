#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//옵션이 없으면
//argc = 2 argv[1] = filename

// 옵션이 들어오면 
//argc = 3
// argv[1] = -opt 
// argv[2] = filename

void append(char *dst, char c) {
    char *p = dst;
    while (*p != '\0') 
	p++;
    *p = c;
    *(p+1) = '\0';
}
int flag_b = 0; int flag_s = 0;

int main(int argc, char *argv[]) {
    void view_file(FILE *);
    void fatal(char *);
    FILE *fp;

    int opt;

    if(argc == 1) { // filename 미기재
    	puts("사용법 : a.out file_name");
    	exit(0);
    }

    while((opt = getopt(argc, argv, "bs")) != -1){
		switch (opt){
			case 'b':
			flag_b = 1;
			break;
			case 's':
			flag_s = 1;
			break;
			default:
			printf("잘못된 옵션: %c\n", optopt);
			exit(1);
		}
    }
    
    *argv++;
    if((flag_b || flag_s) == 1)
	*argv++;

    do	{
    	if(*argv) { // filename이 있다면
	    puts(*argv);    // filename을 출력
	    if((fp = fopen(*argv, "r")) == NULL) // 읽기형식으로 열고
			fatal("fopen"); // 아무것도 안 가르키면 에러
	    view_file(fp);
	    fclose(fp); // 파일 종료
	}
	*argv++; // 다음 인자 
	} while(*argv);
}
void view_file(FILE *fp) {
    int c;
    int line = 1;
    int isLine = 0;	// 공백 아니면 1
    int prevBlank = 0;	// 전 라인이 공백 라인이었으면 1
    char str[1024] = "\0";
    
    while((c=getc(fp)) != EOF) {
		if( c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\v' && c != '\f'){
			isLine = 1;
		}

		if( c != '\n') {
			append(str, c);
		}
		else { 
			if( flag_b && isLine ) {  // 공백 아님
				printf("%6d\t", line++);
				printf("%s\n", str);
				prevBlank = 0;
				isLine = 0;
				str[0] = '\0';
			}
			else {
				if( flag_s && !isLine && prevBlank ){
					str[0] = '\0';
					prevBlank = 1;
					isLine = 0;
					continue;    
				}
			printf("%s\n", str);
			str[0] = '\0';
			if(!isLine)
				prevBlank = 1;
			else
				prevBlank = 0;

			isLine = 0;
			}
		}
    }
}
void fatal(char *error_name) {
    perror(error_name);
    exit(1);
}
