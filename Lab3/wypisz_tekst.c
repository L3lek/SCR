#include <stdio.h>
#include <unistd.h> 

int main() {
    while (1) {
        printf("To jest tekst w nieskończonej pętli.\n");
		sleep(1);
    }
    return 0;
}