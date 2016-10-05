#include <parse.h>
#include <handle_events.h>
#include <sys/wait.h>
#include <daemon.h>
#include <runtime_read_config.h>
#include <signotify.h>
#include <core.h>




extern int savepid();



int main(int argc, char *argv[])
{
	if(!(home_dir = getenv("HOME"))){
		LOG_ERR();
		REPORT_ERREXIT();
	}
	if(!(cwd = getcwd(cwd, PATH_MAX))){
		LOG_ERR();
		REPORT_ERREXIT();
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

	int ret;

	ret = parse_config_file((const char *)config_file);
	MAIN_CHECK_RETVAL(ret);

	ret = get_inotify_limits();
	MAIN_CHECK_RETVAL(ret);

	ret = init_inotify_actions();
	MAIN_CHECK_RETVAL(ret);

	init_pollfd_structures();

	ret = create_log_streams(0);
	MAIN_CHECK_RETVAL(ret);

	ret = init_event_struct();
	MAIN_CHECK_RETVAL(ret);

	
	ret = start_daemon();
	MAIN_CHECK_RETVAL(ret);

	print_createtime();

	ret = savepid();
	MAIN_CHECK_RETVAL(ret);

	set_sigusr1_handler();
	set_sigusr2_handler();

	for(;;){
		ret = wait_events();
		MAIN_CHECK_RETVAL(ret);
		ret = handle_events();
		MAIN_CHECK_RETVAL(ret);
	}
	return 0;
}

