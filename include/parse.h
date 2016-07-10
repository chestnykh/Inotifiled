#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <handle_events.h>
#include <errno.h>
#include <core.h>

#include <sys/inotify.h>

#include <linux/limits.h>



#ifndef LOG_ERR

#define LOG_ERR() { \
	fprintf(core_log, "ERROR:\n"); \
	fprintf(core_log, "File \"%s\"\n", __FILE__); \
	fprintf(core_log, "Function \"%s\"\n", __func__); \
	fprintf(core_log, "Line %d\n", __LINE__-1); \
	fprintf(core_log, "Errno message: "); \
	fprintf(core_log, "%s\n", strerror(errno)); \
	fflush(core_log); \
}

#endif

#ifndef REPORT_EXIT

#define REPORT_ERREXIT() { \
	fprintf(core_log, "Program exited due to error\n"); \
	fflush(core_log); \
} 

#endif


typedef struct inotify_tracked{
	FILE *log_stream;
	char *path; /*path to file*/
	char *logfile; /*log file*/
	uint32_t events; /*bit mask*/
} inotify_tracked_t;

unsigned int config_strings;
unsigned int ntf; /*number of tracked files*/



struct inotify_tracked *tracked_files;


int prepare_tf_structures();


void initialize_tracked_files_list();

#ifndef CLEAR_STR
#define CLEAR_STR(str,pos) { \
	for(size_t i=0; i<pos; i++){ \
		*str++ = '\0'; \
	} \
}
#endif

int parse_config_file(const char *path);

bool compare_strings(char *s1, char *s2, size_t len);

void to_next_valid_symbol(char **str);

bool unused_string(int pos, int not_graph);

int define_file_string(int strcnt);


int handle_file_string(char *str, int curr_tf_struct/*some args*/);

int handle_event_string(char *str, int curr_tf_struct/*some args*/);

int handle_log_string(char *str, int curr_tf_struct/*some args*/);

int count_strings(FILE *f);

bool empty(char *str);


