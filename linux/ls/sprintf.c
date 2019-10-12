#include <stdio.h>

int main(){
	char s[100];
	char *str = "hello world";

	sprintf(s,"ex) %s\n", str);
	printf("%s \n", s);

	return 0;
}
