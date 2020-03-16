// Author: Liz Parker
// OS Programming Assignment 3

#include <stdio.h> // Standard C library functions
#include <stdlib.h> 
#include <pthread.h>
#include <sys/syscall.h> // To use pid_t
#include <unistd.h>
#include <fcntl.h> // To use file control options
#include <sys/stat.h> // To use gettid
#include <sys/types.h> // To use gettid
#include <semaphore.h> // To use semaphores
#include <time.h> // To use gettimeofday()
#include <sys/time.h> // To use gettimeofday()
#include "util.h" // To use dnslookup
#include "queue.h" // To use queue implementation

void* request(void *id);
void* resolve(void *id);

struct thread{
	int numThread;
	int numFiles;
	FILE* threadFile;
	queue* buffer;
	queue* fileQ;
	queue* stillRequests;
	pthread_mutex_t* bufferMutex;
	pthread_mutex_t* outputMutex;
	pthread_mutex_t* inputMutex;
	pthread_mutex_t* servicedMutex;
};
