#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>

#define BABBLE_NAME "/Y01-42e"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

struct babblespace {
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first, babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

void *create_shared_memory(const char *name, size_t size) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, BABBLE_MODE);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shm_fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    void *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    close(shm_fd);

    return shared_memory;
}

int main() {
    struct babblespace *babblespace;
    size_t size = sizeof(struct babblespace);

    // Utwórz nowy obszar pamięci wspólnej
    babblespace = (struct babblespace *)create_shared_memory(BABBLE_NAME, size);

    // Zainicjalizuj mutex i zmienną warunkową w pamięci wspólnej
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&babblespace->babble_mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_cond_init(&babblespace->babble_cond, &cond_attr);
    pthread_condattr_destroy(&cond_attr);

    // Inicjalizuj pozostałe pola struktury
    babblespace->babble_first = 0;
    babblespace->babble_total = 0;

    printf("Utworzono nowy obszar pamięci wspólnej.\n");

    // ... Tutaj możesz dodać kod do pracy z nowym obszarem pamięci wspólnej ...

    // Zwolnij zmapowany obszar pamięci
    if (munmap(babblespace, size) == -1) {
        perror("munmap");
    }

    return 0;
}