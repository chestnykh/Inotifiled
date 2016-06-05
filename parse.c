#include <parse.h>
#include <errno.h>
#include <util.h>

extern char start_wd[PATH_MAX];



int parse_config_file(const char *path)
{
	FILE *conf = fopen(path, "r");
	if(!conf){
		LOG_ERR();
		return -1;
	}

	FILE *npfd = fopen("npollfd.data", "r");
	int get;
	char pdata[10];
	memset(pdata, 0, 10);
	for(size_t i=0; (get = fgetc(npfd)) != EOF; i++){
		pdata[i] = (char)get;
	}
	ntf = atoi(pdata); 
	//unstrcat(start_wd, strlen("/npollfd.data"));
	/*тут мы должны подготовить таблицу файл -> события*/
	/*так как далее ее будем заполнять*/
	prepare_tf_structures();
	/*
	printf("ev0 = %d\n", tracked_files -> events);
	tf_tonext();
	printf("ev1 = %d\n", tracked_files -> events);
	tf_tostart();
	printf("ev0 = %d\n", tracked_files -> events);
	*/

	TF_TOSTART();
	int icurr = 1; /*current symbol*/
	int pos = 0; /*string position*/
	int not_graph = 0; /*separator counter*/
	char *str = (char *)malloc(LINE_MAX);
	int strings_in_conf = 0;
	while(icurr != EOF){
		/*current string*/
		pos = 0;
		not_graph = 0;
		/*получаем новую строку в str*/
		while((icurr=fgetc(conf)) != 10 && icurr != EOF){
			*str++ = (char)icurr;
			if(++pos >= LINE_MAX-1){
				break;
			}
			if(!isgraph(icurr)) not_graph++;
		}
		/*если строка пустая то дальше (и если строка это один символ \n) */
		if(unused_string(pos,not_graph)) continue;
		strings_in_conf ++;
		str[pos]='\0';
		/*на начало строки*/
		str -= pos;
	
		/*на первый графический символ новой строки*/
		to_next_valid_symbol(&str);
		if(define_file_string(strings_in_conf) == 1){
			/*тут формируется (одна из стадий) таблица файл - события*/
			handle_file_string(str);
		}
		else if(define_file_string(strings_in_conf) == 2) handle_event_string(str);
		else{
			handle_log_string(str);

		}
		CLEAR_STR(str,pos);
	}
	fclose(conf);
	return 0;
}




int prepare_tf_structures()
{
	tracked_files = (struct inotify_tracked*)malloc(ntf * sizeof(struct inotify_tracked));
	if(!tracked_files){
		LOG_ERR();
		return -1;
	}
	inotify_tracked_entry = tracked_files;
	initialize_tracked_files_list();
	return 0;
}



void initialize_tracked_files_list()
{
	//we initialize events fields in all structures by zero
	FOR_EACH_TF(){
		tracked_files -> events = 0;
		TF_TONEXT();
	}
}



__attribute__((always_inline)) void to_next_valid_symbol(char **str)
{
	while(!isgraph((int)(**str))){
		if(**str == '\0') break;
		(*str)++;
	}
}



bool compare_strings(char *s1, char *s2, size_t len)
{
	if((strlen((const char*)(s1)) < len) || (strlen((const char*)(s2)) < len)) return false;
	for(size_t i=0; i<len; i++){
		if(*s1++ != *s2++) return false;	
	}
	return true;
}


bool unused_string(int pos, int not_graph)
{
	return (!pos || pos == not_graph);	
}

int define_file_string(int strcnt)
{
	if(strcnt % 3 == 1) return 1;
	if(strcnt % 3 == 2) return 2;
	return 0;
}

