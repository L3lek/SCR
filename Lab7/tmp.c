#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int N;               /* liczba liczb pierwszych do znalezienia (arg. wywołania) */
int sprawdz = 2;      /* następna liczba do sprawdzania */
int znalezione = 0;   /* liczba znalezionych i zsumowanych liczb pierwszych */
long suma = 0;        /* to będzie obliczona suma liczb pierwszych */
pthread_mutex_t muteks_pobierania;
pthread_mutex_t muteks_sumowania;

int is_prime(int num) {
    if (num < 2) {
        return 0;
    }
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) {
            return 0;
        }
    }
    return 1;
}

void *watek(void *arg) {
    while (znalezione < N) {
        int local_num;

        pthread_mutex_lock(&muteks_pobierania);
        local_num = sprawdz++;
        pthread_mutex_unlock(&muteks_pobierania);
		
		pthread_mutex_lock(&muteks_sumowania);
        if (is_prime(local_num)) {
            suma += local_num;
            znalezione++;
        }
		pthread_mutex_unlock(&muteks_sumowania);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Użycie: %s N K\n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]);
    int K = atoi(argv[2]);

    if (N <= 0 || K <= 0) {
        printf("N i K muszą być liczbami dodatnimi.\n");
        return 1;
    }

    pthread_t threads[K];
    pthread_mutex_init(&muteks_sumowania, NULL);
    pthread_mutex_init(&muteks_pobierania, NULL);

    for (int i = 0; i < K; i++) {
        pthread_create(&threads[i], NULL, watek, NULL);
    }

    for (int i = 0; i < K; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Suma pierwszych %d liczb pierwszych wynosi: %ld\n", N, suma);

    return 0;
}
