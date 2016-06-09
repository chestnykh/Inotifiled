#pragma once

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <poll.h>
#include <sys/inotify.h>


typedef unsigned int uint;
//weljbfewfnwflenew



#ifndef REPORT_ACTION
#define REPORT_ACTION(mask, log) { \
	if(mask & IN_ACCESS){ \
		fprintf(log, "READ action has happened\n");\
	}\
	if(mask & IN_MODIFY){ \
		fprintf(log, "WRITE action has happened\n");\
	}\
	if(mask & IN_ATTRIB){ \
		fprintf(log, "Metadata has changed\n");\
	}\
	if(mask & IN_CLOSE_WRITE){ \
		fprintf(log, "FIle had been closed before it has opened to write\n");\
	}\
	if(mask & IN_CLOSE_NOWRITE){ \
		fprintf(log, "FIle has closed before it had been opened to write\n");\
	}\
	if(mask & IN_OPEN){ \
		fprintf(log, "OPEN action has happened\n");\
	}\
	if(mask & IN_MOVED_FROM){ \
		fprintf(log, "File has moved from the tracking directory\n");\
	}\
	if(mask & IN_MOVED_TO){ \
		fprintf(log, "File has moved to the tracking directory\n");\
	}\
	if(mask & IN_CREATE){ \
		fprintf(log, "File has created in the tracking directory\n");\
	}\
	if(mask & IN_DELETE){ \
		fprintf(log, "File has deleted in the tracking directory\n");\
	}\
	if(mask & IN_MOVE_SELF){ \
		fprintf(log, "File has moved itself\n");\
	}\
	if(mask & IN_DELETE_SELF){ \
		fprintf(log, "File has deleted itself\n");\
	}\
} 
#endif





uint inotify_max_inst;
uint inotify_max_watches;

int *inotify_fds;
uint32_t *inotify_wds;

FILE *core_log;

struct pollfd *fds;

struct inotify_event *ievents;

int get_inotify_limits();


void print_timeinfo(FILE *log);


int create_log_streams();


int init_inotify_actions();

void init_pollfd_structures();

int init_log_files();


int init_event_struct();

int wait_events();

//int handle_events(struct pollfd changed_file);
int handle_events();




/*для handle_events()*/

#define BUFSIZE 4096

//char buf[BUFSIZE]__attribute__((aligned(__alignof__(struct inotify_event))));

