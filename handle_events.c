#include <handle_events.h>
#include <parse.h>


void print_timeinfo(FILE *log)
{
	time_t _t = time(NULL);
	struct tm *time = localtime(&_t);
	fprintf(log, "[%d-", time->tm_year+1900);
	fprintf(log, "%d-",time->tm_mon+1);	
	fprintf(log, "%d ",time->tm_mday);	
	fprintf(log, "%d:",time->tm_hour);	
	fprintf(log, "%d:",time->tm_min);	
	fprintf(log, "%d]\n",time->tm_sec);	
}


int get_inotify_limits()
{
	/*лучше конечно юзать системные вызовы*/
	FILE *f = fopen("/proc/sys/fs/inotify/max_user_instances", "r");
	if(!f){
		LOG_ERR(); /*добавить __FILE__ __LINE__ ?*/
		return -1;
	}

	char *buf = (char *)malloc(20);
	if(buf == NULL){
		LOG_ERR();
		return -1;
	}
	int curr = 1;
	int digits = 0;
	while(1){
		curr = fgetc(f);
		if(curr == EOF || curr == 10) break;
		*buf = (char)curr;
		buf++;
		digits++;
	}
	if(digits > 19 || digits < 1){
		fprintf(core_log, "ERROR: Cannot get inotify watches limit\n");
		return -1;
	}


	buf -= digits;
	buf[digits] = '\0';
	inotify_max_inst = (uint)atoi(buf);
	for(int i=0; i<digits; i++) buf[i] = '\0';
	f = fopen("/proc/sys/fs/inotify/max_user_watches", "r");
	if(!f){
		LOG_ERR();
		return -1;
	}
	digits = 0;
	while(1){
		curr = fgetc(f);
		if(curr == EOF || curr == 10) break;
		*buf = (char)curr;
		buf++;
		digits++;
	}
	if(digits > 19 || digits < 1){
		fprintf(stderr, "ERROR: Cannot get inotify watches limit\n");
		return -1;
	}
	buf -= digits;
	buf[digits] = '\0';
	inotify_max_watches = (uint)atoi(buf);
	free(buf);
	return 0;
}


int init_inotify_actions()
{
	if(ntf > inotify_max_inst){
		fprintf(core_log, "The number of tracked files is higher than the max value of inotify instances\n");
		return -1;
	}
	if(ntf > inotify_max_watches){
		fprintf(core_log, "The number of tracked_files is higher than the max value of inotify watches\n");
	}
	
	inotify_fds = (int *)malloc(ntf);
	if(!inotify_fds){
		LOG_ERR();
		return -1;
	}
	inotify_fds_entry = inotify_fds;
	inotify_wds = (uint32_t *)malloc(ntf);
	if(!inotify_wds){
		LOG_ERR();
		return -1;
	}
	inotify_wds_entry = inotify_wds;
	TF_TOSTART();
	
	for(size_t i=0; i<sizeof(int)*ntf; i+=sizeof(int)){
		inotify_fds[i] = inotify_init1(0);
		if(inotify_fds[i] == -1){
			LOG_ERR();
			return -1;
		}
		inotify_wds[i] = inotify_add_watch(inotify_fds[i], tracked_files -> path, tracked_files -> events);
		if(inotify_wds[i] == -1){
			LOG_ERR();
			return -1;
		}
		TF_TONEXT();
	}
	/*избыточно....*/
	TF_TOSTART();
	return 0;
}


void init_pollfd_structures()
{
	size_t var = 0;
	for(int i=0; i<ntf; i++){
		fds[i].fd = inotify_fds[var];
		fds[i].events = POLLIN;
		var += 4;
	}
}



int wait_events()
{
	int pollret = poll(fds, (nfds_t)ntf, -1);
	if(pollret == -1){
		LOG_ERR();
		return -1;
	}
	//printf("pollret = %d\n", pollret);
	return 0;
}


int create_log_streams()
{
	int fd;
	TF_TOSTART();
	FOR_EACH_TF(){
		fd = open(tracked_files -> logfile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if(fd == -1){
			LOG_ERR();
			return -1;
		}
		tracked_files -> log_stream = fdopen(fd, "a+");
		if(!tracked_files -> log_stream){
			LOG_ERR();
			return -1;
		}
		TF_TONEXT();
	}
	/*избыточно...*/
	TF_TOSTART();
	return 0;
}




int init_event_struct()
{
	ievents = (struct inotify_event*)malloc(sizeof(struct inotify_event));
	if(!ievents){
		LOG_ERR();
		return -1;
	}
	return 0;
}


int handle_events()
{
	size_t i;
	ssize_t rread;
	//struct inotify_event *ievents;
	char buf[BUFSIZE]__attribute__((aligned(__alignof__(struct inotify_event))));
	//memset(buf, 0, BUFSIZE);
	TF_TOSTART();
	for(size_t j=0; j<ntf; j++){
		//printf("!!!! %p\n%d\n", pfd, pfd -> fd);
		if((fds[j].revents) & POLLIN){
			rread = read(fds[j].fd, buf, BUFSIZE);
			if(rread == -1){
				LOG_ERR();
				return -1;
			}
			i=0;
			while(i < rread){
				fprintf(tracked_files -> log_stream, "##########\n");
				fprintf(tracked_files -> log_stream, "%li\n", rread);
				print_timeinfo(tracked_files -> log_stream);
				ievents = (struct inotify_event *)&buf[i];
				fprintf(tracked_files -> log_stream, "wd = %d\n", ievents->wd);
				fprintf(tracked_files -> log_stream, "mask = %u\n", ievents->mask);
				fprintf(tracked_files -> log_stream, "cookie = %u\n", ievents->cookie);
				if(ievents->len != 0) fprintf(tracked_files -> log_stream, "name = %s\n", ievents->name);
				//fprintf(stdout, "##########\n");
				//print_timeinfo(stdout);
				ievents = (struct inotify_event *)&buf[i];
				//fprintf(stdout, "wd = %d\n", ievents->wd);
				//fprintf(stdout, "mask = %u\n", ievents->mask);
				//fprintf(stdout, "cookie = %u\n", ievents->cookie);
				//if(ievents->len != 0) fprintf(stdout, "name = %s\n", ievents->name);
				fflush(tracked_files->log_stream);
				i+=sizeof(struct inotify_event) + ievents -> len;
			}	
		}
		TF_TONEXT();
	}
	return 0;
}
