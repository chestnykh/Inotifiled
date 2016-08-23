#include <stdio.h>
#include <signotify.h>
#include <parse.h>
#include <daemon.h>


/*
void lock_sigusr1()
{
	if(sigemptyset(&sig_usr1)) LOG_ERR();
	if(sigaddset(*sig_usr1, SIGUSR1)) LOG_ERR();
	if(sigprocmask(SIG_SETMASK, &sig_usr1, NULL)) LOG_ERR();
}	

void unlock_sigusr1()
{
	if(sigprocmask(SIG_UNBLOCK, &sig_usr1, NULL)) LOG_ERR();	
	if(sigdelset(&sig_usr1, SIGUSR1)) LOG_ERR();
}
*/

extern int read_again();

void sigusr1_handler(int signum, siginfo_t *sinfo, void *ucontext)
{
	if(read_again() == -1){
		fprintf(core_log, "Error while reading again configuration file!\n");
		REPORT_ERREXIT();
	}
}


void set_sigusr1_handler()
{
	/*action.sa_sigaction = NULL;*/ //так делать почему-то нельзя
	struct sigaction action =
       	{
		.sa_sigaction = sigusr1_handler,
		.sa_flags = SA_SIGINFO | SA_RESTART,
		.sa_restorer = NULL
	};
	if(sigemptyset(&action.sa_mask) == -1)
		LOG_ERR();
	/*if(sigaddset(&action.sa_mask, SIGUSR1)) LOG_ERR();*/ //избыточно



	if(sigaction(SIGUSR1, &action, NULL) == -1){
		LOG_ERR();
	}
}


void kill_handler(int sig)
{
	print_finishtime();
}


void set_kill_handler()
{
	if(signal(SIGTERM, kill_handler) == SIG_ERR)
		fprintf(core_log, "Cannot set SIGKILL handler.\nFinish time won't be printed in log files!\n");
}
