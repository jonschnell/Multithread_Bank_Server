#include "Queue.h"

//outline provided by Aiman Priester
Queue q;

pthread_mutex_t mutex;
pthread_cond_t cond;


/* Initialize the fields of a Queue */
void init_queue()
{
	/* reset the fields of q */
	q.head = q.tail = NULL;
	q.count = q.done = 0;
	
	/* initialize the mutex and condition variable for queue access */
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
}


/*
Set the queue's 'done' flag to 1. This function is called
when the user typed the "END" command.
By setting the 'done' flag, the dequeue() function will
no longer block the calling thread and immediately returns
NULL when the queue is emptied. 
*/
void mark_queue_as_done()
{
	/* protect the operation with mutexes */
	pthread_mutex_lock(&mutex);
	
	q.done = 1;
	/* broadcast cond to wake up the threads that are waiting */
	pthread_cond_broadcast(&cond);
	
	pthread_mutex_unlock(&mutex);
}



/* Attach a new Node pointer to the tail end of the queue. */
void enqueue(Node *node)
{
	/* protect the operation with mutexes */
	pthread_mutex_lock(&mutex);
	
	/* attach the new node to the tail end of the queue*/
	if (q.count == 0)
		q.head = q.tail = node;
	else
	{
		q.tail->next = node;
		q.tail = node;
	}
	q.count++;
	/* broadcast cond to wake up the threads that are waiting */
	pthread_cond_broadcast(&cond);
	
	pthread_mutex_unlock(&mutex);
}



/*
Remove the head node (if exits) from the queue and return it.
If the queue is empty, the function will:
(1) wait until the queue is non-empty if done==0,
(2) return NULL if done==1

Return: A Node pointer for a pending user request, or NULL
if the queue is empty and the 'done' flag is 1.
*/
Node* dequeue()
{
	Node *result;
	
	/* protect the operation with mutexes */
	pthread_mutex_lock(&mutex);
	
	/* wait for cond if the queue is empty and program is not done */
	while (q.done==0 && q.count==0)
		pthread_cond_wait(&cond, &mutex);
	/* If we reach this point, there are only 2 possibilities:
		 (1) queue is not empty
		 (2) queue is empty, and program is waiting to exit
	*/
	if (q.count > 0)
	{
		result = q.head;
		q.head = q.head->next;
		q.count--;
	}
	else
	{
		result = NULL;
	}
	
	pthread_mutex_unlock(&mutex);
	
	return result;
}

int get_queue_len() {
	return q.count;
}
