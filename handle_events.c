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
		REPORT_ERREXIT();
		return -1;
	}

	char *buf = malloc(20);
	if(buf == NULL){
		LOG_ERR();
		REPORT_ERREXIT();
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
		REPORT_ERREXIT();
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
	
	inotify_fds = calloc(ntf,sizeof(int));
	if(!inotify_fds){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	inotify_wds = calloc(ntf, sizeof(uint32_t));
	if(!inotify_wds){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	
	for(size_t i=0; i<ntf; i++){
		inotify_fds[i] = inotify_init1(0);
		if(inotify_fds[i] == -1){
			LOG_ERR();
			REPORT_ERREXIT();
			return -1;
		}
		inotify_wds[i] = inotify_add_watch(inotify_fds[i], tracked_files[i].path, tracked_files[i].events);
		if(inotify_wds[i] == -1){
			LOG_ERR();
			REPORT_ERREXIT();
			return -1;
		}
	}
	return 0;
}


void init_pollfd_structures()
{
	fds = calloc(ntf, sizeof(struct pollfd));
	for(int i=0; i<ntf; i++){
		fds[i].fd = inotify_fds[i];
		fds[i].events = POLLIN;
	}
}





int create_log_streams()
{
	int fd;
	for(size_t i=0; i<ntf; i++){
		fd = open(tracked_files[i].logfile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		if(fd == -1){
			LOG_ERR();
			REPORT_ERREXIT();
			return -1;
		}
		tracked_files[i].log_stream = fdopen(fd, "a+");
		if(!tracked_files[i].log_stream){
			LOG_ERR();
			REPORT_ERREXIT();
			return -1;
		}
	}
	return 0;
}




int init_event_struct()
{
	ievents = malloc(sizeof(struct inotify_event));
	if(!ievents){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	return 0;
}

int wait_events()
{
	int pollret;
	do {
		pollret = poll(fds, (nfds_t)ntf, -1);
		//if(errno == EINTR) puts("EINTR!!!");
	} while (pollret == -1 && errno == EINTR);
	fprintf(core_log, "pollret = %d\n", pollret);
	fflush(core_log);
	if(pollret == -1){
		LOG_ERR();
		REPORT_ERREXIT();
		return -1;
	}
	return 0;
}

int handle_events()
{
	size_t i;
	ssize_t rread;
	char buf[BUFSIZE]__attribute__((aligned(__alignof__(struct inotify_event))));
	//memset(buf, 0, BUFSIZE);
	for(size_t j=0; j<ntf; j++){
		if((fds[j].revents) & POLLIN){
			rread = read(fds[j].fd, buf, BUFSIZE);
			if(rread == -1){
				LOG_ERR();
				REPORT_ERREXIT();
				return -1;
			}
			i=0;
			while(i < rread){
				fprintf(tracked_files[j].log_stream, "##########\n");
				print_timeinfo(tracked_files[j].log_stream);
				ievents = (struct inotify_event *)&buf[i];
				REPORT_ACTION(ievents -> mask , tracked_files[j].log_stream, ievents->cookie);
				if(ievents->len) fprintf(tracked_files[j].log_stream, "name = %s\n", ievents->name);
				if(ievents->cookie) fprintf(tracked_files[j].log_stream, "cookie = %u\n", ievents->cookie);
				fflush(tracked_files[j].log_stream);
				i+=sizeof(struct inotify_event) + ievents -> len;
			}	
		}
	}
	return 0;
}
