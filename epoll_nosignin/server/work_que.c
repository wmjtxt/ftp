#include "work_que.h"

void que_insert(pque p,pNode_t pnew)
{
	if(p->phead==NULL)
	{
		p->phead=pnew;
		p->ptail=pnew;
	}else{
		p->ptail=pnew;
		p->phead=p->phead->pnext;
	}
	++(p->size);
}

void que_get(pque p,pNode_t* pcur)
{
	*pcur=p->phead;
	p->phead=p->phead->pnext;
	p->size--;
}
