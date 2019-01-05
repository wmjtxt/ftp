#include "func.h"

int send_n(int new_fd,char *buf,int len)
{
	int ret;
	int total=0;
	while(total<len)
	{
		ret=send(new_fd,buf+total,len-total,0);
		if(ret>=0)
		{
			total=total+ret;
		}else{
			return -1;
		}
	}
	return 0;
}

int recv_n(int new_fd,char *buf,int len)
{
	int ret;
	int total=0;
	while(total<len)
	{
		ret=recv(new_fd,buf+total,len-total,0);
		if(ret>=0)
		{
			total=total+ret;
		}else{
			return -1;
		}
	}
	return 0;
}
