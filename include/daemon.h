#pragma once

#include <unistd.h>
#include <sys/types.h>

int start_daemon();

void print_createtime();
void print_finishtime();

int register_finish_procedures();
