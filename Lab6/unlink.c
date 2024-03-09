#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define BABBLE_NAME "/Y01-42e"

int main() {
    // Usuń segment pamięci współdzielonej o nazwie BABBLE_NAME
    if (shm_unlink(BABBLE_NAME) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }

    printf("Segment pamięci współdzielonej usunięty.\n");

    return 0;
}