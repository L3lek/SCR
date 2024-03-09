#include <stdio.h>
#include <unistd.h>
#include <time.h>

int main() {
    int i = 0;
    struct timespec t;
    t.tv_sec = 0;
    t.tv_nsec=100;
    

    while (1) {
        i++;
		//nanosleep(&t,0);
    }

    return 0;
}