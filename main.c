#include <parse.h>
#include <handle_events.h>
#include <sys/wait.h>
#include <create_daemon.h>
#include <util.h>



char start_wd[PATH_MAX];



int prepare_poll_data()
{
	pid_t pid = fork();
	if(pid == -1){
		perror("fork");
		return -1;
	}
	if(!pid){
		if(chdir("./const_pollfd") == -1){
			perror("chdir");
			return -1;
		}
		//unstrcat(start_wd, strlen("/const_pollfd"));
		int ret = execl("./npfd", "npfd", "../watching.conf", NULL);
		if(ret == -1){
			perror("execl");
			return -1;
		}
		exit(EXIT_SUCCESS);
	}
	int completion_status;
	pid_t child_info = waitpid(pid, &completion_status, 0);
	bool is_exited = WIFEXITED(completion_status);
	if(!is_exited){
		return -1;
	}
	//printf("exit code = %d\n", WEXITSTATUS(completion_status));
	return 0;
}




int main(int argc, char *argv[])
{
	//char cur_wd[PATH_MAX];
	if(!getcwd(start_wd, PATH_MAX)) return -1;
	//start_daemon();
	/*
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
	//if(chdir("/") == -1){
	//	fprintf(core_log, "Failed to change working directory to root!\n");
	//	return -1;
	//}
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
	*/
	prepare_poll_data();
	system("bash kostyl.sh");
	//unstrcat(start_wd, strlen("/kostyl.sh"));

	int clog = open("ifile.log", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(clog == -1){
		LOG_ERR();
		return -1;
	}
	core_log = fdopen(clog, "a+");
	if(!core_log){
		LOG_ERR();
		return -1;
	}
	parse_config_file("watching.conf");
	//unstrcat("watching.conf");
	get_inotify_limits();
	init_inotify_actions();
	init_pollfd_structures();
	create_log_streams();
	init_event_struct();
	for(;;){
		wait_events();
		handle_events();
	}
	return 0;
}

