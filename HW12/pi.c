#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <stdlib.h>

// volatile int global=0;
// pthread_mutex_t mutex;

int thread(int* arg) {
	int cnt = 0;
	int times = *arg;
	struct drand48_data buffer;
	double result;
	srand48_r(clock(), &buffer);
	for (int i = 0; i < times; i++)
	{
		drand48_r(&buffer, &result);
		double x = result;
		drand48_r(&buffer, &result);
		double y = result;
		if(x * x + y * y <= 1)
		++cnt;
	}
	// pthread_mutex_lock(&mutex);
	// 	global += cnt;
	// pthread_mutex_unlock(&mutex);
	// printf("%d\n", global);
	printf("%d\n", cnt);
	return cnt;
}
int main(int argc, char const *argv[]) {
    assert(argc == 3);
    char *endptr = (char*)malloc(sizeof(char));
	int threads = strtol(argv[2], &endptr, 10);
	int total = strtol(argv[1], &endptr, 10);
	pthread_t* id = (pthread_t*)malloc(sizeof(pthread_t) * threads);
    // pthread_attr_t attr;
    // pthread_attr_init(&attr);
	srand(time(0));
    // pthread_mutexattr_t attr;
    // pthread_mutexattr_init(&attr);
    // pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ADAPTIVE_NP);
	// pthread_mutex_init(&mutex, &attr);
	int once = total / threads;
	int r = total % threads;
	// int* times = &once;
	for(int i = 0; i < threads - 1; i++)
		pthread_create(id + i, NULL, (void *) thread, &once);
	once += r;
	pthread_create(id + threads - 1, NULL, (void *) thread, &once);
	double ans = 0;
	void* ret = NULL;
	for(int i = 0; i < threads; i++)
	{
		pthread_join(id[i], &ret);
		ans += (int)ret;
	}
	printf("pi = %0.8lf\n", ans / (double)total * 4);
	return (0);
}
