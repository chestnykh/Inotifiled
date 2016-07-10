#pragma once


#include <signal.h>

//sigset_t sig_usr1;


//struct sigaction action;


int set_sigusr1_handler();

/*Реакция на sigusr1 будет перечитывание конфигурационного файла*/

void sigusr1_handler(int signum, siginfo_t *sinfo, void *ucontext);
