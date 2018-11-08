#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t inserterActive;
sem_t searcherActive;
sem_t deleterActive;

sem_t searchers;
sem_t inserters;
struct node {
	int val;
	struct node * next;
};




void printList(struct node * head) {
	struct node * current;
	current = (struct node *)malloc(sizeof(struct node));
	current = head;
	int i = 0;
	while(current != NULL) {
		i++;
		printf("List at %d:\t%d\n", i, current->val);
	       	current = current->next;
	}	
}

void addToList(struct node * head) {
	int ranNum = rand() % 11;
	struct node * current;
	current = (struct node *)malloc(sizeof(struct node));
	current = head;
	while(current != NULL &&current->next != NULL) {
		current = current->next;
	}
	current->next = malloc(sizeof(struct node));
	current->next->val = ranNum;
	current->next->next = NULL;
}

void removeFromList(struct node * head) {
	int i = 0;
	int size = 0;
	int ranNum;
	struct node * current = head;
	struct node * temp_node = NULL;
	
	while(current != NULL) {
		size++;
	       	current = current->next;
	}
	current = head;
	printf("%d",size);

	ranNum = (rand() % (size - 1 + 1)) + 1;

	for(i = 0; i < ranNum-1; i++) {
		current = current->next;
	}

	temp_node = current->next;
	if(current != NULL && temp_node!= NULL) {
		current->next = temp_node->next;
		free(temp_node);
	}

}

void *insertThread(void * n) {
	struct node * head = (struct node *)n;
	while(1) {
	sem_wait(&inserterActive);
	sem_wait(&deleterActive);
	printf("Adding an item to the list\n");
	addToList(head);
	sem_post(&inserterActive);
	sem_post(&deleterActive);

	}
}

void *searchThread(void * n) {
	struct node * head = (struct node *)n;
	while(1) {
	sem_wait(&searcherActive);
	sem_wait(&deleterActive);
	printf("Printing the list: \n");
	printList(head);
	sem_post(&searcherActive);
	sem_post(&deleterActive);
	}
}

void *deleteThread(void * n) {
	struct node * head = (struct node *)n;
	while(1) {
	sem_wait(&inserterActive);
	sem_wait(&searcherActive);
	sem_wait(&deleterActive);
	printf("Deleter is deleting and item from the list\n");
	removeFromList(head);
	sem_post(&inserterActive);
	sem_post(&searcherActive);
	sem_post(&deleterActive);
	}
}

int main(int argc, char **argv) { 
	int i;
	sem_init (&inserterActive, 0, 1);
	sem_init (&searcherActive, 0 , 1);
	sem_init (&deleterActive, 0 , 1);
	sem_init (&searchers, 0 , 4);
	sem_init (&inserters, 0, 1);
	
      	struct node * head = NULL;
	head = (struct node*)malloc(sizeof(struct node));
	head->val = 1;
	head->next = NULL;

	pthread_t inserterThreads[2];
	pthread_t searcherThreads[3];
	pthread_t deleterThreads[2];

	for (i = 0; i < 2; i++) {
		pthread_create(&inserterThreads[i], NULL, insertThread, head);
	}
	for (i = 0; i < 3; i++) {
		pthread_create(&searcherThreads[i], NULL, searchThread, head);
	}
	for (i = 0; i < 2; i++) {
		pthread_create(&deleterThreads[i], NULL, deleteThread, head);
	}
	for (i = 0; i < 2; i++) {
		pthread_join(inserterThreads[i], NULL);
	}
	for (i = 0; i < 3; i++) {
		pthread_join(searcherThreads[i], NULL);
	}
	for (i = 0; i < 2; i++) {
		pthread_join(deleterThreads[i], NULL);
	}
	return 0;
}
