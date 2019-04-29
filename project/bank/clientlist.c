#include<stdlib.h>
#include "clientlist.h"

/*******************************************************************************
init_clientlist：初始化客户信息存储表 
*******************************************************************************/
struct client_list *init_clientlist(struct client_list *clientlist)
{
	clientlist = malloc(sizeof(struct client_list));
	if(clientlist == NULL){
		printf("Error, malloc is failed.\n");
		exit(EXIT_FAILURE);
	}
	clientlist->client_data = malloc(sizeof(struct cnode));
	if(clientlist->client_data == NULL){
		printf("Error, malloc is failed.\n");
		exit(EXIT_FAILURE);
	}
	clientlist->client_data->index = -1;
	clientlist->client_data->enter_time = -1;
	clientlist->client_data->wait_time = -1;
	clientlist->client_data->next = NULL;
	
	clientlist->client_counts = 0;
	clientlist->rear = clientlist->client_data; 
	
	return clientlist;
} 

/*******************************************************************************
add_client_data：向客户信息表中添加用户信息 
*******************************************************************************/

struct client_list *add_client_data(struct client_list *clientlist, int client_index, int client_enter_time, int client_wait_time)
{
	struct cnode *new_cnode;
	
	new_cnode = malloc(sizeof(struct cnode));
	if(new_cnode == NULL){
		printf("Error, malloc is failed.\n");
		exit(EXIT_FAILURE);
	}
	new_cnode->index = client_index;
	new_cnode->enter_time = client_enter_time;
	new_cnode->wait_time = client_wait_time;
	new_cnode->next = NULL;
	
	clientlist->rear->next = new_cnode;
	clientlist->rear = new_cnode;
	(clientlist->client_counts)++;
	
	return clientlist;
}
