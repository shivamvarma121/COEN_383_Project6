#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct msg{
	int pipeid;	//id generated during creation to track pipes
	char* ts;	//timestamp from pipe/terminal before sending message to main
}MSG;

static void* pipe_exec(void*);	// needs to be written before term
static void* term_exec(void*);	// needs to be written after pipe
void wakeup_threads();	// initiates all threads
char* gettimestamp();	//returns current timestamp, free when done.


int main()
{
	pthread_t tids[5];	// 0,1,2,3 are pipes, 4 is terminal
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


	// needs to grab messages with select() and print timestamp + message when available


	printf("exiting program.\n");
	return 0;

}


// terminal simulator
static void* term_exec(void* messg){
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);

	MSG* message = messg;
	printf("%s terminal message\n", gettimestamp());

	pthread_mutex_unlock(&mutex);
	return NULL;
}


// pipe process simulators
static void* pipe_exec(void* messg) {
	pthread_mutex_lock(&mutex);
	pthread_cond_wait(&cond, &mutex);


	MSG* message = messg;
	printf("%s pipe %d message\n", gettimestamp(), message->pipeid);
	
	pthread_mutex_unlock(&mutex);
	return NULL;
}

char* gettimestamp(){
        struct timeval tp;
        gettimeofday(&tp, 0);
        time_t curtime = tp.tv_sec;
        struct tm *t = localtime(&curtime);
        char *result;
        asprintf(&result, "%02d:%02d:%02d:%03d", t->tm_hour, t->tm_min, t->tm_sec, tp.tv_usec/1000);
        return result;
}


void wakeup_threads() {
        pthread_mutex_lock(&mutex);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
}

