#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#define MAX_RESOLVE_TH 10

extern bool input_done;

//Struct to store arguements for the resquester threads
typedef struct {
	//Pointer to queue
    queue* req_queue;

    //Pointer to file name
    char* file_name;

    //Pointer to queue mutex
    pthread_mutex_t* que_mutex;
} requester_thread_args ;

//Struct to store arguements for the resolver threads
typedef struct {
	//Pointer to queue
    queue* req_queue;

    //Pointer to output file
    FILE* outputfp;

    //Pointer to queue mutex
    pthread_mutex_t* que_mutex;
    
    //Pointer to output file mutex
    pthread_mutex_t* out_mutex;
} resolver_thread_args ;

/* Function prototype for requester function. Accepts void
pointer, used to pass in struct with arguments */
void* requester(void*);

/* Function prototype for resolver function. Accepts void
pointer, used to pass in struct with arguments */
void* resolver(void*);

void sleep();

#endif