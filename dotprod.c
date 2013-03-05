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
double *creacio, *lock, *seccioCritica, *unlock;
unsigned long long *td,*destruccio;
DOTDATA dotstr;
pthread_t *callThd;
pthread_mutex_t mutexsum;

void *dotprod(void *arg) {

    int i, start, end, len;
    long offset;
    double mysum, *x, *y;

    offset = (long) arg;
    len = dotstr.veclen;
    start = offset*len;
    end = start + len;
    x = dotstr.a;
    y = dotstr.b;
    mysum = 0;
    for (i = start; i < end; i++) {
        mysum += (x[i] * y[i]);
    }
    

    ticks t = getticks();
    pthread_mutex_lock(&mutexsum);
    lock[offset] = elapsed(getticks(), t);

    t = getticks();
    dotstr.sum += mysum;
    seccioCritica[offset] = elapsed(getticks(), t);

    t = getticks();
    pthread_mutex_unlock(&mutexsum);
    unlock[offset] = elapsed(getticks(), t);

    td[offset] = (unsigned long long)getticks();
    pthread_exit((void*) 0);
}

int main(int argc, char *argv[]) {

    struct timeval tim;
    gettimeofday(&tim, NULL);
    double tempsInicial = tim.tv_sec * 1000000 + tim.tv_usec;
    ticks ticksInicials = getticks();

    long i;
    double *a, *b;
    void *status;
    ticks t;
    pthread_attr_t attr;
    
    if (argc != 3) {
        printf("Usage: ./dotprod NUMTHREADS VECLEN");
        exit(0);
    }
    NUMTHRDS = atoi(argv[1]);
    VECLEN = atoi(argv[2]);
        
    creacio = (double*) malloc(NUMTHRDS * sizeof (double));
    lock = (double*) malloc(NUMTHRDS * sizeof (double));
    seccioCritica = (double*) malloc(NUMTHRDS * sizeof (double));
    unlock = (double*) malloc(NUMTHRDS * sizeof (double));
    destruccio = (unsigned long long*) malloc(NUMTHRDS * sizeof (unsigned long long));
    a = (double*) malloc(NUMTHRDS * VECLEN * sizeof (double));
    b = (double*) malloc(NUMTHRDS * VECLEN * sizeof (double));
    td = (unsigned long long*) malloc(NUMTHRDS * sizeof (unsigned long long));
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
    for (i = 0; i < NUMTHRDS; i++) {
        t = getticks();
        pthread_create(&callThd[i], &attr, dotprod, (void *) i);
        creacio[i] = elapsed(getticks(), t);
    }

    pthread_attr_destroy(&attr);
    for (i = 0; i < NUMTHRDS; i++) {
        pthread_join(callThd[i], &status);
        destruccio[i] = (unsigned long long)getticks();
    }

    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);

    double ticksTotals = elapsed(getticks(), ticksInicials);
    gettimeofday(&tim, NULL);
    double tempsTotal = (tim.tv_sec * 1000000 + tim.tv_usec) - tempsInicial;
    double freq = tempsTotal / ticksTotals;
    int cont;
    printf("#Thread\tCreacio\t\tLock\t\tSC\t\tUnlock\t\tDestr\t\ttd\n",freq);
    for (cont = 0; cont < NUMTHRDS; cont++) {
        printf("%i\t%f\t%f\t%f\t%f\t%llu\t%llu\n", cont, creacio[cont]*freq, lock[cont]*freq, seccioCritica[cont]*freq, unlock[cont]*freq, destruccio[cont],td[cont]);
    }
    pthread_exit(NULL);

}

