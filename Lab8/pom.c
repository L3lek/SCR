#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_N 3
#define MAX_X 2
#define MAX_Y 5
#define MAX_S 3

int teren[MAX_X][MAX_Y];
int tereny_niezajete = MAX_X * MAX_Y;
int zakonczeni_synowie = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void inicjuj_teren() {
    for (int i = 0; i < MAX_X; ++i) {
        for (int j = 0; j < MAX_Y; ++j) {
            teren[i][j] = 0;
        }
    }
}

int losuj_teren_x() {
    return rand() % MAX_X;
}

int losuj_teren_y() {
    return rand() % MAX_Y;
}

void zajmij_teren(int x, int y) {
    teren[x][y] = 1;
    tereny_niezajete--;
}

int sprawdz_teren(int x, int y) {
    return teren[x][y] == 0;
}

void* syn_losuj(void* arg) {
    int* syn_params = (int*)arg;
    int syn_id = syn_params[0];
    int szanse = syn_params[1];
    int zdobyte_tereny = 0;

    while (tereny_niezajete > 0 && szanse > 0) {
        int teren_x = losuj_teren_x();
        int teren_y = losuj_teren_y();

        pthread_mutex_lock(&mutex);
        if (sprawdz_teren(teren_x, teren_y)) {
            zajmij_teren(teren_x, teren_y);
            zdobyte_tereny++;
        }
        pthread_mutex_unlock(&mutex);

        szanse--;
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    zakonczeni_synowie++;
    pthread_mutex_unlock(&mutex);

    printf("Syn %d zdobył %d terytoriów\n", syn_id, zdobyte_tereny);

    pthread_exit(NULL);
}

void* rejent_wybor(void* arg) {
    inicjuj_teren();
    int tereny_zdobyte = 0;
    pthread_t synowie_watki[MAX_N];

    for (int i = 0; i < MAX_N; ++i) {
        int syn_params[2] = {i, MAX_S};
        pthread_create(&synowie_watki[i], NULL, syn_losuj, (void*)syn_params);
    }

    for (int i = 0; i < MAX_N; ++i) {
        pthread_mutex_lock(&mutex);
        while (zakonczeni_synowie < MAX_N) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < MAX_N; ++i) {
        pthread_join(synowie_watki[i], NULL);
    }

    tereny_zdobyte = (MAX_X * MAX_Y) - tereny_niezajete;
    printf("\nLiczba terytoriów niezajętych: %d\n", tereny_niezajete);

    pthread_exit(NULL);
}

int main() {
    srand((unsigned)time(NULL));

    pthread_t rejent_watek;

    pthread_create(&rejent_watek, NULL, rejent_wybor, NULL);
    pthread_join(rejent_watek, NULL);

    return 0;
}