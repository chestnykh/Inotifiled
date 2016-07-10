#include <parse.h>
#include <handle_events.h>
#include <sys/wait.h>
#include <daemon.h>
#include <add_watch.h>
#include <signotify.h>
#include <core.h>


#define MAIN_CHECK_RETVAL(ret) {\
	if (ret == -1) return -1;\
}


int ret;
extern int savepid();


int track_files(void *nothing)
{
	set_sigusr1_handler();
	print_starttime();
	savepid();
	fprintf(core_log, "here\n");
	fflush(core_log);
	for(;;){
		ret = wait_events();
		MAIN_CHECK_RETVAL(ret);
		ret = handle_events();
		MAIN_CHECK_RETVAL(ret);
	}
	return -1;
}

int main(int argc, char *argv[])
{
	home_dir = getenv("HOME");
	if(!home_dir){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	int flag = 0;
	char *corelog;
	while((flag = getopt(argc, argv, "c:l:")) != -1){
		switch(flag){
			case 'c':{
				config_file = optarg;
				break;
			}
			case 'l':{
				corelog = optarg;
				break;		
			}
			case '?':{
				fprintf(stderr, "Invalid argument!\n");
				return -1;
			}
		}
	}
	int clog = open((const char *)corelog, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(clog == -1){
		LOG_ERR();
		return -1;
	}
	core_log = fdopen(clog, "a+");
	if(!core_log){
		LOG_ERR();
		return -1;
	}
	ret = parse_config_file((const char *)config_file);
	MAIN_CHECK_RETVAL(ret);

	ret = get_inotify_limits();
	MAIN_CHECK_RETVAL(ret);

	ret = init_inotify_actions();
	MAIN_CHECK_RETVAL(ret);

	init_pollfd_structures();

	ret = create_log_streams();
	MAIN_CHECK_RETVAL(ret);

	ret = init_event_struct();
	MAIN_CHECK_RETVAL(ret);

	//set_sigusr1_handler();

	//track_files();
	
	start_daemon();
	print_starttime();
	savepid();
	fprintf(core_log, "here\n");
	fflush(core_log);
	set_sigusr1_handler();
	for(;;){
		ret = wait_events();
		MAIN_CHECK_RETVAL(ret);
		ret = handle_events();
		MAIN_CHECK_RETVAL(ret);
	}
	
	/*сюда дойти не должно*/
	return 0;
}

