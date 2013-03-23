#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cycle.h>
#include <sys/time.h>

typedef struct {
    double *a;
    double *b;
    double sum;
    int veclen;
} DOTDATA;

int NUMTHRDS;
int VECLEN;
DOTDATA dotstr;
pthread_t *callThd;
pthread_mutex_t mutexsum;

double convert(double tempsTotal,double ticksTotals, double t){
    return(tempsTotal/ticksTotals)*t;
}

void *dotprod(void *arg) {

    int i, start, end, len;
    long offset;
    double mysum, *x, *y;

    offset = (long)arg;
    len = dotstr.veclen;
    start = offset*len;
    end = start + len;
    x = dotstr.a;
    y = dotstr.b;
    mysum = 0;
    for (i = start; i < end; i++) {
        mysum += (x[i] * y[i]);
    }
    
    pthread_mutex_lock(&mutexsum);
    dotstr.sum += mysum;
    pthread_mutex_unlock(&mutexsum);

    pthread_exit((void*) 0);
}

int main(int argc, char *argv[]) {

    struct timeval tim;
    ticks ticksInicials = getticks();
    gettimeofday(&tim, NULL);
    double tempsInicial = tim.tv_sec * 1000000 + tim.tv_usec;

    long i;
    double *a, *b;
    void *status;
    pthread_attr_t attr;
    
    if (argc != 3) {
        printf("Usage: ./dotprod NUMTHREADS VECLEN");
        exit(0);
    }
    NUMTHRDS = atoi(argv[1]);
    VECLEN = atoi(argv[2]);
    
    a = (double*) malloc(NUMTHRDS * VECLEN * sizeof (double));
    b = (double*) malloc(NUMTHRDS * VECLEN * sizeof (double));
    callThd = (pthread_t*) malloc(NUMTHRDS * sizeof (pthread_t));

    for (i = 0; i < VECLEN * NUMTHRDS; i++) {
        a[i] = 1;
        b[i] = a[i];
    }

    dotstr.veclen = VECLEN;
    dotstr.a = a;
    dotstr.b = b;
    dotstr.sum = 0;

    pthread_mutex_init(&mutexsum, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    ticks t1 = getticks();
    int n = NUMTHRDS;
    for (i = 0; i < NUMTHRDS; i++) {
        pthread_create(&callThd[i], &attr, dotprod,(void *)i);
    }

    pthread_attr_destroy(&attr);
    for (i = 0; i < NUMTHRDS; i++) {
        pthread_join(callThd[i], &status);
    }
    ticks t2 = getticks();

    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);
    ticks ticksFinals = getticks();
    gettimeofday(&tim, NULL);
    double tempsTotal = (tim.tv_sec * 1000000 + tim.tv_usec) - tempsInicial;
    double ticksTotals = elapsed(ticksFinals, ticksInicials);
    
    printf("fase sequencial inicial: %f\n",convert(tempsTotal,ticksTotals,elapsed(t1,ticksInicials)));
    printf("fase sequencial final: %f\n",convert(tempsTotal,ticksTotals,elapsed(ticksFinals,t2)));
    pthread_exit(NULL);
}

