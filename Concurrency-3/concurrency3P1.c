#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t sharedResource;

void* consumeResource() {
	for(;;) {
		int ranNum;
		int threadCount;
		sem_wait(&sharedResource);
		ranNum = rand() % 9 + 2;
		printf("Resource is being used for %d seconds\n", ranNum);
		sleep(ranNum);
		sem_post(&sharedResource);
		while(threadCount != 3) {
			sem_getvalue(&sharedResource, &threadCount);
		}
	}
}

int main(int argc, char **argv) {
	int i = 0;
	pthread_t threads[3];
	sem_init(&sharedResource, 0, 3);
	

	for(i = 0; i < 3; i++) {
		pthread_create(&threads[i], NULL, consumeResource, NULL);
	}
	for(i = 0; i < 3; i++) {
		pthread_join(threads[i], NULL);
	}
	return 0;
}

