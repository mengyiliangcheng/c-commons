#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include "queue.h"

/*
int main()
{
	struct userqueue *my_queue;
	int i, handle_time, ret_time_value;
	struct node *print_ptr;

	srand((unsigned) time(NULL));	
	my_queue = init_userqueue(my_queue);
	for(i = 0; i < 10; i++){
		handle_time = rand() % 10 + 10;
		Enqueue(my_queue, i, handle_time);	
	}
	
	for(print_ptr = my_queue->front->next; print_ptr != NULL; print_ptr = print_ptr->next){
		printf("The No.%d Client will wait %d minutes.\n", print_ptr->index, print_ptr->handle_time);
	}
	printf("-----------------------------------------\n");
	
	while(is_queue_empty(my_queue) != TRUE){
		ret_time_value = get_front_node_handle_time(my_queue);
		Dequeue(my_queue);
		printf("the head of queue's handle_time is %d.\n", ret_time_value);
		printf("-----------------------------------------\n");
	}
	Dequeue(my_queue);
	Dequeue(my_queue);
	return 0;	
}
*/
