#include <stdio.h>
#include <unistd.h>
#include <grp.h>
#include <sys/types.h>

int main() {
	gid_t grId;
	struct group * grEntry;

	grId = getgid();
	grEntry = getgrgid(grId);

	printf("Group Name : %s\n", grEntry -> gr_name);
	printf("Group ID   : %6d\n", grEntry -> gr_gid);

	return 0;
}
