#include <parse.h>
#include <errno.h>
#include <core.h>


int readline(char *str, FILE *f)
{
	if(!fgets(str,LINE_MAX,f))
		return -1;
	str[strlen(str)-1] = '\0';
	return 0;
}


int parse_config_file(const char *path)
{
	FILE *conf = fopen(path, "r");
	if(!conf){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	int strings = count_strings(conf);
	if(strings % 3){
		fprintf(core_log, "Error in config file!\n");
		fflush(core_log);
		return -1;
	}
	ntf = strings/3;
	/*тут мы должны подготовить таблицу файл -> события*/
	/*так как далее ее будем заполнять*/
	int current_tf_struct = 0;
	prepare_tf_structures();
	int icurr = 1; /*current symbol*/
	int pos = 0; /*string position*/
	int not_graph = 0; /*separator counter*/
	char *str = malloc(LINE_MAX);
	int strings_in_conf = 0;
	while(!readline(str,conf)){
		if(current_tf_struct > ntf){
			fprintf(core_log, "Unable to parse config file \"%s\" correctly!\n", path);
			return 1;
		}
		/*current string*/
		pos = 0;
		not_graph = 0;
		/*если строка пустая то дальше (и если строка это один символ \n) */
		if(unused_string(str))
			continue;
		strings_in_conf ++;
		/*на начало строки*/
		/*на первый графический символ новой строки*/
		to_next_valid_symbol(&str);
		if(define_string_type(strings_in_conf) == 1){
			/*тут формируется (одна из стадий) таблица файл - события*/
			handle_file_string(str, current_tf_struct);
		}
		else if(define_string_type(strings_in_conf) == 2){
			handle_event_string(str, current_tf_struct);
		}
		else{
			handle_log_string(str,current_tf_struct);
			current_tf_struct++;

		}
	}
	fclose(conf);
	return 0;
}




int prepare_tf_structures()
{
	tracked_files = calloc(ntf , sizeof(struct inotify_tracked));
	if(!tracked_files){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	initialize_tracked_files_list();
	return 0;
}



void initialize_tracked_files_list()
{
	/*we initialize events fields in all structures by zero*/
	for(size_t i=0; i<ntf; i++){
		tracked_files[i].events = 0;
	}
}



void to_next_valid_symbol(char **str)
{
	while(!isgraph((int)(**str))){
		if(**str == '\0') break;
		(*str)++;
	}
}



bool unused_string(char *str)
{
	if(!str)
		return true;
	size_t len = strlen(str);
	int not_graph = 0;
	for(int i=0; i<len; i++)
	{
		if(!isgraph((int)(str[i]))) 
			not_graph++;
	}
	return not_graph == len;
}

int define_string_type(int strcnt)
{
	if(strcnt % 3 == 1) return 1;
	if(strcnt % 3 == 2) return 2;
	return 0;
}

int handle_file_string(char *str, int curr_tf_struct)
{
	to_next_valid_symbol(&str);
	if(strncmp(str,"file", 4) && strncmp(str,"File",4)){
		fprintf(core_log, "Unrecognized file string!\n");
		return -1;
	}
	/*всё ок, строка начинается с 'file' или с 'File'*/
	str += 4;
	to_next_valid_symbol(&str);
	if(*str != '='){
		fprintf(core_log, "Expected \" = \" symbol!\n");
		return -1;
	}
	/*далее получаем путь к файлу*/
	/*тут не проверяем существует ли такой файл. Если нет то ошибка вылезет в начале работы inotify*/
	/*переход к пути к файлу в следующих 2-х строчках*/
	str++;
	to_next_valid_symbol(&str);

	char *temp = str;
	size_t path_size = 0;
	while(*str++ != '\0') path_size++;
	str = temp;
	
	char *path = malloc(path_size);
	/*подготовим в соответсвующей структуре место для пути*/

	while(*str != '\0'){
		*path = *str;
		 path++;
		 str++;
	}
	*path = '\0';
	path -= path_size;
	if(strlen(path) > PATH_MAX){
		fprintf(core_log, "Too big path to file\n");
		return -1;
	}
	tracked_files[curr_tf_struct].path = malloc(path_size);
	if(!tracked_files[curr_tf_struct].path){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	printf("[ath = %s\n", path);
	tracked_files[curr_tf_struct].path = path;
	return 0;
}


int handle_event_string(char *str, int curr_tf_struct)
{
	/*очищаем поле events*/
	tracked_files[curr_tf_struct].events=0;

	to_next_valid_symbol(&str);
	if(strncmp(str,"events",6)){
		fprintf(core_log, "Unrecognized events string!\n");
		return -1;
	}
	

	/*переходим на символ после s в events*/
	str += 6;
	to_next_valid_symbol(&str);

	if(*str != '='){
		fprintf(core_log, "Expected \" = \" symbol!\n");
		return -1;
	}
	str++;
	to_next_valid_symbol(&str);
	char *event = malloc(30); /*30 хватит*/
	size_t ch = 0;
	while(*str != '\n' && *str != '\0' && (int)(*str) != EOF){
		ch = 0;
		to_next_valid_symbol(&str);
		while(isalpha((int)(*str)) || *str == ' '){
			if(!isgraph((int)(*str))) continue;
			if(*str == ',') break;
			*event++ = *str++;
			ch++;
		}
		event[ch] = '\0';
		event -= ch;
	
		if(!strncmp(event,"write",5)){
			tracked_files[curr_tf_struct].events |= IN_MODIFY;
		}
		if(!strncmp(event,"read",4)){
			tracked_files[curr_tf_struct].events |= IN_ACCESS;
		}
		if(!strncmp(event,"open",4)){
			tracked_files[curr_tf_struct].events |= IN_OPEN;
		}
		if(!strncmp(event,"delete",6)){
			tracked_files[curr_tf_struct].events |= IN_DELETE;
		}
		if(!strncmp(event,"metadata",8)){
			tracked_files[curr_tf_struct].events |= IN_ATTRIB;
		}
		if(!strncmp(event,"all",3)){
			tracked_files[curr_tf_struct].events |= IN_ALL_EVENTS;
		}
		if(!strncmp(event,"change metadata",15)){
			tracked_files[curr_tf_struct].events |= IN_ATTRIB;
		}
		if(!strncmp(event,"close write",11)){
			tracked_files[curr_tf_struct].events |= IN_CLOSE_WRITE;
		}
		if(!strncmp(event,"close nowrite",13)){
			tracked_files[curr_tf_struct].events |= IN_CLOSE_NOWRITE;
		}
		if(!strncmp(event,"move from",9)){
			tracked_files[curr_tf_struct].events |= IN_MOVED_FROM;
		}
		if(!strncmp(event,"move to",7)){
			tracked_files[curr_tf_struct].events |= IN_MOVED_TO;
		}
		if(!strncmp(event,"create",6)){
			tracked_files[curr_tf_struct].events |= IN_CREATE;
		}
		if(!strncmp(event,"all close",9)){
			tracked_files[curr_tf_struct].events |= IN_ALL_EVENTS;
		}
		if(!strncmp(event,"all move",8)){
			tracked_files[curr_tf_struct].events |= IN_MOVE;
		}
		if(!strncmp(event,"delete self",11)){
			tracked_files[curr_tf_struct].events |= IN_DELETE_SELF;
		}
		if(!strncmp(event,"move self",9)){
			tracked_files[curr_tf_struct].events |= IN_MOVE_SELF;
		}
		if(!strncmp(event,"don't Follow symbolic links",27)){
			tracked_files[curr_tf_struct].events |= IN_DONT_FOLLOW;
		}
		CLEAR_STR(event,ch);
		str++;	
	}
	printf("events = %d\n", tracked_files[curr_tf_struct].events);
	return 0;
}

int handle_log_string (char *str, int curr_tf_struct)
{
	to_next_valid_symbol(&str);
	if(strncmp(str,"logfile",7)){
		fprintf(core_log, "Unrecognized log string!\n");
		return -1;
	}
	str+=7;
	to_next_valid_symbol(&str);
	if(*str != '='){
		fprintf(core_log, "Expected \" = \" symbol!\n");
		return -1;
	}
	str++;
	to_next_valid_symbol(&str);	
	size_t log_name_size = 0;
	while(*str++ != '\0') log_name_size++;
	str -= (log_name_size+1);
	char *log = malloc(log_name_size+1);
	if(!log){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	for(size_t i=0; i<log_name_size; i++){
		log[i] = *str++;
	}
	log[log_name_size] = '\0';
	tracked_files[curr_tf_struct].logfile = log;
	if(strlen(tracked_files[curr_tf_struct].logfile) > PATH_MAX){
		fprintf(stderr, "Too big path to file\n");
		return -1;
	}
	return 0;
}


int count_strings(FILE *f){
	int strings = 0;
	char str[LINE_MAX];

	/*запоминаем позицию в потоке*/
	fpos_t pos;
	if(fgetpos(f, &pos)){
		LOG_ERR();
		fprintf(core_log, "Warning: continuous work of the programm might bi incorrect!\n");
	}
	while(!feof(f)){
		if(fgets(str, LINE_MAX, f) && !unused_string(str))
			strings++;
	}
	/*возвращаем позицию в потоке в его */
	if(fsetpos(f, &pos)){
		LOG_ERR();
		fprintf(core_log, "Warning: continuous work of the programm might be incorrect!\n");
	}
	return strings;
}



