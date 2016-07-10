#include <stdio.h>
#include <parse.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern char *home_dir;


int savepid()
{
	char *path_to_pidfile;
	path_to_pidfile =  strcat(home_dir,"/.ifiled.pid");
	int ifiled_pid = open(path_to_pidfile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(ifiled_pid == -1){
		LOG_ERR();
		return -1;
	}
	FILE *pidfile = fdopen(ifiled_pid, "w");
	if(!pidfile){
		LOG_ERR();
		return -1;
	}
	fprintf(pidfile, "%d", getpid());
	fflush(pidfile);
	if(fclose(pidfile)) LOG_ERR();
	return 0;
}
