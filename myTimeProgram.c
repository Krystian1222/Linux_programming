// C program for measure execution time of given program

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int arg_test_count = 0;
	int arg_time_count = 0;
    int arg_count = 0;
	float tuser_avg = 0;
    float tsys_avg = 0;
    float treal_avg = 0;
    int ret = 0;
    int number_of_starts = 1;
    int display_test_program = 0;
    int Varg = 0;
    int Targ = 0;
    char *test_program_name = NULL;
    struct timespec start;
    struct timespec end;
    struct rusage rsg;
    int optarg_old = 0;
    while((ret = getopt(argc, argv, "+vt:")) != -1)
    {
        switch(ret)
        {
            case 'v': Varg = 1;
            		  break;
            case 't': Targ = 1;
            		  optarg_old = atoi(optarg);
            		  break;
            default: break;
        }
    }
    if(Varg)
    {
    	display_test_program = 1;
    }
    if(Targ)
    {
    	number_of_starts = optarg_old;
    }

    test_program_name = argv[optind];
   	if(!test_program_name)
   	{
   		printf("No test program is given.\n");
   		return -1;
   	}

    for(int i = 0; i < argc; i++)
    {
    	arg_count++;
    	if(strcmp(argv[i], test_program_name) == 0)
    	{
    		arg_time_count = arg_count;
    	}
    }

    arg_test_count = arg_count - arg_time_count;
	char **arg_prog_test = malloc((arg_test_count + 2) * sizeof(char*));

	int counter = 0;
	for(int i = optind; i < argc; i++)
	{
		arg_prog_test[counter] = argv[i];
		counter++;
	}

	arg_prog_test[0] = test_program_name;
	arg_prog_test[arg_test_count + 1] = NULL;

    for(int i = 0; i < number_of_starts; i++)
    {
        clockid_t clock = CLOCK_REALTIME;
        int time_result = clock_gettime(clock, &start);
        if(time_result == -1)
        {
            printf("Time reading error.\n");
            return -1;
        }

        pid_t pid = fork();
        if(pid == -1)
        {
            printf("The child process has not been created.\n");
            return -1;
        }
        else if(pid > 0)
        {
            int wait_result = wait4(pid, NULL, 0, &rsg);
            if(wait_result == -1)
            {
                printf("No child processes.\n");
                return -1;
            }

            int time_download_result = clock_gettime(clock, &end);

            if(time_download_result == -1)
            {
                printf("Time download error.\n");
                return -1;
            }

            long int end_sec;
            long int end_nsec;
            int minutes;
            float treal = 0;
            float seconds_without_minutes = 0;
            if(start.tv_nsec > koniec.tv_nsec)
            {
                end_sec = koniec.tv_sec - 1;
                end_nsec = koniec.tv_nsec + 1000000000;
                treal = (float)(end_sec - start.tv_sec) + (end_nsec - start.tv_nsec) / 1000000000.0;
                treal_avg += treal;
                minutes = (int)(treal / 60);
                seconds_without_minutes = treal - minutes * 60;
                //printf("real: %fs.\n", treal);
                printf("real: %dm%fs.\n", minutes, seconds_without_minutes);
            }
            else
            {
                treal = (float)(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
                treal_avg += treal;
                minutes = (int)(treal / 60);
                seconds_without_minutes = treal - minutes * 60;
                //printf("real: %fs.\n", treal);
                printf("real: %dm%fs.\n", minutes, seconds_without_minutes);
            }
            float tuser = rsg.ru_utime.tv_sec + rsg.ru_utime.tv_usec / 1000000.0;
            float tsys = rsg.ru_stime.tv_sec + rsg.ru_stime.tv_usec / 1000000.0;
            int minutes_user = (int)(tuser / 60);
            float seconds_without_minutes = tuser - minutes_user * 60;
            int minutes_sys = (int)(tsys / 60);
            float seconds_without_minutes_sys = tsys - minutes_sys * 60;
            tuser_avg += tuser;
            tsys_avg += tsys;
            //printf("user: %fs.\n", tuser);
            printf("user: %dm%fs.\n", minutes_user, seconds_without_minutes);
            //printf("sys:  %fs.\n", tsys);
            printf("sys:  %dm%fs.\n", minutes_sys, seconds_without_minutes_sys);
        }
        else
        {
            if(!display_test_program)
            {
                close(1);
                close(2);
                int h = open("/dev/null", O_WRONLY);
                dup2(h, 1);
                dup2(h, 2);
            }
            int execvp_result = execvp(test_program_name, arg_prog_test);
            if(execvp_result)
            {
            	printf("Function execvp call error.\n");
            	return -1;
            }
        }
    }
    if(number_of_starts >= 2)
    {
		treal_avg = treal_avg / number_of_starts;
		tuser_avg = tuser_avg / number_of_starts;
		tsys_avg = tsys_avg / number_of_starts;
		int minutes_user_avg = (int)(tuser_avg / 60);
		float seconds_without_minutes_user_avg = tuser_avg - minutes_user_avg * 60;
		int minutes_sys_avg = (int)(tsys_avg / 60);
		float seconds_without_minutes_sys_avg = tsys_avg - minutes_sys_avg * 60;
		int minutes_real_avg = (int)(treal_avg / 60);
		float seconds_without_minutes_real_avg = treal_avg - minutes_real_avg * 60;
		printf("real avg: %dm%fs.\n", minutes_real_avg, seconds_without_minutes_real_avg);
		printf("user avg: %dm%fs.\n", minutes_user_avg, seconds_without_minutes_user_avg);
		printf("sys avg:  %dm%fs.\n", minutes_sys_avg, seconds_without_minutes_sys_avg);
	}
    //printf("real avg: %lfs.\n", treal_avg);
    //printf("user avg: %fs.\n", tuser_avg);
    //printf("sys avg:  %fs.\n", tsys_avg);
    free(arg_prog_test);
    return 0;
}
