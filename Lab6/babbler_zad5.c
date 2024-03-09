#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void display_babbles_order(struct babblespace *babblespace) {
    pthread_mutex_lock(&babblespace->babble_mutex);

    while (1){
        pthread_cond_wait(&babblespace->babble_cond, &babblespace->babble_mutex);
        system("clear");

        for (int i = 0; i < babblespace->babble_total; ++i) {
        printf("Komunikat [%d]: %s\n", i+1, babblespace->babbles[(babblespace->babble_first + i) % BABBLE_LIMIT]);
        }

        printf("Wpisz wiadomosc badz q aby wyjsc \n");
    }
    
    pthread_mutex_unlock(&babblespace->babble_mutex);
}

void write_babble(struct babblespace *babble_ptr, const char *user_initials, const char *message) {
    pthread_mutex_lock(&babble_ptr->babble_mutex);

    // Dodaj inicjały użytkownika do komunikatu
    snprintf(babble_ptr->babbles[(babble_ptr->babble_first + babble_ptr->babble_total) % BABBLE_LIMIT],
             BABBLE_LENGTH,
             "%s: %s",
             user_initials,
             message);

    if (babble_ptr->babble_total < BABBLE_LIMIT) {
        babble_ptr->babble_total++;
    } else {
        babble_ptr->babble_first = (babble_ptr->babble_first + 1) % BABBLE_LIMIT;
    }

    pthread_cond_broadcast(&babble_ptr->babble_cond);
    pthread_mutex_unlock(&babble_ptr->babble_mutex);
}

int main() {

    int shm_fd;
    struct babblespace *babblespace;
    char user_initials[] = "NL";

    // Otwórz istniejący obszar pamięci wspólnej
    shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Zmapuj obszar pamięci wspólnej do przestrzeni adresowej procesu
    babblespace = (struct babblespace *)mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (babblespace == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
            display_babbles_order(babblespace);
            exit(EXIT_SUCCESS);
    } else {
        printf("Wpisz wiadomosc badz q aby wyjsc \n");

        char new_babble[BABBLE_LENGTH];
        while (strcmp(new_babble, "q") != 0) {
            new_babble[0]='\0';

            fgets(new_babble, BABBLE_LENGTH, stdin);

            size_t len = strlen(new_babble);
            if (len > 0 && new_babble[len - 1] == '\n') {
                new_babble[len - 1] = '\0';
            }

            if(strcmp(new_babble, "q") != 0) {
            write_babble(babblespace, user_initials, new_babble);
            }
        }
    }

    // Zwolnij zmapowany obszar pamięci
    if (munmap(babblespace, sizeof(struct babblespace)) == -1) {
        perror("munmap");
    }  

    // Zamknij deskryptor pamięci wspólnej
    close(shm_fd);

    return 0;
}
