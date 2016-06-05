#include <util.h>


void unstrcat(char *str, size_t num)
{
	size_t len = strlen(str);
	for(size_t i=len-1; i>len-num-1; i--){
		str[i] = '\0';
	}
}
