#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_N 3
#define MAX_X 3
#define MAX_Y 5
#define MAX_S 5

int teren[MAX_X][MAX_Y];
int tereny_niezajete=MAX_X*MAX_Y;
int zakonczeni_synowie=0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct {
    int id;
    int szanse;
    int zdobyte;
} Syn;

typedef struct {
    Syn synowie[MAX_N];
    int tereny_zdobyte;
} Rejent;

void inicjuj_teren(){
    for (int i = 0; i<MAX_X; ++i){
        for (int j = 0; j<MAX_Y; ++j){
        teren[i][j]=0;
        }
    }
}
int losuj_teren_x(){
    int x = rand() % MAX_X;
    return x;
}

int losuj_teren_y(){
    int y = rand() % MAX_Y;
    return y;
}

void zajmij_teren(int x, int y) {
    teren[x][y] = 1;
    tereny_niezajete--;
}

int sprawdz_teren(int x, int y) {
    if(teren[x][y]==1){
        return 0;
    }else
    return 1;
}

void* syn_losuj(void* arg) {
    Rejent* rejent = (Rejent*)arg;
    int syn_id = -1;

    pthread_mutex_lock(&mutex);
    syn_id = rejent->synowie[0].id;
    rejent->synowie[0].id++;
    pthread_mutex_unlock(&mutex);

    int szanse = rejent->synowie[syn_id].szanse;
    rejent->synowie[syn_id].zdobyte=0;

    while (tereny_niezajete > 0 && szanse > 0) {
        int teren_x, teren_y;
        teren_x=losuj_teren_x();
        teren_y=losuj_teren_y();

        pthread_mutex_lock(&mutex);
        if (sprawdz_teren(teren_x, teren_y)) {
            zajmij_teren(teren_x, teren_y);
           //printf("Syn %d zdobył terytorium: (%d, %d)\n", syn_id, teren_x , teren_y);
           rejent->synowie[syn_id].zdobyte++;
        } else {
           // printf("Syn %d trafił na zajęty teren. Strata jednej szansy.\n", syn_id);
        }
        pthread_mutex_unlock(&mutex);

        szanse--;
    }
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    zakonczeni_synowie++;
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void* rejent_wybor(void* arg) {
    inicjuj_teren();
    Rejent* rejent = (Rejent*)arg;
    pthread_t synowie_watki[MAX_N];
    rejent->tereny_zdobyte=0;

    for (int i = 0; i < MAX_N; ++i) {
        rejent->synowie[i].id = i;
        rejent->synowie[i].szanse = MAX_S;
        pthread_create(&synowie_watki[i], NULL, syn_losuj, (void*)rejent);
    }
   
    for (int i = 0; i < MAX_N; ++i) {
        pthread_mutex_lock(&mutex);
        while (zakonczeni_synowie<MAX_N) {
            pthread_cond_wait(&cond, &mutex);
        }
        printf("Syn %d zdobył %d terytoriow\n", i, rejent->synowie[i].zdobyte);
        rejent->tereny_zdobyte += rejent->synowie[i].zdobyte;
        pthread_mutex_unlock(&mutex);
    }

    for (int i = 0; i < MAX_N; ++i) {
        pthread_join(synowie_watki[i], NULL);
    }
    int tereny_wolne= (MAX_X*MAX_Y)-rejent->tereny_zdobyte;
    printf("\nLiczba terytoriów niezajętych: %d\n", tereny_wolne);
    pthread_exit(NULL);
}

int main() {
    srand((unsigned)time(NULL));

    Rejent rejent;
    pthread_t rejent_watek;
   

    pthread_create(&rejent_watek, NULL, rejent_wybor, (void*)&rejent);
    pthread_join(rejent_watek, NULL);

    return 0;
}