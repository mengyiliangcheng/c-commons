#include<stdlib.h>
#include<time.h>
#include "clientlist.h"
#include "queue.h"
#include "bankimitate.h"

/*******************************************************************************
init_for_bank����ʼ��״̬  ��ʾ���п���Ӫҵ 
*******************************************************************************/
void init_for_bank(void)
{
	int i;
	
	srand((unsigned) time(NULL));
	
	curr_time = 0;											/*��ʼ����ǰʱ����*/
	client_num = 1;											/*��ʼ���ͻ����*/
	client_num_in_bank = 0;									/*��ʼ�������Ŷӿͻ���*/
	clientlist = init_clientlist(clientlist); 				 
	for(i = 0; i < 4; i++){
		bank_queue[i] = init_userqueue(bank_queue[i]);
	}
} 

/*******************************************************************************
create_clienter������һ��"�ͻ����������¼�"(��������ͻ���ʱ�����봦��ҵ��ʱ��)
				 
	ע���������һ����Ҫ��������curr_time>540ʱ����ʾ��ʱ�����Ѿ�Ӫҵ��9Сʱ
								����޷������µ�"�ͻ������¼�" 
								
	���������������Ǽٶ�������"�ͻ������¼�"��ʱ��������5-7����֮��
						  ���ͻ�����ҵ���ʱ������ֲ���20-25����֮��  
*******************************************************************************/
void create_clienter(void)
{	/*
	srand((unsigned) time(NULL));
 	*/
 	
 	if(curr_time <= 540){
		/*"�ͻ���������"ʱ��Ĳ���*/
	 	time_between_clients = rand() % 3 + 5;		 
	 	client_handle_time = rand() % 6 + 20;
		
		/*�ù̶�ʱ����Ϊ������֤ ����̶�Ϊ10  ����ʱ��50*/
		/*
		time_between_clients = 10;		 
	 	client_handle_time = 50;
		*/
		
		curr_time += time_between_clients;		/*�ͻ���������ʱ��ʱ����*/ 	
	}
}

/*******************************************************************************
calc_client_in_bank��ͳ��ĳ��ʱ����ʱ���������пͻ������� 
*******************************************************************************/
void calc_client_in_bank(void)
{
	int i;
	
	client_num_in_bank = 0;			/*����Ϊ0*/
	for(i = 0; i < 4; i++){
		client_num_in_bank += bank_queue[i]->queue_counts;
	}
}

/*******************************************************************************
handle_queue_when_new_client_arrive����һ���ͻ����������¼�����ʱ���з��洦��
									 �ض����еĹ���
				   					 ʵ�ʲ�����ָ��ĳ�����нṹ��ָ�� 	
*******************************************************************************/
void handle_queue_when_new_client_arrive(struct userqueue *user_queue)
{
	/*
		��ȡ��ǰʱ����(ʵ����Ϊ���¼���ʱ����)�����������ʱ���֮�ڸö���
		�Ŷӿͻ����������(time_between_clients��ȥ����Ԫ�ص�handle_time,Ϊ0ʱ��
		Ԫ���뿪���У���������ֱ��time_between_clientsΪ0 ���߸ö���Ϊ��) 
	*/
	int remain_between_time = time_between_clients;		/*ʱ���ʣ����*/
	int remain_handle_time;								/*���׿ͻ����ҵ����Ҫ��ʱ�䳤��*/
	int compare_time;									/*�洢����������������ı���*/
	
	/*����һ���ͻ������¼�������ʱ���֮ǰ����������һֱ����*/
	while(remain_between_time != 0){
		if(user_queue->queue_counts == 0){
			break;
		} 
		remain_handle_time = get_front_node_handle_time(user_queue);
		compare_time = remain_handle_time - remain_between_time;
		if(compare_time	<= 0){		/*��ʱ��ʾ���׿ͻ����ҵ���뿪����*/
			Dequeue(user_queue);
			remain_between_time -= remain_handle_time; 
		}else{
			user_queue->front->next->handle_time = (remain_handle_time - remain_between_time);	
			break;
		}
	}
}

