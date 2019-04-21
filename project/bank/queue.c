#include<stdlib.h>
#include "queue.h"

/*******************************************************************************
init_userqueue����ʼ�����нṹ
   				ʵ�ʲ�����ָ����нṹ��ָ��
				����ֵ��ָ���ʼ����Ķ��нṹ��ָ�� 
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
is_queue_empty���ж϶����Ƿ�Ϊ��
				ʵ�ʲ�����ָ����нṹ��ָ��
				����ֵ��Ϊ�� TRUE
						��Ϊ�� FALSE
						���в�����(δ��ʼ��) ��ʾ������Ϣ 
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
Enqueue��������βԪ�ز���
		 ʵ�ʲ�����ָ����е�ָ�� �ͻ��ı�� �ͻ����������ʱ��
		 ����ֵ������������ָ����е�ָ�� 
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
Dequeue��ɾ������Ԫ��
		 ʵ�ʲ�����ָ����нṹ��ָ�� 
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
get_front_node_handle_time����ȡ����Ԫ�ؽ��Ĵ���������¼�ֵ
							ʵ�ʲ�����ָ����нṹ��ָ��
							����ֵ���ö��ж���Ԫ�ص�handle_time��ֵ (������Ϊ��ʱ����-1)
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
