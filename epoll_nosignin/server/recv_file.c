#include "factory.h"

int recv_file(int new_fd,char *path)
{
	DIR *dir;
	dir=opendir(path);
	if(NULL==dir)
	{
		return -1;
	}
	int dfd=dirfd(dir);
	int fd;
	train t;
	memset(&t,0,sizeof(t));
	int len;
	char buf[1000]={0};
	recv(new_fd,&len,sizeof(int),0);
	memset(buf,0,sizeof(buf));
	if(-1==recv(new_fd,buf,len,0)){
		perror("recv");
		return -1;
	}
    fd=openat(dfd,buf,O_RDWR|O_CREAT,0666);
	if(-1==fd)
	{
	    perror("open");
	    return -1;
	}
	while(1)
	{
	    recv_n(new_fd,(char*)&len,sizeof(int));
	    if(len>0)
	    {
	        memset(buf,0,sizeof(buf));
	        if(-1==recv_n(new_fd,buf,len))
	        {
	            break;
	        }
	        write(fd,buf,len);
	    }else{
	        break;
	    }
		int flag=1;
		int ret=send(new_fd,&flag,sizeof(int),0);
		if(-1==ret)
		{
		    perror("send");
		    return -1;
		}
	}
	return 0;
}
