#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct msg{
	int pipeid;
	void* ts;  //timestamp
}MSG;

static void* pipe_exec(void*);
static void* term_exec(void*);
void wakeup_threads();

int main()
{
	pthread_t tids[5];
	
	for(int i = 0; i <4; i++) {
		MSG* message = malloc(sizeof(MSG));
		message->pipeid = i;
		pthread_create(&tids[i], NULL, &pipe_exec, message);
	}
	MSG* message = malloc(sizeof(MSG));
	message->pipeid = 4;
	pthread_create(&tids[4], NULL, &term_exec, message);
	
	printf("Simulation ready, press any key to Continue\n");
	getchar();
	wakeup_threads();
	
	sleep(30);

	printf("exiting program.\n");
	return 0;

}


// terminal simulator
static void* term_exec(void* messg){
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);

	MSG* message = messg;
	printf("terminal message\n");

	pthread_mutex_unlock(&mutex);
	return NULL;
}


// pipe process simulators
static void* pipe_exec(void* messg) {
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);


	MSG* message = messg;
	printf("pipe %d message\n", message->pipeid);
	
	pthread_mutex_unlock(&mutex);
	return NULL;
}


void wakeup_threads() {
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
}

