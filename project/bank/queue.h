#ifndef QUEUE_H
#define QUEUE_H
#define TRUE 1
#define FALSE 0

typedef int Bool;

struct node{
	int index;				/*�ͻ����*/
	int handle_time;		/*�ÿͻ�����ҵ���ʱ��*/
	struct node *next; 
};

struct userqueue{
	struct node *front;		/*����ͷָ��*/
	struct node *rear;		/*����βָ��*/
	int queue_counts;		/*�����д洢Ԫ�صĸ���*/ 
};

struct userqueue *init_userqueue(struct userqueue *user_queue);
Bool is_queue_empty(struct userqueue *user_queue);
struct userqueue *Enqueue(struct userqueue *user_queue, int client_index, int client_handle_time);
void Dequeue(struct userqueue *user_queue);
int get_front_node_handle_time(struct userqueue *user_queue);
#endif
