// C program to test the handling of incoming signals

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <wait.h>
#include <string.h>

int sig_alrm = 1, main_loop = 1, children = 1, finished_children = 0;

char *display_current_time()
{
    time_t current_time;
    char *time_string;

    current_time = time(NULL);
    if(current_time == 0)
    {
        fprintf(stderr, "Failure to obtain the current time.\n");
        exit(EXIT_FAILURE);
    }

    time_string = ctime(&current_time);

    if(time_string == NULL)
    {
        fprintf(stderr, "It is not possible to recalculate the current time.\n");
        exit(EXIT_FAILURE);
    }

    int time_string_len = strlen(time_string);
    time_string[time_string_len - 1] = '\0';
    return time_string;
}

void child_handler_sigalrm(int number, siginfo_t *info, void *ucontent)
{
	if(number == SIGALRM)
	{
		children = 0;
	}
}

void sigint_handler(int number, siginfo_t *info, void *ucontent)
{
	if(number == SIGINT)
	{
		main_loop = 0;
	}

}

void sigalrm_handler(int number, siginfo_t *info, void *ucontent)
{
    if(number == SIGALRM)
    {
        sig_alrm = 0;
    }
}

void sigchld_handler(int number, siginfo_t *info, void *ucontent)
{
    if(number == SIGCHLD)
    {
		finished_children++;
		printf("\t\t\t[ %d ] [ %d ] [ %s ]\n", info->si_pid, info->si_status, display_current_time());
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    int ret = 0;
    int Warg = 0;
    int Marg = 0;
    int spacing = 0;
    int Tmax = 0;
    int children_counter = 0;

    while((ret = getopt(argc, argv, "w:m:")) != -1)
    {
        switch(ret)
        {
            case 'w': Warg = 1;
            		  break;
            case 'm': Marg = 1;
            		  break;
            default: break;
        }
    }

    if(!Warg || !Marg)
    {
        printf("Wrong arguments were given.\n");
        return -1;
    }
    if(Warg)
    {
        spacing = atoi(argv[2]);
    }
    if(Marg)
    {
        Tmax = atoi(argv[4]);
    }

	struct sigaction sigAction_sigalrm;
	sigAction_sigalrm.sa_sigaction = sigalrm_handler;
	sigAction_sigalrm.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&(sigAction_sigalrm.sa_mask));
    sigaddset(&sigAction_sigalrm.sa_mask, SIGCHLD);

	struct sigaction sigAction_sigchld;
	sigAction_sigchld.sa_sigaction = sigchld_handler;
	sigAction_sigchld.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&(sigAction_sigchld.sa_mask));
	sigaddset(&sigAction_sigchld.sa_mask, SIGCHLD);

	struct sigaction sigAction_sigint;
	sigAction_sigint.sa_sigaction = sigint_handler;
	sigAction_sigint.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&(sigAction_sigint.sa_mask));
	sigaddset(&sigAction_sigint.sa_mask, SIGINT);

	sigaction(SIGCHLD, &sigAction_sigchld, NULL);
	sigaction(SIGINT, &sigAction_sigint, NULL);
	sigaction(SIGALRM, &sigAction_sigalrm, NULL);

	struct sigaction sigAction2;
	sigAction2.sa_sigaction = child_handler_sigalrm;
	sigAction2.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&(sigAction2.sa_mask));
	sigaddset(&sigAction2.sa_mask, SIGALRM);

    int arg_max = 0;
    if(spacing < Tmax)
    {
        arg_max = Tmax;
    }
    else
    {
        arg_max = spacing;
    }

	while(main_loop)
	{
		children_counter++;
		int id = fork();
		if(id == -1)
		{
		    printf("The child process has not been created.\n");
            return -1;
		}
		else if(id > 0)
		{
			alarm(spacing);
			while(sig_alrm)
			{
				//
			}
			sig_alrm = 1;
		}
		else
		{
            srand(time(NULL));
			int random_number = (rand() % arg_max + 2) - 1;
            printf("[ %d ] [ %d ] [ %s ]\n", getpid(), random_number, display_current_time());
			sigaction(SIGALRM, &sigAction2, NULL);
			alarm(random_number);
			long long int calculations = 1;
			while(children)
			{
				calculations++;
			}
			return random_number;
		}
	}

    while(waitpid(-1, NULL, WNOHANG) >= 0);

    return 0;
}
