#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "student.h"

void dbcreate(char *argv[]);
void dbquery(char *argv[]);
void dbupdate(char *argv[]);
void dbdelete(char *argv[]);

int main(int argc, char *argv[]){
	int job_num;
	
	if(argc < 2){
		fprintf(stderr, "사용법: %s file\n", argv[0]);
		exit(1);
	}

	while(1){
		printf("\n***** 학생관리 프로그램 *****\n");
		printf("1. 학생 자료 입력\n");
		printf("2. 학생 검색\n");
		printf("3. 학생 자료 수정\n");
		printf("4. 학생 자료 삭제\n");
		printf("5. 프로그램 종료\n\n");
	

		printf("** 작업할 번호를 선택하세요 ** --> ");
		scanf("%d", &job_num);
	
		switch(job_num){
			case 1:
				dbcreate(argv);
				break;
			case 2:
				dbquery(argv);
				break;
			case 3:
				dbupdate(argv);
				break;
			case 4:
				dbdelete(argv);
				break;
			default:
				printf("프로그램을 종료합니다.\n");
				return 0;
		}
	}
}

void dbcreate(char* argv[]){
    int fd;
    struct student record;

    if((fd = open(argv[1], O_WRONLY|O_CREAT|O_EXCL, 0640)) == -1){
		perror(argv[1]);
		exit(2);
    }
	printf("\n학생 자료를 입력하세요. (학번 0 입력시 종료)\n\n");
    printf("%-8s %-8s %-4s\n", "학번", "이름", "점수");
    while(scanf("%d %s %d", &record.id, record.name, &record.score) == 3 && record.id != 0) {
		lseek(fd, (record.id - START_ID)*sizeof(record), SEEK_SET);
		write(fd, (char*)&record, sizeof(record));
    }
    close(fd);
}
void dbquery(char *argv[]){
	int fd, id;
	char c;
	struct student record;

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		perror(argv[1]);
		exit(2);
	}

	do {
		printf("\n검색할 학생의 학번 입력: ");
		if(scanf("%d", &id) == 1) {
			lseek(fd, (id-START_ID)*sizeof(record), SEEK_SET);
			if((read(fd, (char*)&record, sizeof(record)) > 0) && (record.id != 0))
				printf("학번:%d\t 이름:%s\t 점수:%d\n\n", record.id, record.name, record.score);
			else
				printf("레코드 %d 없음\n\n", id);
		}
		else {
			printf("입력 오류\n");
			getchar();
		}
		printf("계속하겠습니까?(Y/N)");
		getchar();
		scanf("%c", &c);
	} while (c == 'Y' || c == 'y');
	close(fd);
}
void dbupdate(char *argv[]){
	int fd, id;
	char c;
	struct student record;

	if ((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(2);
	}

	do {
		printf("\n수정할 학생의 학번 입력: ");
		if (scanf("%d", &id) == 1) {
			lseek(fd, (long)(id-START_ID)*sizeof(record), SEEK_SET);
			if((read(fd, (char*)&record, sizeof(record)) > 0) && (record.id != 0)) {
				printf("학번:%8d\t 이름:%4s\t 점수:%4d\n", record.id, record.name, record.score);
				printf("\n새로운 점수: ");
				scanf("%d", &record.score);
				lseek(fd, (long)-sizeof(record), SEEK_CUR);
				write(fd, (char*)&record, sizeof(record));
			}
			else
				printf("레코드 %d 없음 \n", id);
		}
		else {
			printf("입력오류\n");
			getchar();
		}
		printf("\n계속하겠습니까?(Y/N)");
		getchar();
		scanf("%c", &c);
	} while(c=='Y' || c=='y');
	
	close(fd);
}
void dbdelete(char *argv[]){
	int fd, id;
	char c;
	struct student record;
	
	if((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(2);
	}

	do {
		printf("\n삭제할 학생의 학번을 입력하세요: ");
		if(scanf("%d", &id) == 1) {
			lseek(fd, (long)(id-START_ID)*sizeof(record), SEEK_SET);
			if((read(fd, (char*)&record, sizeof(record)) > 0) && (record.id != 0)) {
				printf("\n학번:%8d\t 이름:%4s\t 점수:%4d\n", record.id, record.name, record.score);
				printf("\ndb를 삭제하시겠습니까?(Y/N)" );
				getchar();
				scanf("%c", &c);

				if(c == 'Y' | c == 'y' ){
					record.id = 0;
					lseek(fd, (long) -sizeof(record), SEEK_CUR);
					write(fd, (char*) &record, sizeof(record));
					printf("\n학번이 %d인 학생의 레코드가 삭제 되었습니다.\n", id);
				}
				else
					printf("\n취소 되었습니다.\n");
			}
			else
				printf("\n레코드 %d 없음\n", id);
		}
		else {
			printf("\n입력 오류\n");
			getchar();
		}
		
		printf("\n계속하겠습니까?(Y/N) ");
		getchar();
		scanf("%c", &c);
	} while( c == 'Y' | c == 'y' );
	close(fd);
}
