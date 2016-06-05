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


#define Ntf 2


/*
#ifndef REPORT_ACTION(mask, log) { \
	if(mask & )
}
*/




uint inotify_max_inst;
uint inotify_max_watches;

int *inotify_fds;
int *inotify_fds_entry;


uint32_t *inotify_wds;
uint32_t *inotify_wds_entry;

FILE *core_log;
FILE *log_streams[Ntf];


struct pollfd fds[Ntf];

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

