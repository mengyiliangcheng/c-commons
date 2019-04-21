#ifndef BANKIMITATE_H
#define BANKIMITATE_H
#define MAX_TIME 540 			/*�ٶ�Ϊ9Сʱ������*/

int curr_time;					/*��ǰʱ����*/
int time_between_clients; 		/*�����ͻ������¼���ʱ����*/
int client_handle_time; 		/*�ͻ�����ҵ�������ʱ��*/
int client_num;					/*�ͻ����*/
int client_num_in_bank;			/*ĳ��ʱ�������������ŶӵĿͻ���(�Ķ��е��ܺ�)*/
 
struct client_list *clientlist;		/*ָ��洢�ͻ�������Ϣ�������ָ��*/
struct userqueue *bank_queue[4];	/*ָ���ĸ����нṹ��ָ��*/

void init_for_bank(void);  
void create_clienter(void);
void calc_client_in_bank(void);
void handle_queue_when_new_client_arrive(struct userqueue *user_queue);
int get_min_count_queue(void);
int get_all_wait_time_before_handle(struct userqueue *user_queue);
void imitate_bankflow(void);
void calc_avg_handle_time(void);
#endif
