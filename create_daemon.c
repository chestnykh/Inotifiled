#include <create_daemon.h>
#include <stdio.h>

int start_daemon()
{
	pid_t daemon_pid = fork();
	if(daemon_pid == -1){
		fprintf(core_log, "Failed to start daemon!\n");
		fflush(core_log);
		return -1;
	}
	if(daemon_pid != 0) exit(EXIT_SUCCESS);
	if(setsid() == -1){
		fprintf(core_log, "Failed to create own daemon session!\n");
		return -1;
	}
	if(chdir("/") == -1){
		fprintf(core_log, "Failed to change working directory to root!\n");
		return -1;
	}
	int cl;
	cl = fclose(stdin);
	if(cl == EOF){
		fprintf(core_log, "Failed to close STDIN!\n");	
	}
	cl = fclose(stdout);
	if(cl == EOF){
		fprintf(core_log, "Failed to close STDOUT!\n");	
	}
	cl = fclose(stderr);
	if(cl == EOF){
		fprintf(core_log, "Failed to close STDERR!\n");	
	}
	return 0;
}
