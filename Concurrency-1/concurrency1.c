#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "mt19937ar.h"

struct sharedBuffer {
   int printedValue;
   int workTime;
};
struct threadController {
   int currentIndxConsumer;
   int currentIndxProducer;
   int rngWhich;
   struct sharedBuffer buff[32];
   pthread_mutex_t lock;
   pthread_cond_t consumerThread;
   pthread_cond_t producerThread;
};

struct threadController overloard;

int ranGen(int lower, int upper) { 
   int ranNum = 0;
   if (overloard.rngWhich == 1) {
      __asm__ __volatile__("rdrand %0":"=r"(ranNum));
   }
   else {
      ranNum = (int)genrand_int32();
   }
   
   ranNum = abs(ranNum %(upper - lower));
   if(ranNum < lower) {
      return lower;
   }
   return ranNum;
}

void* producerThread(void *arg) {
   for(;;) {
      pthread_mutex_lock(&overloard.lock);

      if(overloard.currentIndxProducer == 31) {
         printf("Shared Buffer at max size Thread blocking... \n");
         pthread_cond_signal(&(overloard.consumerThread));
         pthread_cond_wait(&(overloard.producerThread), &overloard.lock);
      }
      overloard.buff[overloard.currentIndxProducer].printedValue = ranGen(0, 100);
      overloard.buff[overloard.currentIndxProducer].workTime = ranGen(2, 12);
      overloard.currentIndxProducer++;
      printf("Item produced at: %d \n", overloard.currentIndxProducer);
      sleep(ranGen(3, 10));


      pthread_cond_signal(&(overloard.consumerThread));
      pthread_cond_wait(&(overloard.producerThread), &overloard.lock);
      
      if(overloard.currentIndxProducer >= 31) {
         overloard.currentIndxProducer = 0;
      }

      pthread_mutex_unlock(&overloard.lock);
   }
}

void* consumerThread(void *arg) {
   for(;;) {
     pthread_mutex_lock(&overloard.lock);
     
     if(overloard.currentIndxConsumer >= 31) {
       overloard.currentIndxConsumer = 0;
     }

     pthread_cond_signal(&(overloard.producerThread));
     pthread_cond_wait(&(overloard.consumerThread), &overloard.lock);

     if(overloard.currentIndxProducer == 0) {
        printf("Shared Buffer is empty, consumer Thread blocking... \n");
        pthread_cond_wait(&(overloard.consumerThread), &overloard.lock);
     }
     printf("Consuming from Buffer at %d \n Number Consumed: %d \n", overloard.currentIndxConsumer, overloard.buff[overloard.currentIndxConsumer].printedValue);
     sleep(overloard.buff[overloard.currentIndxConsumer].workTime);
     printf("Slept for %d seconds \n", overloard.buff[overloard.currentIndxConsumer].workTime);
     //Delete Consumed value
     overloard.buff[overloard.currentIndxConsumer].printedValue = 0;
     overloard.buff[overloard.currentIndxConsumer].workTime = 0;
     overloard.currentIndxConsumer++;
     
     pthread_mutex_unlock(&overloard.lock); 
   }
}

int whichRNG()
{

	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;

	char vendor[13];


	eax = 0x01;

	__asm__ __volatile__(
	                     "cpuid;"
	                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
	                     : "a"(eax)
	                     );
	
	if(ecx & 0x40000000){
		//use rdrand
		printf("Using Rrand\n");
      		overloard.rngWhich = 1;
	}
	else{
		//use mt19937
		printf("Using mt19937\n");
      		overloard.rngWhich = 0;
	}

	return 0;
}
int main(int argc, char **argv) {
   whichRNG();
   int threadNumber = 1;
   int i;
   //Quick check for Ran Gen function upper bound is +3 of desired upper bound
   /*for(i=0; i <100; i++) {
	printf("One %d \n", ranGen(2, 12));
	printf("Two: %d \n", ranGen(3, 10));
   }*/
   if( argc <= 1) {
      printf(" No command line argument given defualt # of pairs is 1, include a number to have more pairs.");
   }
   else {
      threadNumber = atoi(argv[1]);
   }
   pthread_t threads[2 * threadNumber];

   for(i = 0; i < threadNumber; i++) {
      pthread_create(&threads[i], NULL, consumerThread, NULL);
      pthread_create(&threads[i+1], NULL, producerThread, NULL);  
   }

   for(i = 0; i < threadNumber; i++) {
      pthread_join(threads[i], NULL);
   }
   return 0;

}
