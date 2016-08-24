#include <runtime_read_config.h>
#include <alloca.h>
#include <signotify.h>
#include <core.h>
#include <parse.h>
#include <handle_events.h>
#include <sys/stat.h>
#include <daemon.h>


int is_file_tracking(char *filename)
{
	for(unsigned int i=0; i<ntf; i++){
		if(!strcmp(filename,tracked_files[i].path)){
			struct_num = i;
			return struct_num;
		}
	}
	return -1;	
}


/* 
 *
 *
*/
void print_starttime_in_new_logfiles()
{
	struct stat filestat;
	for(unsigned int i=0; i<ntf; i++){
		if(stat((const char *)tracked_files[i].logfile, &filestat)){
			fprintf(core_log, "Cannot get stat info of the file \"%s\"\n", tracked_files[i].logfile);
			fflush(core_log);
			continue;
		}
		if(!filestat.st_size)
			print_createtime(tracked_files[i].log_stream);
	}
}


/*
 итак, проблема. Демон запущен, файл икс - лог. демон в него записал. демон остановлен. демон запущен
 второй раз. файл икс не лог. файл икс добавлен во время выполнения как лог. в файле икс
 осталась инфа от предыдущего запуска демона так как в коде ниже все рантайм-логи открываются в режиме дополнения.
 это неправильно.
 мб в начале чтения конфига заново сначала составить предыдущий список логфайлов,
 после перечитывания новый список логфайлов и те логи который добавились следует очистить. 
 */


bool is_in_list(char *list[], char *file)
{
	for(unsigned int i=0; i<ntf; i++){
		if(!strcmp(file, list[i]))
			return true;
	}
	return false;
}


int read_again()
{
	/* tracking files list creation*/
	char *old_list[ntf];
	for(unsigned int i=0; i<ntf; i++){
		old_list[i]=tracked_files[i].logfile;
	}
	

	for(unsigned int i=0; i<ntf; i++){
		if(inotify_rm_watch(inotify_fds[i], inotify_wds[i]) == -1){
			LOG_ERR();
			return -1;
		}
	}
	for(unsigned int i=0; i<ntf; i++){
		if(fclose(tracked_files[i].log_stream)){
			fprintf(core_log, "Cannot close log file \"%s\"\n", tracked_files[i].logfile);
			fflush(core_log);
			return -1;
		}
	}
	free(inotify_fds);
	free(inotify_wds);
	free(fds);
	free(tracked_files);


	int ret;

	ret = parse_config_file((const char *)config_file);
	CHECK_RETVAL(ret);


	ret = init_inotify_actions();
	CHECK_RETVAL(ret);

	init_pollfd_structures();


	/*not good code*/
	ret = create_log_streams(NOTRUNCATE);

	for(unsigned int i=0; i<ntf; i++){
		if(!is_in_list(old_list, tracked_files[i].logfile)){
			if(truncate((const char *)tracked_files[i].logfile, (off_t)0)){
				LOG_ERR();
			}
		}
	}

	CHECK_RETVAL(ret);
	print_starttime_in_new_logfiles();

	return 0;	
}


