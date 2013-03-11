#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cycle.h>

typedef struct {
    double *a;
    double *b;
    double sum;
    int veclen;
} DOTDATA;

typedef struct {
    int n;
    int i;
} params;

int NUMTHRDS;
int VECLEN;
DOTDATA dotstr;
pthread_t *callThd;
pthread_mutex_t mutexsum;

void *dotprod(void *arg) {

    int i, start, end, len, n;
    long offset;
    double mysum, *x, *y;
    void *status;
    offset = ((params*) arg)->i;
    n = ((params*) arg)->n;
    pthread_attr_t attr;
    params *p;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    while (n > 1) {
        p->n = n / 2;
        p->i = ((params*) arg)->i + p->n;
        if (n % 2 == 1)p->n++;
        n /= 2;
        pthread_create(&callThd[p->i], &attr, dotprod, (void *) p);
    }

    pthread_attr_destroy(&attr);

    len = dotstr.veclen;
    start = offset*len;
    end = start + len;
    x = dotstr.a;
    y = dotstr.b;


    for (i = start; i < end; i++) {
        pthread_mutex_lock(&mutexsum);
        dotstr.sum += (x[i] * y[i]);
        pthread_mutex_unlock(&mutexsum);
    }
    while (n < ((params*) arg)->n) {
        pthread_join(callThd[offset+n], &status);
        n*=2;
    }

    pthread_exit((void*) 0);
}

int main(int argc, char *argv[]) {
    long i;
    double *a, *b;
    void *status;
    pthread_attr_t attr;

    if (argc != 3) {
        printf("Usage: ./dotprod-basic NUMTHREADS TAMANY_PROBLEMA");
        return 0;
    }

    NUMTHRDS = atoi(argv[1]);
    VECLEN = atoi(argv[2]);

    callThd = (pthread_t*) malloc(NUMTHRDS * sizeof (pthread_t));
    a = (double*) malloc(NUMTHRDS * VECLEN * sizeof (double));
    b = (double*) malloc(NUMTHRDS * VECLEN * sizeof (double));

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

    params *p;
    int n = NUMTHRDS;
    while (n > 1) {
        p->n = n / 2;
        p->i = n;
        if (n % 2 == 1)p->n++;
        n /= 2;
        pthread_create(&callThd[p->i], &attr, dotprod, (void *) p);
    }

    pthread_attr_destroy(&attr);

    for (i = 1; i < NUMTHRDS; i++) {
        pthread_join(callThd[i], &status);
    }

    printf("Sum =  %f \n", dotstr.sum);
    free(a);
    free(b);
    pthread_mutex_destroy(&mutexsum);
    pthread_exit(NULL);
}

