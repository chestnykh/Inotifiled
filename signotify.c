#include <stdio.h>
#include <signotify.h>
#include <parse.h>
#include <daemon.h>


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
	struct sigaction action =
       	{
		.sa_sigaction = sigusr1_handler,
		.sa_flags = SA_SIGINFO | SA_RESTART,
		.sa_restorer = NULL
	};
	if(sigemptyset(&action.sa_mask) == -1)
		LOG_ERR();



	if(sigaction(SIGUSR1, &action, NULL) == -1){
		LOG_ERR();
	}
}


void sigusr2_handler(int sig)
{
	print_finishtime();
	raise(SIGKILL);
}


void set_sigusr2_handler()
{
	if(signal(SIGUSR2, sigusr2_handler) == SIG_ERR)
		LOG_ERR();
}
