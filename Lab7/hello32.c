/******************************************************************************
* FILE: hello32.c
* DESCRIPTION:
*   A "hello world" Pthreads program which creates a large number of 
*   threads per process.  A sleep() call is used to insure that all
*   threads are in existence at the same time.  Each Hello thread does some
*   work to demonstrate how the OS scheduler behavior affects thread 
*   completion order.
* AUTHOR: Blaise Barney
* LAST REVISED: 01/29/09
******************************************************************************/
//#include <pthread.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <math.h>
//#include <unistd.h>
//#include <string.h>
//#define NTHREADS	10

//void *Hello(void *threadid)
//{
//   int i;
//   double result=0.0;
//   sleep(3);
//   for (i=0; i<10000; i++) {
//     result = result + sin(i) * tan(i);
//     }
//   printf("%ld: Hello World!\n", threadid);
//   pthread_exit(NULL);
//}

//int main(int argc, char *argv[])
//{
//pthread_t threads[NTHREADS];
//int rc; 
//long t;
//for(t=0;t<NTHREADS;t++){
//  rc = pthread_create(&threads[t], NULL, Hello, (void *)t);
//  if (rc){
//    printf("ERROR: return code from pthread_create() is %d\n", rc);
//    printf("Code %d= %s\n",rc,strerror(rc));
//    exit(-1);
//    }
//   }
//printf("main(): Created %ld threads.\n", t);
//pthread_exit(NULL);
//}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <bits/pthreadtypes.h>

#define NTHREADS 10

pthread_barrier_t barrier;

void *Hello(void *threadid) {
    int i;
    double result = 0.0;

    pthread_barrier_wait(&barrier); // Czekaj na pozostałe wątki

    sleep(3);

    for (i = 0; i < 10000; i++) {
        result = result + sin(i) * tan(i);
    }

    printf("%ld: Hello World!\n", threadid);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NTHREADS];
    int rc;
    long t;

    pthread_barrier_init(&barrier, NULL, NTHREADS + 1); // Inicjalizuj barierę

    for (t = 0; t < NTHREADS; t++) {
        rc = pthread_create(&threads[t], NULL, Hello, (void *)t);
        if (rc) {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            printf("Code %d= %s\n", rc, strerror(rc));
            exit(-1);
        }
    }

    pthread_barrier_wait(&barrier); // Odblokuj wątki

    for (t = 0; t < NTHREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_barrier_destroy(&barrier); // Zniszcz barierę

    printf("main(): Created %ld threads.\n", t);
    pthread_exit(NULL);
}