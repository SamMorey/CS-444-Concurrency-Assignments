#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

struct philosophers {
   char philosopherName[30];
   int philosopherSeat;
};
pthread_mutex_t thinkLock;
pthread_mutex_t forks[5]; 


void *think(struct philosophers  *philosopher) {
   int sleepTime = rand() % 20 + 1;
   pthread_mutex_lock(&thinkLock);
   printf("%s \t is thinking... \n", philosopher->philosopherName);
   pthread_mutex_unlock(&thinkLock);
   sleep(sleepTime);
}

void *getForks(struct philosophers *philosopher) {
   int leftFork;
   int rightFork;

   leftFork = philosopher->philosopherSeat;
   rightFork = philosopher->philosopherSeat+1;
   
   //Diogenes grabs the fork to his left first to annoy Plato
   if (strcmp(philosopher->philosopherName, "Diogenes") == 0) {
      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is atempting to grab fork %d\n", philosopher->philosopherName, leftFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_lock(&forks[leftFork]);

      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is atempting to grab fork %d\n", philosopher->philosopherName, rightFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_lock(&forks[rightFork]);
   }
   else {
      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is atempting to grab fork %d\n", philosopher->philosopherName, rightFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_lock(&forks[rightFork]);

      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is atempting to grab fork %d\n", philosopher->philosopherName, leftFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_lock(&forks[leftFork]);
   }
}

void *eat(struct philosophers *philosopher) {
   int sleepTime = rand() % (9 + 1 - 2) + 2;
   pthread_mutex_lock(&thinkLock);
   printf("%s \tis eating\n", philosopher->philosopherName);
   pthread_mutex_unlock(&thinkLock);

   sleep(sleepTime);

   pthread_mutex_lock(&thinkLock);
   printf("%s \t is done eating\n", philosopher->philosopherName);
   pthread_mutex_unlock(&thinkLock);

}

void *putForks(struct philosophers *philosopher) {
   int leftFork;
   int rightFork;

   leftFork = philosopher->philosopherSeat;
   rightFork = philosopher->philosopherSeat+1;
   
   if (strcmp(philosopher->philosopherName, "Diogenes") == 0) {
      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is returning fork %d\n", philosopher->philosopherName, leftFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_unlock(&forks[leftFork]);

      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is returning fork %d\n", philosopher->philosopherName, rightFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_unlock(&forks[rightFork]);
   }
   else {
      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is returning fork %d\n", philosopher->philosopherName, rightFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_unlock(&forks[rightFork]);

      pthread_mutex_lock(&thinkLock); 
      printf("%s \t is returning fork %d\n", philosopher->philosopherName, leftFork);
      pthread_mutex_unlock(&thinkLock);
      pthread_mutex_unlock(&forks[leftFork]);
   }
  
}

void *living(void * philosopher) {
   struct philosophers *theBoys = (struct philosophers *)philosopher;
   for(;;) {
      think(theBoys);
      getForks(theBoys);
      eat(theBoys);
      putForks(theBoys);
   }
}

int main() {
   int i;
   struct philosophers *theBoys;
   theBoys = malloc(sizeof(struct philosophers)*5);

   strcpy(theBoys[0].philosopherName, "Plato"); 
   strcpy(theBoys[1].philosopherName, "Diogenes"); 
   strcpy(theBoys[2].philosopherName, "Aristotle"); 
   strcpy(theBoys[3].philosopherName, "Socrates"); 
   strcpy(theBoys[4].philosopherName, "Epicurus"); 
   for(i = 0; i < 5; i++) {
      theBoys[i].philosopherSeat = i+1;
      printf("The Boy: %s \t HisSeat: %d \n", theBoys[i].philosopherName, theBoys[i].philosopherSeat);
   }
   for(i = 0; i < 5; i++) {
      pthread_mutex_init(&forks[i], NULL);
   } 
   pthread_mutex_init(&thinkLock, NULL);

   pthread_t philosopherActions[5];
   for (i = 0; i < 5; i++) {
      pthread_create(&philosopherActions[i], NULL, living, &theBoys[i]);
   }

   for ( i = 0; i < 5; i++) {
      pthread_join(philosopherActions[i], NULL);
   }
}
