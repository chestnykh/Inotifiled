#pragma once


#include <parse.h>
#include <handle_events.h>

unsigned int struct_num;


int is_file_tracking(const char *file);

int change_tracking_file_watches(unsigned int struct_num);

