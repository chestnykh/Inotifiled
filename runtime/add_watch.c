#include <add_watch.h>
#include <alloca.h>
#include <signotify.h>

int is_file_tracking(const char *file)
{
	for(size_t i=0; i<ntf; i++){
		if(!strcmp(file,tracked_files[i].path)){
			struct_num = (unsigned int)i;
			return struct_num;
		}
	}
	return -1;	
}

int change_tracking_file_watches(unsigned int struct_num)
{	
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

