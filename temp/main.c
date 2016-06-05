#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void unstrcat(char *str, size_t num)
{
	size_t len = strlen(str);
	for(size_t i=len-1; i>len-num-1; i--){
		str[i] = '\0';
	}
}


int main()
{
	char str[] = "/home/dima";
	printf("str = %s\n", str);
	printf("str = %s\n", strcat(str,"/Documents"));
	printf("len = %d\n", strlen("/Documents"));
	unstrcat(str, strlen("/Documents"));
	printf("str = %s\n", str);
	return 0;

}
