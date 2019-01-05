#include "factory.h"

int send_file(int sfd,char *path,char* buf)
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
	char fname[128]={0};
	memset(&t,0,sizeof(t));
	memset(fname,0,sizeof(fname));
	strncpy(fname,buf+5,strlen(buf)-5);
//	sprintf(fname,"%c%s%c",'"',buf1,'"');
	printf("fname=%s\n",fname);
	fd=openat(dfd,fname,O_RDWR);
	printf("fd=%d\n",fd);
	if(-1==fd)
	{
		perror("open");
		return;
	}
	t.len=strlen(fname);
	strcpy(t.buf,fname);
	send(sfd,&t,4+t.len,0);
	int flag=0;
	long len=0;
	recv(sfd,&flag,sizeof(int),0);
	printf("server,flag=%d\n",flag);
	if(flag==1)
	{
		recv(sfd,&len,sizeof(long),0);//断点位置
	}
	printf("server,len=%d\n",(int)len);
	lseek(fd,(int)len,SEEK_SET);
	char *p;
	int mmapflag=0,offset=0;
	struct stat statbuf;
	stat(fname,&statbuf);
	if(statbuf.st_size-len>100000000)
	{
		mmapflag=1;
	}
	if(mmapflag==1)
	{
		printf("mmapflag=%d\n",mmapflag);
		p=(char*)mmap(NULL,statbuf.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
		if(p==(char*)-1)
		{
			perror("mmap");
			return -1;
		}
		for(int i=0;i<statbuf.st_size/1000;i++,p+=t.len)
		{
			memset(&t,0,sizeof(t));
			t.len=sizeof(t.buf);
			memcpy(t.buf,p,t.len);
			send(sfd,&t,4+t.len,0);
		}
		memset(&t,0,sizeof(t));
		t.len=statbuf.st_size%1000;
		memcpy(t.buf,p,t.len);
		send(sfd,&t,4+t.len,0);
	}else
	{
    }
	while(memset(&t,0,sizeof(t)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0)
	{
	//	printf("server,t.len=%d\n",t.len);
	    if(-1==send_n(sfd,(char*)&t,4+t.len))
	    {
			printf("send_n error\n");
	        return -1;
	    }
	}
	flag=0;
	send_n(sfd,(char*)&flag,sizeof(int));
	return 0;
}
