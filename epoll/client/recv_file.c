#include "func.h"

int recv_file(int new_fd)
{
	int fd;
	int len;
	int flag=0;
	char buf[1000]={0};
	struct dirent *p;
	struct stat statbuf;
	recv(new_fd,&len,sizeof(int),0);
	memset(buf,0,sizeof(buf));
	if(-1==recv(new_fd,buf,len,0)){
		perror("recv");
		return -1;
	}
    fd=open(buf,O_RDWR);
	if(-1==fd)
	{
		fd=open(buf,O_RDWR|O_CREAT,0666);
		if(-1==fd)
		{
			perror("open");
			return -1;
		}
	}else{
		flag=1;
		stat(buf,&statbuf);
	}
	send(new_fd,&flag,sizeof(int),0);
	printf("client,flag=%d\n",flag);
	if(flag==1)
	{
		send(new_fd,&statbuf.st_size,sizeof(long),0);
	}
	printf("client,size=%d\n",(int)statbuf.st_size);
	lseek(fd,(int)statbuf.st_size,SEEK_SET);
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
	}
	return 0;
}
