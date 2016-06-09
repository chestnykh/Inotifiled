#include <parse.h>
#include <handle_events.h>
#include <sys/wait.h>
#include <daemon.h>
#include <util.h>



char start_wd[PATH_MAX];


#define MAIN_CHECK_RETVAL(ret) {\
	if (ret == -1) return -1; \ 
}


int main(int argc, char *argv[])
{
	int ret;
	//char cur_wd[PATH_MAX];
	if(!getcwd(start_wd, PATH_MAX)) return -1;


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

	ret = parse_config_file("watching.conf");
	MAIN_CHECK_RETVAL(ret);
	//unstrcat("watching.conf");

	ret = get_inotify_limits();
	MAIN_CHECK_RETVAL(ret);

	ret = init_inotify_actions();
	MAIN_CHECK_RETVAL(ret);

	init_pollfd_structures();

	ret = create_log_streams();
	MAIN_CHECK_RETVAL(ret);

	ret = init_event_struct();
	MAIN_CHECK_RETVAL(ret);
	
	start_daemon();
	print_starttime();
	for(;;){
		wait_events();
		MAIN_CHECK_RETVAL(ret);
		handle_events();
	}
	return 0;
}

