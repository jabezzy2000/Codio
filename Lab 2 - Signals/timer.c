#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t alarm_count = 0; 
volatile sig_atomic_t running = 1;    

time_t start_time; 

void alarm_handler(int signum)
{
    
    alarm_count++;   
    alarm(1);
}

void sigint_handler(int signum)
{
    
    running = 0;
}

int main(int argc, char * argv[])
{
    
    signal(SIGALRM, alarm_handler); 
    signal(SIGINT, sigint_handler); 

    
    start_time = time(NULL);

    
    alarm(1);

    printf("Timer started. Press CTRL-C to stop.\n");

    while (running)
    {
        pause(); 
    }
    
    time_t end_time = time(NULL);
    double total_time = difftime(end_time, start_time);

    printf("\nProgram executed for %.0f seconds.\n", total_time);
    printf("Number of SIGALRM signals received: %d\n", alarm_count);

    return 0;
}
