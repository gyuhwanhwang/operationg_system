#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int main(){
	DIR *dp;
	struct dirent *d;

	mkdir("testDir1", 0755);
	mkdir("testDir2", 0755);

	dp = opendir(".");
	if (NULL != dp) {
		while(d = readdir(dp))
			printf("%s\n", d->d_name);
		
		closedir(dp);
	}
}
