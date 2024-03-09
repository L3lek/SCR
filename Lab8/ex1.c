#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define N 3
#define X 5
#define Y 2
#define S 3

int territories[X][Y];
int unclaimed_territories;
int claimed_territories = 0;
int territories_left=0;
int sons_finished = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void initialize_territories() {
    for (int i = 0; i < X; ++i) {
        for (int j = 0; j < Y; ++j) {
            territories[i][j] = 0;
        }
    }
    unclaimed_territories = X * Y;
}

//void print_territories() {
//    printf("Territories:\n");
//    for (int i = 0; i < X; ++i) {
//        for (int j = 0; j < Y; ++j) {
//            printf("%d ", territories[i][j]);
//        }
//        printf("\n");
//    }
//    printf("\n");
//}

void* son_thread(void* arg) {
    long son_id = (long)arg;
    srand48(time(NULL) + son_id); // Initialize random seed for each son

    for (int i = 0; i < S; ++i) {
        int x, y;

        pthread_mutex_lock(&mutex);

            x = (int)(drand48() * X);
            y = (int)(drand48() * Y);

            if (territories[x][y] == 0) {
                territories[x][y] = son_id;
                claimed_territories++;
                unclaimed_territories--;
        }
        pthread_mutex_unlock(&mutex);
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void* regent_thread(void* arg) {
    pthread_mutex_lock(&mutex);

    while (sons_finished < N) {
        pthread_cond_wait(&cond, &mutex);
        sons_finished++;
        printf("Son %ld claimed %d territories\n", sons_finished, claimed_territories-territories_left);
        territories_left=claimed_territories;
    }

    pthread_mutex_unlock(&mutex);

    printf("Regent found %d unclaimed territories\n", unclaimed_territories);
    pthread_exit(NULL);
}

int main() {

    pthread_t sons[N], regent;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    initialize_territories();
	
	pthread_create(&regent, &attr, regent_thread, NULL);

    for (long i = 0; i < N; ++i) {
        pthread_create(&sons[i], &attr, son_thread, (void*)i);
    }

    

    for (int i = 0; i < N; ++i) {
        pthread_join(sons[i], NULL);
    }

    pthread_join(regent, NULL);

    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
