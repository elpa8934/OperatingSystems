#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Initialize a queue
int newQueue(queue * q, int size){
	if(size > 0) {
		q->size = size;
		q->elements = malloc(sizeof(char*)*size);
		if(!(q->elements)){
			fprintf(stderr, "Error creating queue - malloc error/n");
			return 1;
		}
		for(int i = 0; i < size; i++){
			q->elements[i] = '\0';
		}
		q->front = 0;
		q->rear = 0;
		q->numElements = 0;
		return q->size;
	}
	else {
		fprintf(stderr, "Error creating queue - invalid size/n");
	}
	return 1;
}


// Check if queue is empty
int queue_is_empty(queue *q){
	if((q->front == q->rear) && (q->elements[q->front] == '\0')){
		return 1;
	} 
	return 0;
}

// Check if queue is full
int queue_is_full(queue *q){
	if((q->front == q->rear) && (q->elements[q->front] != '\0')){
		return 1;
	}
	else{
		return 0;
	}
}

// Add element to queue
int enqueue(queue *q, char* toAdd){
	if(queue_is_full(q)){
		fprintf(stderr, "Error adding to already full queue");
		return 1;
	}
	q->elements[q->rear] = toAdd;
	q->rear = ((q->rear + 1) % q->size);
	q->numElements++;
	return 0;
}

// Remove element from queue and return it
char* dequeue(queue *q){
	if (queue_is_empty(q)){
		return '\0';
	}
	char* dequeued = q->elements[q->front];
	q->elements[q->front] = '\0';
	q->front = ((q->front + 1) % q->size);
	q->numElements--;
	return dequeued;
}

// Return current size of queue
int current_q_size(queue *q){
	return q->numElements;
}

// Free queue's allocated memory
void freeQueue(queue* q){
	while(!queue_is_empty(q)){
		dequeue(q);
	}
	free(q->elements);
}
