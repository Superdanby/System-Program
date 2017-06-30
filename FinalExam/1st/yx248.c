#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <stdlib.h>
#include <signal.h>

// volatile int global=0;
pthread_mutex_t mutex;
pthread_t* id;

typedef struct{
    int id, once;
}yee;
int threads;
yee* info;
int total;
// static volatile int run = 1;

int** passed = NULL;
void threadstp(int a)
{
    // printf("^Z\n");
    int cur = 0;
    for(int i = 0; i < threads; i++)
    {
        cur += *passed[i];
    }
    printf("Progress: %lf%\n", (double)cur/(double)total*100);
    // run = 0;
    return;
}

int thread(yee* info) {
    int times = info->once;
    // printf("%d\n", times);
	int cnt = 0;
	struct drand48_data buffer;
	double resultd;
	long resulti;
	srand48_r(clock(), &buffer);
    int i = 0;
    passed[(info->id)] = &i;
    // printf("%d %d\n", info->passed, info->once);
	for (; i < times; i++)
	{
		drand48_r(&buffer, &resultd);
		double xd = resultd;
		lrand48_r(&buffer, &resulti);
		long xi = resulti % 8;
		drand48_r(&buffer, &resultd);
		double yd = resultd;
		lrand48_r(&buffer, &resulti);
		long yi = resulti % 64;
		xd += (double)xi;
		yd += (double)yi;
		if(yd < xd * xd)
			++cnt;
	}
	// pthread_mutex_lock(&mutex);
    // passed += i;
	// pthread_mutex_unlock(&mutex);
	return cnt;
}
int main(int argc, char const *argv[]) {
	//Catches ^C signal to close "Grade" properly.

    assert(argc == 3);
    char *endptr = (char*)malloc(sizeof(char));
	threads = strtol(argv[2], &endptr, 10);
	total = strtol(argv[1], &endptr, 10);
	id = (pthread_t*)malloc(sizeof(pthread_t) * threads);
	passed = (int**)malloc(sizeof(int*) * threads);
    signal(SIGTSTP,threadstp);
	// srand(time(0));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP);
	pthread_mutex_init(&mutex, &attr);

    // int once = total / threads;
	int r = total % threads;
    info = (yee*)malloc(sizeof(yee) * threads);
    for(int i = 0; i < threads; i++)
    {
        info[i].id = i;
        info[i].once = total / threads;
        printf("%d %d\n", info[i].id, info[i].once);
    }
    info[threads - 1].once += r;

	for(int i = 0; i < threads; i++)
		pthread_create(id + i, NULL, (void *) thread, &info[i]);
	// int last = once + r;
	// pthread_create(id + threads - 1, NULL, (void *) thread, &last);

    sigset_t sigset;
    sigfillset(&sigset);
	sigaddset(&sigset, SIGTSTP);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

	double ans = 0;
	void* ret = NULL;
    for(int i = 0; i < threads; i++)
    {
        pthread_join(id[i], &ret);
        ans += (int)ret;
    }
    // return 0;

	printf("yx^2 = %0.8lf\n", ans / (double)total * 8 * 64);
	return (0);
}
