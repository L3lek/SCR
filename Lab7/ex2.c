#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <linux/time.h>

#define VECLEN 1000000
#define NUM_THREADS 4

struct ThreadData {
    double *a;
    double *b;
    int veclen;
    double sum;
};

double get_actual_time_in_us() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1e6 + now.tv_nsec * 1e-3;
}

void *dotprod(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    int start = data->veclen / NUM_THREADS * data->sum;
    int end = data->veclen / NUM_THREADS * (data->sum + 1);

    double mysum = 0.0;

    for (int i = start; i < end; i++) {
        mysum += (data->a[i] * data->b[i]);
    }

    data->sum = mysum;

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i, len;
    double *a, *b;
    pthread_t threads[NUM_THREADS];
    struct ThreadData thread_data[NUM_THREADS];

    len = VECLEN;
    a = (double *)malloc(len * sizeof(double));
    b = (double *)malloc(len * sizeof(double));

    for (i = 0; i < len; i++) {
        a[i] = 1;
        b[i] = a[i];
    }

    for (i = 0; i < NUM_THREADS; i++) {
        thread_data[i].veclen = len;
        thread_data[i].a = a;
        thread_data[i].b = b;
        thread_data[i].sum = i; // thread id is used as a part of the sum

        pthread_create(&threads[i], NULL, dotprod, (void *)&thread_data[i]);
    }

    double time_start = get_actual_time_in_us();

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double final_sum = 0.0;

    for (i = 0; i < NUM_THREADS; i++) {
        final_sum += thread_data[i].sum;
    }

    double time_stop = get_actual_time_in_us();

    /* Print result and release storage */
    printf("Sum =  %f \n", final_sum);
    printf("It took %.3lf microseconds to run.\n", time_stop - time_start);

    free(a);
    free(b);

    return 0;
}
