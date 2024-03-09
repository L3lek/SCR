#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>

#define BABBLE_NAME "/Y01-42e"
#define BABBLE_MODE 0777
#define BABBLE_LIMIT 32
#define BABBLE_LENGTH 80

#define USER_NAME "Jakub Gil: "

struct babblespace {
    pthread_mutex_t babble_mutex;
    pthread_cond_t babble_cond;
    int babble_first, babble_total;
    char babbles[BABBLE_LIMIT][BABBLE_LENGTH];
};

struct babblespace *shared_memory;


void add_babble(struct babblespace *shared_memory, const char *message) {
    pthread_mutex_lock(&(shared_memory->babble_mutex));
    printf("AAAA\n");

    int index = (shared_memory->babble_first + shared_memory->babble_total) % BABBLE_LIMIT;
    printf("AAAA\n");
    strncpy(shared_memory->babbles[index], message, (BABBLE_LENGTH - 1) + sizeof(USER_NAME));
    shared_memory->babbles[index][BABBLE_LENGTH - 1] = '\0';

    if (shared_memory->babble_total < BABBLE_LIMIT) {
        shared_memory->babble_total++;
    } else {
        shared_memory->babble_first = (shared_memory->babble_first + 1) % BABBLE_LIMIT;
    }

    pthread_cond_broadcast(&(shared_memory->babble_cond)); //Zasygnalizuj dodanie nowego komunikatu 

    pthread_mutex_unlock(&(shared_memory->babble_mutex));

}

void* printf_bubbles() {
    
    pthread_mutex_lock(&(shared_memory->babble_mutex));

    while (1) {
        pthread_cond_wait(&(shared_memory->babble_cond), &(shared_memory->babble_mutex));
        system("clear");

        for (int i = 0; i < shared_memory->babble_total; ++i) {
            printf("[%d] %s\n", i, shared_memory->babbles[i]);
        }

        printf("Wpisz wiadomosc badz q aby wyjsc \n");
    }

    pthread_mutex_unlock(&(shared_memory->babble_mutex));
}


int main() {
    int shm_fd;

    shm_fd = shm_open(BABBLE_NAME, O_RDWR, BABBLE_MODE);
    shared_memory = (struct babblespace *)mmap(NULL, sizeof(struct babblespace), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    char user_input[BABBLE_LENGTH];
    
    int process_PID = fork(); // Tworzenie wielu procesów

    if (process_PID == 0){ // Dziecko
        printf_bubbles();
    }
    else if (process_PID == -1)
    {
        printf(" Nie udalo sie sklonowac procesu\n ");
        exit(-1);
    }
    else{
        printf("Wpisz wiadomosc badz q aby wyjsc \n");

        char message[11 + BABBLE_LENGTH];
        while (strcmp(user_input, "q") != 0) {
            message[0] = '\0'; // Wyczysc tablice przed nadaniem nowego komunikatu
            scanf("%s",user_input);
            
            strcat(message, USER_NAME);
            strcat(message, user_input);

            if (strcmp(user_input, "q") != 0) {
                add_babble(shared_memory, message);
            }
        }
    }
	
    system("clear");
    for (int i = 0; i < shared_memory->babble_total; ++i) {
        printf("[%d] %s\n", i, shared_memory->babbles[i]);
    }

    close(shm_fd);

    return 0;
}
