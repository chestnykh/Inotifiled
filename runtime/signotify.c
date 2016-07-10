#include <stdio.h>
#include <signotify.h>
#include <parse.h>


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

void sigusr1_handler(int signum, siginfo_t *sinfo, void *ucontext)
{
	change_tracking_file_watches(0);
	fprintf(core_log, "in handler\n");
	fflush(core_log);
}



int set_sigusr1_handler()
{
	/*action.sa_sigaction = NULL;*/ //так делать почему-то нельзя
	struct sigaction action =
       	{
		.sa_sigaction = sigusr1_handler,
		.sa_flags = SA_RESTART | SA_SIGINFO,
		.sa_restorer = NULL
	};
	if(sigemptyset(&action.sa_mask) == -1) LOG_ERR();
	/*if(sigaddset(&action.sa_mask, SIGUSR1)) LOG_ERR();*/ //избыточно



	if(sigaction(SIGUSR1, &action, NULL) == -1){
		LOG_ERR();
		return -1;
	}
	fprintf(core_log, "HERE\n");
	fflush(core_log);
	return 0;
}
