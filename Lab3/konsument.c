#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    // Uruchamiamy pierwszy program w tle
    if (fork() == 0) {
        execl("./wypisz_tekst", "wypisz_tekst", NULL);
    }
	sleep(1000);
    return 0;
}
