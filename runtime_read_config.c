#include <runtime_read_config.h>
#include <alloca.h>
#include <signotify.h>
#include <core.h>
#include <parse.h>
#include <handle_events.h>
#include <sys/stat.h>


int is_file_tracking(char *filename)
{
	for(size_t i=0; i<ntf; i++){
		if(!strcmp(filename,tracked_files[i].path)){
			struct_num = (unsigned int)i;
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
	for(int i=0; i<ntf; i++){
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
	for(int i=0; i<ntf; i++){
		if(!strcmp(file, list[i]))
			return true;
	}
	return false;
}


int read_again()
{
	/* tracking files list creation*/
	char *old_list[ntf];
	for(int i=0; i<ntf; i++){
		old_list[i]=tracked_files[i].logfile;
	}
	

	for(int i=0; i<ntf; i++){
		if(inotify_rm_watch(inotify_fds[i], inotify_wds[i]) == -1){
			LOG_ERR();
			return -1;
		}
	}
	for(int i=0; i<ntf; i++){
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

	for(int i=0; i<ntf; i++){
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



/*
int read_again()
{
	FILE *conf = fopen(config_file, "r");
	if(!conf)
		return -1;
	int strings = count_strings(conf);


	printf("strings = %d\n", strings);
	if(strings % 3){
		fprintf(core_log, "Error in config file!\n");
		fflush(core_log);
		return -1;
	}
	int last_strings = ntf*3;

	int diff;
	if(last_strings > strings){
		diff = last_strings - strings; //наблюдаемых файлов стало меньше
		inotify_
	}
	else diff = strings - last_strings; //наблюдаемых файлов стало больше
	


	int string_type;
	char str[LINE_MAX];
	for(size_t i=1; i<=strings; i++){
		readline(str,conf);
		if(unused_string(str)){
			i--;
			continue;
		}
		string_type=define_string_type(i);
		if(string_type == 1){
			if(handle_file_string(str, 0, NOCHANGE_TF))
				return -1;
			if(is_file_tracking(str) > 0)
				continue;
			
		}
		if(string_type == 2){
		}
		if(!string_type){
		}
	}
	return 0;
}
*/


/* Это плохой код, надо переделать
 * И какова цель этой функции ?
 * sigusr1 должен давать команду перечитать конфиг полностью,
 * или перечитать только для отдельного файла?
 * Короче полностью разобраться
 */

int change_tracking_file_watches(unsigned int struct_num)
{	
	return 0;
	FILE *conf = fopen(config_file,"r");
	char *str = alloca(LINE_MAX);
	int updated_events;
	for(size_t i=1; i<=ntf*3; i++){
		fgets(str,LINE_MAX,conf);
		str[strlen(str)-1]='\0';
		to_next_valid_symbol(&str);
		if(!strncmp(str,"events",6)){
			updated_events = handle_event_string(str, struct_num);
			if(updated_events < 0){
				fprintf(core_log,"Unable to re-read configuration file\n");
				fflush(core_log);
				return -1;
			}
			if(inotify_rm_watch(inotify_fds[struct_num], inotify_wds[struct_num])){
				LOG_ERR();
				return -1;
			}
			fprintf(tracked_files[struct_num].log_stream, "During changing watches watch instance has deleted and created again\n");
			fprintf(tracked_files[struct_num].log_stream, "TRACKING FILE HASN'T BEEN DELETED!\n");
			fflush(tracked_files[struct_num].log_stream);
			if(inotify_add_watch(inotify_fds[struct_num], tracked_files[struct_num].path, tracked_files[struct_num].events) == -1){
				LOG_ERR();
				return -1;
			}
		}
	}
	return 0;
}	


