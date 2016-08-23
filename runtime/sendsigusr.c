#include <signotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>



int main(int argc, char *argv)
{
	char *pid_file_name = strcat(getenv("HOME"), "/.ifiled.pid");
	FILE *pidfile = fopen(pid_file_name, "r");
	if(!pidfile){
		fprintf(stderr, "$HOME/.ifiled.pid: no such file\n");
		return -1;
	}
	char pid_str[5];
       	fgets(pid_str, 6, pidfile);
	pid_t daemon_pid = atoi(pid_str);
	if(kill(daemon_pid, SIGUSR1) == -1){
		perror("kill");
	}
	
	return 0;
}