int handle_file_string(char *str)
{
	to_next_valid_symbol(&str);
	if(!compare_strings(str,"file", 4) && !compare_strings(str,"File",4)){
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
	
	char *path = (char *)malloc(path_size);
	/*подготовим в соответсвующей структуре место для пути*/
	tracked_files -> path = (char *)malloc(path_size);
	if(tracked_files -> path == NULL){
		LOG_ERR();
		return -1;
	}

	while(*str != '\0'){
		*path = *str;
		 path++;
		 str++;
	}
	*path = '\0';
	path -= path_size;
	if(strlen(tracked_files -> path) > PATH_MAX){
		fprintf(stderr, "Too big path to file\n");
		return -1;
	}
	tracked_files -> path = path;
	return 0;
}


int handle_event_string(char *str)
{
	to_next_valid_symbol(&str);
	if(!compare_strings(str,"events",6)){
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
	char *event = (char *)malloc(30); /*30 хватит*/
	size_t ch = 0;
	while(*str != '\n' && *str != '\0' && (int)(*str) != EOF){
		ch = 0;
		while(*str != ',' && *str != '\0' && *str != '\n' && (int)(*str) != EOF){
			*event++ = *str++;
			//printf("*event = %c\n", *eventu);
			ch++;
		}
		event[ch] = '\0';
		event -= ch;
		if(!strcmp(event,"write")){
			tracked_files -> events |= IN_MODIFY;
		}
		if(!strcmp(event,"read")){
			tracked_files -> events |= IN_ACCESS;
		}
		if(!strcmp(event,"open")){
			tracked_files -> events |= IN_OPEN;
		}
		if(!strcmp(event,"delete")){
			tracked_files -> events |= IN_DELETE;
		}
		if(!strcmp(event,"metadata")){
			tracked_files -> events |= IN_ATTRIB;
		}
		if(!strcmp(event,"all")){
			tracked_files -> events |= IN_ALL_EVENTS;
		}
		if(!strcmp(event,"change metadata")){
			tracked_files -> events |= IN_ATTRIB;
		}
		if(!strcmp(event,"close write")){
			tracked_files -> events |= IN_CLOSE_WRITE;
		}
		if(!strcmp(event,"close nowrite")){
			tracked_files -> events |= IN_CLOSE_NOWRITE;
		}
		if(!strcmp(event,"move from")){
			tracked_files -> events |= IN_MOVED_FROM;
		}
		if(!strcmp(event,"move to")){
			tracked_files -> events |= IN_MOVED_TO;
		}
		if(!strcmp(event,"create")){
			tracked_files -> events |= IN_CREATE;
		}
		if(!strcmp(event,"all close")){
			tracked_files -> events |= IN_ALL_EVENTS;
		}
		if(!strcmp(event,"all move")){
			tracked_files -> events |= IN_MOVE;
		}
		if(!strcmp(event,"delete self")){
			tracked_files -> events |= IN_DELETE_SELF;
		}
		if(!strcmp(event,"move self")){
			tracked_files -> events |= IN_MOVE_SELF;
		}
		CLEAR_STR(event,ch);
		str++;	
	}

	//printf("events = %d\n", tracked_files -> events);
	return 0;
}

int handle_log_string (char *str)
{
	to_next_valid_symbol(&str);
	if(!compare_strings(str,"logfile",7)){
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
	char *log = (char *)malloc(log_name_size+1);
	if(!log){
		LOG_ERR();
		return -1;
	}
	for(size_t i=0; i<log_name_size; i++){
		log[i] = *str++;
	}
	log[log_name_size] = '\0';
	tracked_files -> logfile = log;
	if(strlen(tracked_files -> logfile) > PATH_MAX){
		fprintf(stderr, "Too big path to file\n");
		return -1;
	}
	TF_TONEXT();
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
		if(fgets(str, LINE_MAX, f) && !empty(str)) strings++;
	}
	/*возвращаем позицию в потоке в его */
	if(fsetpos(f, &pos)){
		LOG_ERR();
		fprintf(core_log, "Warning: continuous work of the programm might bi incorrect!\n");
	}
	return strings;
}

bool empty(char *str){
	int nograph = 0;
	for(size_t i=0; i<strlen(str); i++){
		if(!isgraph(str[i])) nograph++;
	}
	return nograph == strlen(str);
}


