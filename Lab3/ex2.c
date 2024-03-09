#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int signal_flag = 0;
int usr2_blocked = 0;

void sigterm_handler(int signo) {
        printf("Odebrano sygnał SIGTERM - zakończanie programu\n");
        exit(0);
}
 
void sigusr1_handler(int signo) {
        printf("Odebrano sygnał SIGUSR1\n");
        signal_flag = 1;
}

void sigusr2_handler(int signo) {
        printf("Odebrano sygnał SIGUSR2\n");
}
 
int main() {
    int i =0;
    struct timespec t;    
    t.tv_sec = 0;
    t.tv_nsec=10000000;
    sigset_t mask;
    sigemptyset(&mask);
 
    signal(SIGALRM, SIG_IGN);

    signal(SIGTERM, sigterm_handler);
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);
 
    sigaddset(&mask, SIGUSR2);


    while (1) {
        if (signal_flag) {
            printf("Program otrzymał sygnał SIGUSR1. Kontynuowanie...\n");
            signal_flag = 0;
        }


        if (i == 1){
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            printf("Sygnał SIGUSR2 został odblokowany\n");
        }

        if (i == 5){
            sigprocmask(SIG_BLOCK, &mask, NULL);
            printf("Sygnał SIGUSR2 został zablokowany\n");
        }

        i = i % 1000;
        i++;
        nanosleep(&t,0);
    }

    return 0;
}
 
