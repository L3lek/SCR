#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Uruchamiamy drugi program (konsument)
    if (fork() == 0) {
        execl("./konsument", "konsument", NULL);
    }
	sleep(1000);
    return 0;
}
