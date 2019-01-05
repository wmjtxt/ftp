#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#define FILENAME "file"
typedef void* (*threadfunc)(void*);

typedef struct{
	int len;
	char buf[1000];
}train,*ptrain;
typedef struct{
	pthread_t *pth;
	int pth_num;
	pthread_cond_t cond;
	que_t queue;//放描述符的队列
	threadfunc pfunc;	
	short startflg;
}factory,*pfac;

#endif
