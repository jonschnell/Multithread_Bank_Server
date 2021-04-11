//Jon Schnell
//4/6/21

#include "Bank.h"
#include "Queue.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <stdbool.h> 
#include <pthread.h>

void parse(char input[], char *parsed[]);
void *process(void *fp);
pthread_mutex_t accmutex[10000];

int main(int argc, char *argv[]){

	if(argc != 4){
		printf("Error: too few or too many arguments\n");
		printf("useage: ./appserver <# of worker threads> <# of accounts> <output file>\n");
		exit(-1);
	}
	
	//----------------------- database ------------------------
	int numWorkers = atoi(argv[1]);
	int numAccounts = atoi(argv[2]);
	char outfile[32];
	strcpy(outfile, argv[3]);
	
	FILE *fp = fopen(outfile, "w");
	
	//printf("%d\n", numWorkers);
	//printf("%d\n", numAccounts);
	//printf("%s\n", outfile);
	
	initialize_accounts(numAccounts);
	
	init_queue();
	
	//---------------------- create account mutexes -------------------
	//pthread_mutex_t accmutex[numAccounts];
	for(int i = 0; i < numAccounts; i++){
		pthread_mutex_init(&accmutex[i], NULL);
	}
	
	//---------------------- create threads -------------------
	pthread_t threads[numWorkers];
	int thread;
	
	for(int i = 0; i < numWorkers; i++){
		thread = pthread_create(&threads[i], NULL, process, (void *)fp);
		pthread_mutex_init(&accmutex[i], NULL);
	}
	
	//TODO create account mutexes
	
	//----------------------- user input loop ---------------------------
	int previd = 1;
	while(1){
		// Get user input
		char input[512];
		int length = strlen(fgets(input,512,stdin));
		//null terminate input string
		input[length-1] = '\0';
		//parse input
		char *parsed[21] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
		//parse input
		parse(input, parsed);
		
		//----------- Balance checks ----------
		if(strcmp(parsed[0],"CHECK") == 0){
		
		//construct a CHECK node
		Node * temp;
		//malloc
		temp = (Node*) malloc(sizeof(Node));
		temp->req_id = previd + 1;
		//mark start time
		gettimeofday(&temp->arrival, NULL);
		temp->req_type = 0;//0 for check, 1 for trans
		temp->check_id = atoi(parsed[1]);
		
		//enqueue
		enqueue(temp);
		
		//printf("ID %d\n", read_account(atoi(parsed[1])));
		printf("ID %d\n", previd);
		previd = previd + 1;

		}
		//----------- Transactions ------------
		if(strcmp(parsed[0],"TRANS") == 0){
			
		//construct a CHECK node
		Node * temp;
		//malloc
		temp = (Node*) malloc(sizeof(Node));
		temp->req_id = previd + 1;
		//mark start time
		gettimeofday(&temp->arrival, NULL);
		temp->req_type = 1;//0 for check, 1 for trans

		
		int i = 1;
		int j = 0;
		//construct array of transactions
		while(parsed[i] != NULL){
			Trans * temptrans;
			temptrans = (Trans*) malloc(sizeof(Trans));
			temptrans->acc_id = atoi(parsed[i]);
			temptrans->amount = atoi(parsed[i+1]);
			temp->trans[j] = temptrans;	
			
			//increment
			i = i + 2;
			j++;
		}
		
		//set num trans
		temp->num_trans = (i-1)/2;
				
		//enqueue
		enqueue(temp);
		
		//print transaction ID the increment it
		printf("ID %d\n", previd);
		previd = previd + 1;
			
		
		}
		//----------- Exit --------------------
		if(strcmp(parsed[0],"EXIT") == 0){
			printf("Exiting...");
			sleep(1);
			fclose(fp);
			free_accounts();
			mark_queue_as_done();
			return 0;
		}

	}
}

//parse input string into array of arguments
void parse(char input[], char *parsed[]){
	int count = 0;
	char flag[] = " ";
	
	char *ptr = strtok(input, flag);
	
	while(ptr != NULL){
		parsed[count] = ptr;
		//printf("%s\n", parsed[count]);
		ptr = strtok(NULL, flag);
		count++;
	}
}

//code to be exicuted by the workers
void *process(void *fp){

	//deque
	Node * temp2;
	temp2 = (Node*) malloc(sizeof(temp2));
	temp2 = dequeue();
	
	//FILE *fp;
	struct timeval time;
	
	//request is of type ------------ CHECK --------------
	if (temp2->req_type == 0){
		gettimeofday(&time, NULL);
		fprintf(fp, "%d BAL %d TIME %ld.%06ld %ld.%06ld\n", temp2->req_id-1, read_account(temp2->check_id), temp2->arrival.tv_sec, temp2->arrival.tv_usec, time.tv_sec, time.tv_usec);
	}
	
	//request is of type ------------- TRANS --------------
	if (temp2->req_type == 1){
		
		//write to the accounts
		int i = 0;
		Trans * temptrans;
		temptrans = (Trans*) malloc(sizeof(Trans));
		
		//check for ISF
		bool isf = false;
		int isfacc;
		while (i < temp2->num_trans){
			temptrans = temp2->trans[i];
			if (temptrans->amount + read_account(temptrans->acc_id) < 0){
				isf = true;
				isfacc = temptrans->acc_id;
			}
			i++;
		}
		if (isf == true){
			gettimeofday(&time, NULL);
			fprintf(fp, "%d ISF %d TIME %ld.%06ld %ld.%06ld\n", temp2->req_id-1, isfacc, temp2->arrival.tv_sec, temp2->arrival.tv_usec, time.tv_sec, time.tv_usec);
		}
		else{
			//write the transactions to each acocunt
			i = 0;
			while (i < temp2->num_trans){
				temptrans = temp2->trans[i];
				pthread_mutex_lock(&accmutex[temptrans->acc_id]);
				write_account(temptrans->acc_id, temptrans->amount + read_account(temptrans->acc_id));
				pthread_mutex_unlock(&accmutex[temptrans->acc_id]);
				i++;
			}
			gettimeofday(&time, NULL);
			fprintf(fp, "%d OK TIME %ld.%06ld %ld.%06ld\n", temp2->req_id-1, temp2->arrival.tv_sec, temp2->arrival.tv_usec, time.tv_sec, time.tv_usec);
		}
	
	
	}

	
	//------------- TESTING ----------------
	/*Node * temp2;
	temp2 = (Node*) malloc(sizeof(temp2));
	temp2 = dequeue();
	printf("check id: %d\n", temp2->check_id);
	printf("numtrans: %d\n", temp2->num_trans);
	
	int i = 0;
	Trans * temptrans;
	temptrans = (Trans*) malloc(sizeof(Trans));

	while (i < temp2->num_trans){
		temptrans = temp2->trans[i];
		printf("trans ID: %d\n", temptrans->acc_id);
		printf("trans A: %d\n", temptrans->amount);
		
		i++;
		//temptrans = temptrans
	}*/

}
