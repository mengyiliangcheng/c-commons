#ifndef BANKIMITATE_H
#define BANKIMITATE_H
#define MAX_TIME 540 			/*假定为9小时工作制*/

int curr_time;					/*当前时间结点*/
int time_between_clients; 		/*两个客户到达事件的时间间隔*/
int client_handle_time; 		/*客户处理业务所需的时间*/
int client_num;					/*客户编号*/
int client_num_in_bank;			/*某个时间结点上银行中排队的客户数(四队列的总和)*/
 
struct client_list *clientlist;		/*指向存储客户处理信息的链表的指针*/
struct userqueue *bank_queue[4];	/*指向四个队列结构的指针*/

void init_for_bank(void);  
void create_clienter(void);
void calc_client_in_bank(void);
void handle_queue_when_new_client_arrive(struct userqueue *user_queue);
int get_min_count_queue(void);
int get_all_wait_time_before_handle(struct userqueue *user_queue);
void imitate_bankflow(void);
void calc_avg_handle_time(void);
#endif
