#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"
typedef struct tag_node{
	int new_fd;
	int sfd;
	char path[128];
	struct tag_node* pnext;
}Node_t,*pNode_t;
typedef struct{
	pNode_t phead,ptail;
	int capability;
	int size;
	pthread_mutex_t mutex;
}que_t,*pque;

#endif
