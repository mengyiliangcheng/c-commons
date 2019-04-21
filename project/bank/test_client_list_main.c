#include<time.h>
#include<stdlib.h>
#include<stdio.h>
#include "clientlist.h"

/*
int main()
{
	int i,client_code, client_wait_time, dur_time, client_enter_time;
	struct client_list *clientlist;
	struct cnode *print_ptr;
	
	srand((unsigned) time(NULL));	
	client_enter_time = 0;
	client_code = 1;
	clientlist = init_clientlist(clientlist);
	
	for(i = 0; i < 10; i++){
		dur_time = rand() % 10 + 3;
		client_enter_time += dur_time;
		client_wait_time = rand() % 10 + 3; 
		
		add_client_data(clientlist, client_code++, client_enter_time, client_wait_time);
	}
	
	for(print_ptr = clientlist->client_data->next; print_ptr != NULL; print_ptr = print_ptr->next){
		printf("The No.%d clienter enter time is %d, wait time is %d.\n", print_ptr->index, print_ptr->enter_time, print_ptr->wait_time);
	}
	
	return 0;
	
}
*/
