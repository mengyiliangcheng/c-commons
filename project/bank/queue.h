#ifndef QUEUE_H
#define QUEUE_H
#define TRUE 1
#define FALSE 0

typedef int Bool;

struct node{
	int index;				/*客户编号*/
	int handle_time;		/*该客户处理业务的时间*/
	struct node *next; 
};

struct userqueue{
	struct node *front;		/*队列头指针*/
	struct node *rear;		/*队列尾指针*/
	int queue_counts;		/*队列中存储元素的个数*/ 
};

struct userqueue *init_userqueue(struct userqueue *user_queue);
Bool is_queue_empty(struct userqueue *user_queue);
struct userqueue *Enqueue(struct userqueue *user_queue, int client_index, int client_handle_time);
void Dequeue(struct userqueue *user_queue);
int get_front_node_handle_time(struct userqueue *user_queue);
#endif
