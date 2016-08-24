#include <signotify.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>



int send_sigusr1(pid_t pid)
{
	if(kill(pid, SIGUSR1) == -1){
		perror("kill");
		return -1;
	}
	return 0;
}

int send_sigusr2(pid_t pid)
{
	if(kill(pid, SIGUSR2) == -1){
		perror("kill");
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc > 2){
		fprintf(stderr, "Too many arguments to manage the daemon.\n");
		return -1;
	}
	if(argc == 1)
		return 0;

	char *pid_file_name = strcat(getenv("HOME"), "/.ifiled.pid");
	FILE *pidfile = fopen(pid_file_name, "r");
	if(!pidfile){
		perror("fopen");
		fprintf(stderr, "$HOME/.ifiled.pid: no such file\n");
		return -1;
	}
	char pid_str[5];
       	fgets(pid_str, 6, pidfile);
	pid_t daemon_pid = atoi(pid_str);
	if(!strcmp((const char *)argv[1],"-r")){
		if(!send_sigusr1(daemon_pid))
			return 0;
		return -1;
	}
	if(!strcmp((const char *)argv[1],"-k")){
		if(!send_sigusr2(daemon_pid))
			return 0;
		return -1;
	}
	else{
		fprintf(stderr, "Unknown option.\n");
		return -1;
	}
	return 0;
}
