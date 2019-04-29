#ifndef CLIENTLIST_H
#define CLIENTLIST_H

struct cnode{
	int index;			/*�ͻ����*/
	int enter_time;		/*�ͻ����������¼�����ʱ��ʱ���*/
	int wait_time;		/*�ͻ����ҵ�������ȴ���ȫ��ʱ��*/ 
	struct cnode *next; 
};

struct client_list{
	struct cnode *client_data;		/*ָ��洢�û���Ϣ������ͷ����ָ��*/
	struct cnode *rear;				/*ָ�����������һ���ͻ�����ָ��*/
	int client_counts;				/*�ͻ��洢���д洢�Ŀͻ�����*/ 
};

struct client_list *init_clientlist(struct client_list *clientlist);
struct client_list *add_client_data(struct client_list *clientlist, int client_index, int client_enter_time, int client_wait_time); 
#endif
