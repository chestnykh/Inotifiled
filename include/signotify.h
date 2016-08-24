#pragma once


#include <signal.h>

//sigset_t sig_usr1;


//struct sigaction action;


void set_sigusr1_handler();

/*Реакция на sigusr1 будет перечитывание конфигурационного файла*/

void sigusr1_handler(int signum, siginfo_t *sinfo, void *ucontext);

void set_sigusr2_handler();
