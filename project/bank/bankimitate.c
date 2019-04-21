#include<stdlib.h>
#include<time.h>
#include "clientlist.h"
#include "queue.h"
#include "bankimitate.h"

/*******************************************************************************
init_for_bank：初始化状态  表示银行开门营业 
*******************************************************************************/
void init_for_bank(void)
{
	int i;
	
	srand((unsigned) time(NULL));
	
	curr_time = 0;											/*初始化当前时间结点*/
	client_num = 1;											/*初始化客户编号*/
	client_num_in_bank = 0;									/*初始化银行排队客户数*/
	clientlist = init_clientlist(clientlist); 				 
	for(i = 0; i < 4; i++){
		bank_queue[i] = init_userqueue(bank_queue[i]);
	}
} 

/*******************************************************************************
create_clienter：创建一个"客户到达银行事件"(随机创建客户的时间间隔与处理业务时间)
				 
	注：在这里，有一点需要声明：当curr_time>540时，表示此时银行已经营业了9小时
								因此无法接收新的"客户到达事件" 
								
	声明：在这里我们假定：两个"客户到达事件"的时间间隔处于5-7分钟之间
						  而客户处理业务的时间随机分布在20-25分钟之间  
*******************************************************************************/
void create_clienter(void)
{	/*
	srand((unsigned) time(NULL));
 	*/
 	
 	if(curr_time <= 540){
		/*"客户到达银行"时间的参数*/
	 	time_between_clients = rand() % 3 + 5;		 
	 	client_handle_time = rand() % 6 + 20;
		
		/*用固定时间作为测试验证 间隔固定为10  处理时间50*/
		/*
		time_between_clients = 10;		 
	 	client_handle_time = 50;
		*/
		
		curr_time += time_between_clients;		/*客户到达银行时的时间结点*/ 	
	}
}

/*******************************************************************************
calc_client_in_bank：统计某个时间结点时银行内所有客户的数量 
*******************************************************************************/
void calc_client_in_bank(void)
{
	int i;
	
	client_num_in_bank = 0;			/*重置为0*/
	for(i = 0; i < 4; i++){
		client_num_in_bank += bank_queue[i]->queue_counts;
	}
}

/*******************************************************************************
handle_queue_when_new_client_arrive：当一个客户到达银行事件发生时银行方面处理
									 特定队列的过程
				   					 实际参数：指向某个队列结构的指针 	
*******************************************************************************/
void handle_queue_when_new_client_arrive(struct userqueue *user_queue)
{
	/*
		获取当前时间结点(实际上为两事件的时间间隔)，计算在这个时间段之内该队列
		排队客户的流动情况(time_between_clients减去队首元素的handle_time,为0时首
		元素离开队列，继续处理。直到time_between_clients为0 或者该队列为空) 
	*/
	int remain_between_time = time_between_clients;		/*时间段剩余量*/
	int remain_handle_time;								/*队首客户完成业务还需要的时间长度*/
	int compare_time;									/*存储上述两个变量相减的变量*/
	
	/*在下一个客户到达事件发生的时间点之前，处理流程一直进行*/
	while(remain_between_time != 0){
		if(user_queue->queue_counts == 0){
			break;
		} 
		remain_handle_time = get_front_node_handle_time(user_queue);
		compare_time = remain_handle_time - remain_between_time;
		if(compare_time	<= 0){		/*此时表示队首客户完成业务，离开银行*/
			Dequeue(user_queue);
			remain_between_time -= remain_handle_time; 
		}else{
			user_queue->front->next->handle_time = (remain_handle_time - remain_between_time);	
			break;
		}
	}
}

/*******************************************************************************
get_min_count_queue：获取当前时间节点下人数最少的队列的序号
					 返回值：人数最少的队列的序号(0/1/2/3)
					 
	注：为了方便，简单地两两比较，再在两个比较之中选择最小值 
		最后用一个循环确定值为最小值的队列序号 
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
get_all_wait_time_before_handle：统计某客户在轮到他处理业务之前需要等待的时间长度
								 实际参数：指向特定队列结构的指针
								 返回值："等待"的时间总长度 
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
imitate_bankflow：模拟银行一天的工作流程 
*******************************************************************************/
void imitate_bankflow(void)
{
	int queue_index;				/*银行中队伍的序号*/
	int select_queue_index;			/*某时间结点时最少人数队列的序号*/
	int wait_time_before_handle;	/*客户在处理业务之前需要等待的时间长度*/
	
	init_for_bank();
	
	while(client_num_in_bank > 0 || curr_time <= 540){
		create_clienter();
		for(queue_index = 0; queue_index < 4; queue_index++){	/*分别处理各个队伍*/
			handle_queue_when_new_client_arrive(bank_queue[queue_index]); 
		}
		/*
			确定某客户的队列，将该客户插入特定队列。统计该客户之前所有客户
			需处理各自业务的全部时间，加上该客户的client_handle_time，即为他
			所需等待的时间总长，将这些信息汇总到客户处理信息链表中 
			
			有一个前提条件：在营业时间之内有新事件发生 
		*/
		if(curr_time <=540){
			select_queue_index = get_min_count_queue();	
			
			wait_time_before_handle = get_all_wait_time_before_handle(bank_queue[select_queue_index]);
			Enqueue(bank_queue[select_queue_index], client_num, client_handle_time);
			
			add_client_data(clientlist, client_num, curr_time, wait_time_before_handle + client_handle_time); 
			
			printf("The No.%3d client's begin_time is %3d, handle_time is %2d, waiting time is %2d.\n", clientlist->rear->index, 
												clientlist->rear->enter_time, client_handle_time, clientlist->rear->wait_time);
												
			client_num++;			/*客户的编号*/
		} 
		calc_client_in_bank();		/*处理本次客户到达银行事件后银行内客户的数量*/
	}
	calc_avg_handle_time();
} 

/*******************************************************************************
calc_avg_handle_time：计算平均的客户处理时间长度 
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

