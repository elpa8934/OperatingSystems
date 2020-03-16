// Author: Liz Parker
// OS Programming Assignment 3

#include "multi-lookup.h" // Header file

void* requester(void *id){
	// Create variable tid to hold the thread id
	pid_t tid;
	tid = syscall(SYS_gettid);
	
	// Create a local thread object equal to the thread passed by parameter
	struct thread* thread = id;

	int numFilesServiced = 0; // Number of files serviced by thread to report at the end of the function

	// Stop execution of unneeded threads
	if(thread->numThread > thread->numFiles){
		// Protect serviced file during access
		pthread_mutex_lock(thread->servicedMutex);
		fprintf(thread->threadFile, "Thread %d serviced %d file(s)\n", tid, numFilesServiced);
		// Unlock  serviced file after access
		pthread_mutex_unlock(thread->servicedMutex);
		return NULL;
	}


	int finished = 0;
	char* domain;
	char name[1025];
	char* filename;

	while(!queue_is_empty(thread->fileQ)){
		// Mutex to protect multiple threads from requesting the same file from the fileQ
		pthread_mutex_lock(thread->inputMutex);
		if(!queue_is_empty(thread->fileQ)){
			filename = dequeue(thread->fileQ);
		}
		pthread_mutex_unlock(thread->inputMutex);
		// Open and parse the file
		FILE * input_file = fopen(filename, "r");	
		while(fscanf(input_file, "%s", name) > 0){
			while(finished != 1){
				domain = malloc(1025);
				strncpy(domain,name,1025);
				// Protect buffer queue with Mutex Lock
				pthread_mutex_lock(thread->bufferMutex);
				// If buffer queue is full, keep looping to wait for space
				while(queue_is_full(thread->buffer)){
					pthread_mutex_unlock(thread->bufferMutex);
					usleep(rand() % 100);
					pthread_mutex_lock(thread->bufferMutex);
				}
				// Push hostname onto buffer queue
				enqueue(thread->buffer,domain);
				// Unlock buffer queue access
				pthread_mutex_unlock(thread->bufferMutex);
				// Set finished flag to exit while loop
				finished = 1;
			}
			// Reset finished flag for next loop
			finished = 0;
		}
		// Close the file
		fclose(input_file);
		// Increment number of requested input files
		numFilesServiced++;
		// Error Check
		if(input_file == NULL){
			printf("Failed to close input file");
		}
	}
	// Protect serviced file during access
	pthread_mutex_lock(thread->servicedMutex);
	// Print number of files serviced by the thread
	fprintf(thread->threadFile, "Thread %d serviced %d file(s)\n", tid, numFilesServiced);
	// Unlock  serviced file after access
	pthread_mutex_unlock(thread->servicedMutex);
	return NULL;
}

void* resolver(void *id){
	// Create a local thread object equal to the thread passed by parameter
	struct thread* thread = id;

	char ipstr[INET6_ADDRSTRLEN]; // Max length of IP String is INET6_ADDRSTRLEN
	
	char* popped; // Create variable for dequeued domain

	// Loop through domains in buffer queue
	while(!queue_is_empty(thread->buffer) || queue_is_empty(thread->stillRequests) == 0){
		// Lock buffer queue to protect during access
		pthread_mutex_lock(thread->bufferMutex);
		// Access the domain name
		popped = dequeue(thread->buffer);
		if(popped == NULL){
			// If no domain name available, unlock the buffer queue
			pthread_mutex_unlock(thread->bufferMutex);
			// Wait so a domain name may be added before next loop
			usleep(rand() % 100);
		} else {
			// Unlock the buffer queue after domain access
			pthread_mutex_unlock(thread->bufferMutex);
			// Call DNS lookup and error check
			if(dnslookup(popped, ipstr, sizeof(ipstr)) == UTIL_FAILURE){
				strncpy(ipstr, "", sizeof(ipstr));
				fprintf(stderr, "Error: DNS lookup failure on hostname: %s\n", popped);	
			}
			// Protect output file during access
			pthread_mutex_lock(thread->outputMutex);
			// Write domain and IP addres to output file
			fprintf(thread->threadFile, "%s,%s\n", popped,ipstr);
			// Unlock  output file after access
			pthread_mutex_unlock(thread->outputMutex);
		}
		// Free memory allocated to store the domain name
		free(popped);
	}
	return NULL;
}

