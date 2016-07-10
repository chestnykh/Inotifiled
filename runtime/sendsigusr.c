#include <signotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>



int main(int argc, char *argv)
{
	//char *dir = getenv("HOME");
	// = strcat(home,"/.ifiled.pid");
	char *pid_file_name = strcat(getenv("HOME"), "/.ifiled.pid");
	FILE *pidfile = fopen(pid_file_name, "r");
	if(!pidfile) puts("!!");
	char pid_str[5];
       	fgets(pid_str, 6, pidfile);
	for(size_t i=0; i<5; i++){
		if(!isdigit(pid_str[i])) pid_str[i] = '\0';
	}
	printf("pidstr = %s\n", pid_str);
	pid_t pid = atoi(pid_str);
	printf("pid = %d\n", pid);
	int _kill = kill(pid,SIGUSR1);
	if(_kill == -1 ){
		puts("!!");
	}
	return 0;
}
