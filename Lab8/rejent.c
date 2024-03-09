#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define N 5
#define X 5
#define Y 5
#define S 3

int territories[X][Y];
int unclaimed_territories;
int sons_finished = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void make_kingdom() {
    for (int i = 0; i < X; ++i) {
        for (int j = 0; j < Y; ++j) {
            territories[i][j] = 0;
        }
    }
    unclaimed_territories = X * Y;
	}

void print_territories() {
    printf("Territories:\n");
    for (int i = 0; i < X; ++i) {
        for (int j = 0; j < Y; ++j) {
            printf("%d ", territories[i][j]);
        }
        printf("\n");
    }
}

void* son_thread(void* arg) {
	int* son_params = (int*)arg;
    int chances = S;
	int claimed_territories = 0;


	srand48(time(NULL) + sons_finished);

    while (unclaimed_territories > 0 && chances > 0) {
        int x = (int)(drand48() * X);
        int y = (int)(drand48() * Y);
 
        pthread_mutex_lock(&mutex);
        if (territories[x][y] == 0) {
            territories[x][y] = sons_finished;
            claimed_territories++;
           	unclaimed_territories--;
        }
		pthread_mutex_unlock(&mutex);

        chances--;
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    sons_finished++;
    pthread_mutex_unlock(&mutex);

    printf("Syn %d zdobył %d terytoriów\n", sons_finished-1, claimed_territories);

    pthread_exit(NULL);
}

void* rejent_thread(void* arg) {
    make_kingdom();
    pthread_t sons[N];

    for (int i = 1; i <= N; ++i) {
        pthread_create(&sons[i], NULL, son_thread, NULL);
    }

    for (int i = 0; i < N; ++i) {
        pthread_mutex_lock(&mutex);
        while (sons_finished < N) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(sons[i], NULL);
    }

    printf("\nLiczba terytoriów niezajętych: %d\n", unclaimed_territories);

    pthread_exit(NULL);
}


int main() {
	pthread_t rejent;

    pthread_create(&rejent, NULL, rejent_thread, NULL);
    pthread_join(rejent, NULL);

	print_territories();

    return 0;
}