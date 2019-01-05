#include "factory.h"

void factory_init(pfac p,int pth_num,int cap,threadfunc pfunc)
{
	p->pth=(pthread_t*)calloc(pth_num,sizeof(pthread_t));
	p->pth_num=pth_num;
	pthread_cond_init(&p->cond,NULL);
	p->queue.capability=cap;
	pthread_mutex_init(&p->queue.mutex,NULL);
	p->pfunc=pfunc;
}

void factory_start(pfac p)
{
	int i;
	if(p->startflg==0)
	{
		for(i=0;i<p->pth_num;i++)
		{
			pthread_create(&p->pth[i],NULL,p->pfunc,p);
		}
		p->startflg=1;
	}
}