/*******************************************************************************
get_min_count_queue����ȡ��ǰʱ��ڵ����������ٵĶ��е����
					 ����ֵ���������ٵĶ��е����(0/1/2/3)
					 
	ע��Ϊ�˷��㣬�򵥵������Ƚϣ����������Ƚ�֮��ѡ����Сֵ 
		�����һ��ѭ��ȷ��ֵΪ��Сֵ�Ķ������ 
*******************************************************************************/
int get_min_count_queue(void)
{
	int i;
	int queue_count[4];
	int comp_value_1, comp_value_2, min_value, ret_index;
	
	for(i = 0; i < 4; i++){
		queue_count[i] = bank_queue[i]->queue_counts;
	}
	
	if(queue_count[0] <= queue_count[1]){
		comp_value_1 = queue_count[0];
	}else{
		comp_value_1 = queue_count[1];
	}
 	if(queue_count[2] <= queue_count[3]){
		comp_value_2 = queue_count[2];
	}else{
		comp_value_2 = queue_count[3];
	}
	if(comp_value_1	<= comp_value_2){
		min_value = comp_value_1;
	}else{
		min_value = comp_value_2;
	}
	
	for(i = 0; i < 4; i++){
		if(queue_count[i] == min_value){
			ret_index = i;
			break;
		}
	}
	
	return ret_index;
}

/*******************************************************************************
get_all_wait_time_before_handle��ͳ��ĳ�ͻ����ֵ�������ҵ��֮ǰ��Ҫ�ȴ���ʱ�䳤��
								 ʵ�ʲ�����ָ���ض����нṹ��ָ��
								 ����ֵ��"�ȴ�"��ʱ���ܳ��� 
*******************************************************************************/
int get_all_wait_time_before_handle(struct userqueue *user_queue)
{
	struct node *ptr;
	int wait_time_before_handle = 0;
	
	for(ptr = user_queue->front->next; ptr != NULL; ptr = ptr->next){
		wait_time_before_handle += ptr->handle_time;
	} 
	
	return wait_time_before_handle;
} 

/*******************************************************************************
imitate_bankflow��ģ������һ��Ĺ������� 
*******************************************************************************/
void imitate_bankflow(void)
{
	int queue_index;				/*�����ж�������*/
	int select_queue_index;			/*ĳʱ����ʱ�����������е����*/
	int wait_time_before_handle;	/*�ͻ��ڴ���ҵ��֮ǰ��Ҫ�ȴ���ʱ�䳤��*/
	
	init_for_bank();
	
	while(client_num_in_bank > 0 || curr_time <= 540){
		create_clienter();
		for(queue_index = 0; queue_index < 4; queue_index++){	/*�ֱ����������*/
			handle_queue_when_new_client_arrive(bank_queue[queue_index]); 
		}
		/*
			ȷ��ĳ�ͻ��Ķ��У����ÿͻ������ض����С�ͳ�Ƹÿͻ�֮ǰ���пͻ�
			�账�����ҵ���ȫ��ʱ�䣬���ϸÿͻ���client_handle_time����Ϊ��
			����ȴ���ʱ���ܳ�������Щ��Ϣ���ܵ��ͻ�������Ϣ������ 
			
			��һ��ǰ����������Ӫҵʱ��֮�������¼����� 
		*/
		if(curr_time <=540){
			select_queue_index = get_min_count_queue();	
			
			wait_time_before_handle = get_all_wait_time_before_handle(bank_queue[select_queue_index]);
			Enqueue(bank_queue[select_queue_index], client_num, client_handle_time);
			
			add_client_data(clientlist, client_num, curr_time, wait_time_before_handle + client_handle_time); 
			
			printf("The No.%3d client's begin_time is %3d, handle_time is %2d, waiting time is %2d.\n", clientlist->rear->index, 
												clientlist->rear->enter_time, client_handle_time, clientlist->rear->wait_time);
												
			client_num++;			/*�ͻ��ı��*/
		} 
		calc_client_in_bank();		/*�����οͻ����������¼��������ڿͻ�������*/
	}
	calc_avg_handle_time();
} 

/*******************************************************************************
calc_avg_handle_time������ƽ���Ŀͻ�����ʱ�䳤�� 
*******************************************************************************/
void calc_avg_handle_time(void)
{
	int all_handle_time;
	double avg_handle_time;
	struct cnode *ptr;
	
	for(ptr = clientlist->client_data->next; ptr != NULL; ptr = ptr->next){
		all_handle_time += ptr->wait_time;
	}	
	avg_handle_time = all_handle_time / (clientlist->client_counts);
	
	putchar('\n');
	printf("-----------------------------------------------------------------------------\n");
	printf("The average client's handle_time is %.2f.\n", avg_handle_time);
	printf("-----------------------------------------------------------------------------\n");
}

