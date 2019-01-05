#include "func.h"

int send_file(int sfd,char* buf)
{
	int fd,flag=0;
	train t;
	char fname[128]={0};
	memset(&t,0,sizeof(t));
	memset(fname,0,sizeof(fname));
	strncpy(fname,buf+5,strlen(buf)-5);
//	sprintf(fname,"%c%s%c",'"',buf1,'"');
//	printf("fname=%s\n",fname);
	fd=open(fname,O_RDONLY);
//	printf("fd=%d\n",fd);
	if(-1==fd)
	{
		perror("open");
		return;
	}
	t.len=strlen(fname);
	strcpy(t.buf,fname);
	send(sfd,&t,4+t.len,0);
	struct stat statbuf;
	stat(fname,&statbuf);
	char *p;
	int mmapflag=0,offset=0;
	if(statbuf.st_size>100000000)
	{
		mmapflag=1;
	}
	send(sfd,&mmapflag,sizeof(int),0);
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
		while(memset(&t,0,sizeof(t)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0)
		{
		    if(-1==send_n(sfd,(char*)&t,4+t.len))
		    {
				printf("send_n error\n");
		        return -1;
		    }
		}
		flag=0;
		send_n(sfd,(char*)&flag,sizeof(int));
	}
	recv(sfd,&flag,sizeof(int),0);
	if(flag==1)
	{
		printf("puts file success!\n");
	}else{
		printf("puts file failure!\n");
	}
	return 0;
}
