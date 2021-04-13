#include <pthread.h>
#include <sys/time.h>

//outline provided by Aiman Priester
//modified by jon schnell
/* A transaction pair: <acc> <amount>*/
typedef struct {
	int acc_id;  /* the account ID */
	int amount;  /* the amount to be added. Can be positive or negative */
	//struct Trans *next;
} Trans;


/* A user request (CHECK or TRANS) */
typedef struct Node{
	int req_id;	/* unique ID for this request */
	struct timeval arrival; /* arrival time of request */
	struct timeval finish;  /* finish time of request */
	struct Node *next; 	/* link to the next node */
	
	/* Request type: 0 for CHECK, 1 for TRANS */
	int req_type;
	
	/*** For CHECK request ***/
	int check_id;
	/*************************/
	
	/*** For TRANS request ***/
	int num_trans;
	Trans *trans[32];
	/*************************/
} Node;
	

/* Request Queue */
typedef struct {
	Node *head; /* Take requests from the head end */
	Node *tail; /* Add requests at the tail end */
	
	int count; /* Number of requests in queue */
	
	int done; /* Flag to indicate whether program is waiting to exit */
} Queue;


/* Initialize the Queue */
void init_queue();

/*
Set the queue's 'done' flag to 1. This function is called
when the user typed the "END" command.
By setting the 'done' flag, the dequeue() function will
no longer block the calling thread and immediately returns
NULL when the queue is emptied. 
*/
void mark_queue_as_done();

/* Attach a new Node pointer to the tail end of the queue */
void enqueue(Node *node);

/*
Remove the head node (if exits) from the queue and return it.
If the queue is empty, the function will:
(1) wait until the queue is non-empty if done==0,
(2) return NULL if done==1

Return: A Node pointer for a pending user request, or NULL
if the queue is empty and the 'done' flag is 1.
*/
Node* dequeue();

int get_queue_len();
