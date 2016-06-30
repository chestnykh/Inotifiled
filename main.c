#include <parse.h>
#include <handle_events.h>
#include <sys/wait.h>
#include <daemon.h>




#define MAIN_CHECK_RETVAL(ret) {\
	if (ret == -1) return -1;\
}


int main(int argc, char *argv[])
{
	int flag = 0;
	char *corelog;
	char *conf;
	while((flag = getopt(argc, argv, "c:l:")) != -1){
		switch(flag){
			case 'c':{
				conf = optarg;
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
	int ret;
	ret = parse_config_file((const char *)conf);
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
	
	start_daemon();
	print_starttime();
	for(;;){
		wait_events();
		MAIN_CHECK_RETVAL(ret);
		handle_events();
	}
	return 0;
}

