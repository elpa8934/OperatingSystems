#include <stdio.h>

typedef struct queue {
	char* * elements; // Array for values in queue
	int numElements;
	int front; 
	int rear;
	int size;
}queue;

int newQueue(queue* q, int size);
int queue_is_empty(queue *q);
int queue_is_full(queue *q);
int enqueue(queue* q, char* toAdd);
char* dequeue(queue* q);
int current_queue_size(queue* q);
void freeQueue(queue* q);

