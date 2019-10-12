#include <stdio.h>
#include <time.h>

int main() {
	time_t now;
	time(&now);

	printf("%ld\n", now);
	printf(ctime(&now));

	return 0;
}
