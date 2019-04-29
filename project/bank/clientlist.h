#ifndef CLIENTLIST_H
#define CLIENTLIST_H

struct cnode{
	int index;			/*客户编号*/
	int enter_time;		/*客户到达银行事件发生时的时间点*/
	int wait_time;		/*客户完成业务处理所等待的全部时间*/ 
	struct cnode *next; 
};

struct client_list{
	struct cnode *client_data;		/*指向存储用户信息的链表头结点的指针*/
	struct cnode *rear;				/*指向链表中最后一个客户结点的指针*/
	int client_counts;				/*客户存储表中存储的客户总数*/ 
};

struct client_list *init_clientlist(struct client_list *clientlist);
struct client_list *add_client_data(struct client_list *clientlist, int client_index, int client_enter_time, int client_wait_time); 
#endif
