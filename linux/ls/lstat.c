#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(){
	struct stat buf;
	struct stat * st = &buf;
	lstat("/home/B513108/os/hw4/a.out", &buf);

	printf("uid : %6d \n", st->st_uid);
	printf("gid : %6d \n", st->st_gid);
	printf("%-6d bytes\n", st->st_size);

	return 0;
}
