#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>

int main() {
	uid_t userId;
	struct passwd * userPw;

	userId = getuid();
	userPw = getpwuid(userId);

	printf("Username : %s\n", userPw -> pw_name);
	printf("User ID  : %6d\n", userPw -> pw_uid);
	printf("Home dir : %s\n", userPw -> pw_dir);

	return 0;
}

