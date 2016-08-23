#include <daemon.h>
#include <stdio.h>
#include <handle_events.h>
#include <parse.h>
#include <signal.h>
#include <sched.h>
#include <core.h>

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



int register_finish_procedures(){
	return atexit(&print_finishtime);
}


void print_createtime()
{
	for(int i=0; i<ntf; i++){
		fprintf(tracked_files[i].log_stream, "This logfile created at:");
		print_timeinfo(tracked_files[i].log_stream);
		fprintf(tracked_files[i].log_stream, "\n");
		fflush(tracked_files[i].log_stream);
	}
}

void print_finishtime()
{
	for(int i=0; i<ntf; i++){
		fprintf(tracked_files[i].log_stream, "Daemon finished at:");
		print_timeinfo(tracked_files[i].log_stream);
		fflush(tracked_files[i].log_stream);
	}
}
