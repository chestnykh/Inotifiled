#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <handle_events.h>
#include <errno.h>

#include <sys/inotify.h>

#include <linux/limits.h>



#define LOG_ERR() { \
	fprintf(core_log, "ERROR:\n"); \
	fprintf(core_log, "File \"%s\"\n", __FILE__); \
	fprintf(core_log, "Function \"%s\"\n", __func__); \
	fprintf(core_log, "Line %d\n", __LINE__); \
	fprintf(core_log, "%s\n", strerror(errno)); \
	fflush(core_log); \
}



typedef struct inotify_tracked{
	char *path; /*path to file*/
	uint32_t events; /*bit mask*/
	char *logfile; /*log file*/
	FILE *log_stream;
} inotify_tracked_t;

int config_strings;
int ntf; /*number of tracked files*/



struct inotify_tracked *tracked_files;

struct inotify_tracked *inotify_tracked_entry; /*addr of first struct*/


int prepare_tf_structures();


void initialize_tracked_files_list();


#ifndef TF_TONEXT
#define TF_TONEXT() { \
	if((tracked_files-inotify_tracked_entry)/sizeof(struct inotify_tracked) < ((config_strings/3) - 1)) tracked_files+=sizeof(struct inotify_tracked); \
	else tracked_files = inotify_tracked_entry; \
}
#endif

#ifndef TF_TOSTART
#define TF_TOSTART() tracked_files = inotify_tracked_entry
#endif

#ifndef FOR_EACH_TF
#define FOR_EACH_TF() for(size_t i=0; i<(size_t)(ntf); i++)
#endif

#ifndef CLEAR_STR
#define CLEAR_STR(str,pos) {\
	for(size_t i=0; i<pos-1; i++){\
		*str++ = '\0'; \
	}\
}
#endif

int parse_config_file(const char *path);

bool compare_strings(char *s1, char *s2, size_t len);

__attribute__((always_inline)) void to_next_valid_symbol(char **str);

bool unused_string(int pos, int not_graph);

int define_file_string(int strcnt);


int handle_file_string(char *str/*some args*/);

int handle_event_string(char *str/*some args*/);

int handle_log_string(char *str/*some args*/);

int count_strings(FILE *f);

bool empty(char *str);


