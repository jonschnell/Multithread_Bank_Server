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
void *process();


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
	
	//printf("%d\n", numWorkers);
	//printf("%d\n", numAccounts);
	//printf("%s\n", outfile);
	
	initialize_accounts(numAccounts);
	
	init_queue();
	
	//---------------------- create threads -------------------
	pthread_t threads[numWorkers];
	int thread;
	
	for(int i = 0; i < numWorkers; i++){
		thread = pthread_create(&threads[i], NULL, process, NULL);
	}
	
	//----------------------- user input loop ---------------------------
	while(1){
		int previd;
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
		//TODO
		//malloc
		//enqueue each transaction
		//mark start time
		//printf("ID %d\n", read_account(atoi(parsed[1])));
		

		}
		//----------- Transactions ------------
		if(strcmp(parsed[0],"TRANS") == 0){
		//TODO
		//malloc
		//enqueue each transaction
		//write_account(atoi(parsed[1]), atoi(parsed[2]));
			//struct request request1;
			//struct transfer transfer1;
			
			int i = 1;
			while(parsed[i] != NULL){
				//transfer1.acc_id = atoi(parsed[i]);
				//transfer1.amount = atoi(parsed[i+1]);
				i = i + 2;
				//request1.transfers[(i-1)/2] = transfer1;
			}
			
		
		}
		//----------- Exit --------------------
		if(strcmp(parsed[0],"EXIT") == 0){
			printf("Exiting...");
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
void *process(){
	printf("test\n");
	//deque
	//acquire mutex
	//write to the account
	//mark finish time
	//write to file
	//release mutex
//loop
}
