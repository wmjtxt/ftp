#include "func.h"

int recv_ls(int new_fd)
{
	int fd;
	int len,ret;
	char buf[1000]={0};
	ret=recv(new_fd,&len,sizeof(int),0);
	ret=recv(new_fd,buf,len,0);
	printf("%s\n",buf);
	while(1)
	{
	    recv(new_fd,(char*)&len,sizeof(int),0);
	    if(len>0)
	    {
	        memset(buf,0,sizeof(buf));
	        if(-1==recv(new_fd,buf,len,0))
	        {
	            return -1;
	        }
			printf("%s\n",buf);
	    }else{
	        break;
	    }
	}
	return 0;
}
