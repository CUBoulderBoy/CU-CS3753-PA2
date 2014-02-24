/*
 * File: multi-lookup.c
 * Author: Christopher Jordan
 * Project: CSCI 3753 Programming Assignment 2
 * Create Date: 2014/02/22
 * Modify Date: 2014/02/22
 * Description: Program allowing multi-threaded lookup for domain name to IP addresses
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#include "queue.h"
#include "util.h"
#include "multi-lookup.h"

#define MINARGS 3
#define MAXARGS 12
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 10

bool input_done = false;

int main(int argc, char *argv[]){
	/* Check Arguments */
    if(argc < MINARGS){
    	//If there isn't at least 1 input and 1 output, error
		fprintf(stderr, "Not Enough Arguments: %d\n", (argc - 1));
		fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
    }
    if(argc > MAXARGS){
    	//If there are more than 10 input files, error out
		fprintf(stderr, "To Many Arguments: %d\n", (argc - 1));
		fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
		return EXIT_FAILURE;
    }

    /* Create Thread Pools */
    int i;
    int rc;
    int req_size = argc-2;
    pthread_t req_threads[req_size];
    pthread_t res_threads[MAX_RESOLVER_THREADS];

    /* Initialize RAND with time */
    time_t t;
    srand((unsigned) time(&t));

    /* Open and Protect Output File */
    FILE* output = NULL;
    output = fopen(argv[(argc-1)], "w");
    if(!output){
		perror("Error Opening Output File");
		return EXIT_FAILURE;
    }
    pthread_mutex_t out_mutex = PTHREAD_MUTEX_INITIALIZER;

    /* Create and Protect Mutex Queue */
    queue mut_queue;
    queue_init(&mut_queue, QUEUEMAXSIZE);
    pthread_mutex_t que_mutex = PTHREAD_MUTEX_INITIALIZER;

    /* Spawn Requester Threads */
    requester_thread_args req_pool[req_size];
    for (i = 0; i < req_size; i++){
    	//Set vars in struct to proper arguements
    	//Will all be same except file name
    	req_pool[i].req_queue = &mut_queue;
    	req_pool[i].file_name = argv[i+1];
    	req_pool[i].que_mutex = &que_mutex;

    	//Create actual thread, error if thread cannot be created
    	rc = pthread_create(&(req_threads[i]), NULL, requester, (void *)&(req_pool[i]));
    	if (rc){
    	    printf("ERROR; return code from pthread_create() is %d\n", rc);
    	    exit(EXIT_FAILURE);
    	}
    }

    /* Spawn Resolver Threads */
    resolver_thread_args res_pool;
    
    //Set vars for all resolver threads
    res_pool.req_queue = &mut_queue;
    res_pool.outputfp = output;
    res_pool.que_mutex = &que_mutex;
    res_pool.out_mutex = &out_mutex;

    //Create all resolver threads
    for (i = 0; i < MAX_RESOLVER_THREADS; i++){
    	rc = pthread_create(&(res_threads[i]), NULL, resolver, (void *)&res_pool);
    	if (rc){
    	    printf("ERROR; return code from pthread_create() is %d\n", rc);
    	    exit(EXIT_FAILURE);
    	} 
    }

    /* Join requester threads */
    for (i = 0; i < req_size; i++){
    	rc = pthread_join(req_threads[i], NULL);

    	//If a thread join error occurs, print the error
    	if (rc){
    		fprintf(stderr, "Error: pthread_join on requester thread returned %d\n", rc);
    	}
    }
    input_done = true;

    /* Join resolver threads */
    for (i = 0; i < MAX_RESOLVER_THREADS; i++){
    	rc = pthread_join(res_threads[i], NULL);

    	//If a thread join error occurs, print the error
    	if (rc){
    		fprintf(stderr, "Error: pthread_join on resolver thread returned %d\n", rc);
    	}
    }

    /* Once all threads finish, clean up on exit */
    fclose(output);

    //Destroy queue and mutexes
    queue_cleanup(&mut_queue);
    pthread_mutex_destroy(&out_mutex);
    pthread_mutex_destroy(&que_mutex);

    return EXIT_SUCCESS;
}

void* requester(void* input){
	//Set arguments on the current thread
	requester_thread_args* args = input;

	//Ensure file is set to null
	FILE* inputfp = NULL;

	//Open input file as read
	inputfp = fopen(args->file_name, "r");

	//Error if invalid file name
	if(!inputfp){
		char errorstr[SBUFSIZE];
	    sprintf(errorstr, "Error Opening Input File: %s", args->file_name);
	    perror(errorstr);
	    return NULL;
	}

	//Create buffer to read from file
	char hostname[SBUFSIZE];

	/* Read File and Process*/
	while(fscanf(inputfp, INPUTFS, hostname) > 0){
		while(1) {
			//Invoke a mutex lock on the queue to prevent other threads from accessing
			pthread_mutex_lock(args->que_mutex);

			//Test if the queue is already full, if it is then remove the
			//mutex lock and sleep for a random time.
			if(queue_is_full(args->req_queue)) {
				pthread_mutex_unlock(args->que_mutex);
				sleep();
			}
			//If not full, then process with queue write
			else {
				break;
			}
		}

		//Malloc space for the hostname
        int hostsize = sizeof(hostname);
        char* hostptr = malloc(hostsize);
        strncpy(hostptr, hostname, hostsize);

        //Add hostname to queue
        queue_push(args->req_queue, hostptr);

        //Unlock request queue mutex
        pthread_mutex_unlock(args->que_mutex);
    
    }
    
    /* Close Input File */
    fclose(inputfp);

    return NULL;
}

void* resolver(void* output){
	resolver_thread_args* args = output;

	while(1) {
		//Lock just the request queue mutex
		pthread_mutex_lock(args->que_mutex);

		//Retrieve a hostname
		char* hostnameptr = queue_pop(args->req_queue);

		//Unlock the requester queue mutex
		pthread_mutex_unlock(args->que_mutex);

		if (hostnameptr) {
			char hostname[SBUFSIZE];
			sprintf(hostname, "%s", hostnameptr);
			free(hostnameptr);

			char IP_str[INET6_ADDRSTRLEN];

			//If invalid hostname, then print a blank string
			if(dnslookup(hostname, IP_str, sizeof(IP_str))
       		== UTIL_FAILURE) {
				fprintf(stderr, "dnslookup error: %s\n", hostname);
				strncpy(IP_str, "", sizeof(IP_str));
			}

			//Write to output, again using locks
			pthread_mutex_lock(args->out_mutex);
			fprintf(args->outputfp, "%s, %s\n", hostname, IP_str);
			pthread_mutex_unlock(args->out_mutex);

		}
		else {
			if(input_done){
				return NULL;
			}
			else{
				sleep();
			}
		}
	}
	return NULL;
}

void sleep(){
	//Get random sleep time between 0 and 100 milliseconds
	long nsec = (long)(rand() % 100);
	nsec = nsec * 1000000L;

	//Create timespec struct
	struct timespec sleeptime;
	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = nsec;

	//Sleep via timepec
	nanosleep(&sleeptime, (struct timespec *)NULL);
}