int main(int argc, char *argv[]){
	// Error checking for program arguments
	if(argc < 6){
		printf("Incorrect number of arguments, need : multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ...]\n");
		return 0;
	}

	// Check for invalid file inputs
	FILE * outputFile = fopen(argv[4], "w");
	if(!outputFile){
		fprintf(stderr, "Invalid Output File");
		return 0;
	}
	FILE * servicedFile = fopen(argv[3], "w");
	if(!servicedFile){
		fprintf(stderr, "Invalid Input File");
		return 0;
	}

	// Declare mutexes
	pthread_mutex_t bufferMutex;
	pthread_mutex_t outputMutex;
	pthread_mutex_t inputMutex;
	pthread_mutex_t servicedMutex;

	// Declare queues
	queue remainingRequests; // value of existing requesters
	queue buff; // bounded buffer
	queue fileQ; // file queue

	// Convert string inputs to integers
	char *holder1 = argv[1];
	char *holder2 = argv[2];
	int numRequests = atoi(holder1);
	int numResolves = atoi(holder2);

	int numInputFiles = argc-5 ;

	pthread_t requests[numRequests]; // Requester threads array
	pthread_t resolvers[numResolves]; // Resolver threads array

	newQueue(&remainingRequests,1); // Queue for existing requests
	newQueue(&buff, 10); // Shared buffer for requester and resolver threads
	newQueue(&fileQ, numInputFiles); // Files to be parsed

	// Initialized mutexes to protect shared resources
	pthread_mutex_init(&bufferMutex, NULL);
	pthread_mutex_init(&outputMutex, NULL);
	pthread_mutex_init(&inputMutex, NULL);
	pthread_mutex_init(&servicedMutex, NULL);

	// Record of existing requestor threads
	enqueue(&remainingRequests, "Requestor threads still exist");
	
	// Open input files and store in fileQ
	int fileIndex = 5;
	char* file;
	for(int i = 0; i < numInputFiles; i++){
		file = argv[fileIndex];
		enqueue(&fileQ, file);
		fileIndex++;
	}

	// Requester threads information
	struct thread requesterInfo[numRequests];
	// Resolver threads information
	struct thread resolverInfo[numResolves];

	//Begin tracking performance
	struct timeval start, end;
	gettimeofday(&start, NULL);

	// Create requester threads and store their information
	for (int i = 0; i < numRequests; i++){
		requesterInfo[i].numThread = i+1;
		requesterInfo[i].numFiles = numInputFiles;
		requesterInfo[i].fileQ = &fileQ;
		requesterInfo[i].buffer = &buff;
		requesterInfo[i].bufferMutex = &bufferMutex;
		requesterInfo[i].inputMutex = &inputMutex;
		requesterInfo[i].servicedMutex = &servicedMutex;
		requesterInfo[i].threadFile = servicedFile;
		pthread_create(&(requests[i]), NULL, requester, &(requesterInfo[i]));
	}
	
	// Create resolver threads and store their information
	for (int i = 0; i < numResolves; i++){
		resolverInfo[i].threadFile = outputFile;
		resolverInfo[i].buffer = &buff;
		resolverInfo[i].bufferMutex = &bufferMutex;
		resolverInfo[i].outputMutex = &outputMutex;
		resolverInfo[i].stillRequests = &remainingRequests;
		pthread_create(&(resolvers[i]), NULL, resolver, &(resolverInfo[i]));
	}

	// Join requestor threads
	for (int i = 0; i < numRequests; i++){
		pthread_join(requests[i], NULL);
	}

	dequeue(&remainingRequests); //All requestor threads have finished

	// Join resolver threads
	for (int i = 0; i < numResolves; i++){
		pthread_join(resolvers[i], NULL);
	}
	
	// Clean up

	//Close Files
	fclose(outputFile);
	fclose(servicedFile);

	// Destroy mutexes	
	pthread_mutex_destroy(&bufferMutex);
	pthread_mutex_destroy(&outputMutex);
	pthread_mutex_destroy(&inputMutex);
	pthread_mutex_destroy(&servicedMutex);

	// Free memory
	freeQueue(&remainingRequests);
	freeQueue(&fileQ);
	freeQueue(&buff);

	// End tracking performance
	gettimeofday(&end, NULL);

	// Print information for performance.txt
	printf("Number for requestor thread = %d\n", numRequests);
	printf("Number for resolver thread = %d\n", numResolves);
	printf("Total run time: %ld\n", ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec));
	return 0;

}




