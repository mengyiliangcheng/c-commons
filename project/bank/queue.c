#include<stdlib.h>
#include "queue.h"

/*******************************************************************************
init_userqueue：初始化队列结构
   				实际参数：指向队列结构的指针
				返回值：指向初始化后的队列结构的指针 
*******************************************************************************/
struct userqueue *init_userqueue(struct userqueue *user_queue)
{
	user_queue = malloc(sizeof(struct userqueue));
	if(user_queue == NULL){
		printf("Error, malloc is failed.\n");
		exit(EXIT_FAILURE);
	}
	user_queue->front = malloc(sizeof(struct node));
	if(user_queue->front == NULL){
		printf("Error, malloc is failed.\n");
		exit(EXIT_FAILURE);
	}
	user_queue->front->index = -1;
	user_queue->front->handle_time = -1;
	user_queue->front->next = NULL;
	
	user_queue->rear = user_queue->front;
	user_queue->queue_counts = 0;
	
	return user_queue;
}


/*******************************************************************************
is_queue_empty：判断队列是否为空
				实际参数：指向队列结构的指针
				返回值：为空 TRUE
						不为空 FALSE
						队列不存在(未初始化) 提示错误信息 
*******************************************************************************/
Bool is_queue_empty(struct userqueue *user_queue)
{
	if(user_queue == NULL){
		printf("The struct of userqueue is not initialize.\n");
		exit(EXIT_FAILURE);
	}	
	if(user_queue->queue_counts	== 0){
		return TRUE;
	}else{
		return FALSE;
	}
} 

/*******************************************************************************
Enqueue：新增队尾元素操作
		 实际参数：指向队列的指针 客户的编号 客户处理事务的时间
		 返回值：新增操作后指向队列的指针 
*******************************************************************************/
struct userqueue *Enqueue(struct userqueue *user_queue, int client_index, int client_handle_time)
{
	struct node *ins_node;
	
	ins_node = malloc(sizeof(struct node));
	if(ins_node	== NULL){
		printf("Error, malloc is failed.\n");
		exit(EXIT_FAILURE);
	}	
	ins_node->index = client_index;
	ins_node->handle_time = client_handle_time;
	ins_node->next = NULL;
	
	user_queue->rear->next = ins_node;
	user_queue->rear = ins_node;
	(user_queue->queue_counts)++;
	
	return user_queue;
}

/*******************************************************************************
Dequeue：删除队首元素
		 实际参数：指向队列结构的指针 
*******************************************************************************/
void Dequeue(struct userqueue *user_queue)
{
	struct node *del_ptr;
	
	if(user_queue == NULL){
		printf("The struct of userqueue is not initialize.\n");
		exit(EXIT_FAILURE);
	}
	if(user_queue->queue_counts	== 0){
		printf("The queue is empty.\n");
	}else{
		del_ptr = user_queue->front->next;
		user_queue->front->next = del_ptr->next;
		if(user_queue->queue_counts	== 1){
			user_queue->rear = user_queue->front;
		}
		free(del_ptr);
		(user_queue->queue_counts)--;
	}
}


/*******************************************************************************
get_front_node_handle_time：获取队首元素结点的处理事务的事件值
							实际参数：指向队列结构的指针
							返回值：该队列队首元素的handle_time的值 (若队列为空时返回-1)
*******************************************************************************/
int get_front_node_handle_time(struct userqueue *user_queue)
{
	if(user_queue->queue_counts	== 0){
		printf("The queue is empty.\n");	
		return -1;
	}else{
		return user_queue->front->next->handle_time;	
	}
}
