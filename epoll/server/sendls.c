#include "factory.h"

char pt(int type)
{
	if(type==4)
	{
		return 'd';
	}else if(type==8)
	{
		return '-';
	}
}

int sendls(int new_fd,char*path)
{
	DIR *dir;
	dir=opendir(path);
	if(NULL==dir)
	{
	        return -1;
	}
	int dfd=dirfd(dir);//dirfd,openat,fstat
	int ret;
	int fd=-1;
	struct dirent *p;
	struct stat buf;
	char buf1[128]={0};
	train t;
	memset(&t,0,sizeof(t));
//	sprintf(t.buf,"Filename            Type          Size            Change time");
	sprintf(t.buf,"文件名            文件类型      文件大小            修改时间");
	t.len=strlen(t.buf);
	ret=send(new_fd,&t,t.len+4,0);
//	printf("buf=%s,t.len=%d\n",t.buf,t.len);
	if(-1==ret)
	{
		perror("send");
		return -1;
	}
	while((p=readdir(dir))!=NULL)
	{
		if(!strncmp(p->d_name,".",1)||!strcmp(p->d_name,".."))
		{
			continue;
		}
//		memset(&buf,0,sizeof(struct stat));S
		fd=openat(dfd,p->d_name,O_RDONLY);//no
//		printf("fd=%d,p->d_name=%s\n",fd,p->d_name);
		if(fd==-1)
		{
			perror("openat");
			return -1;
		}
		ret=fstat(fd,&buf);	
		if(ret==-1)
		{
			perror("fstat");
			return -1;
		}
		buf.st_ctime+=3600*8;
		struct tm *p1;
		p1=gmtime(&buf.st_ctime);
		memset(&t,0,sizeof(t));
		sprintf(t.buf,"%-20s %c %15ldB          %04d-%02d-%02d %02d:%02d",p->d_name,pt(p->d_type),buf.st_size,p1->tm_year+1900,p1->tm_mon+1,p1->tm_mday,p1->tm_hour,p1->tm_min);
		t.len=strlen(t.buf);
//		printf("buf=%s,t.len=%d\n",t.buf,t.len);
		ret=send(new_fd,&t,t.len+4,0);
//		printf("ret=%d\n",ret);
		if(ret==-1)
		{
			perror("send");
			return -1;
		}
		strncpy(t.buf+strlen(t.buf),buf1,strlen(buf1));
		close(fd);
	}
	int flag=0;
	ret=send(new_fd,&flag,sizeof(int),0);
	if(ret==-1)
	{
		perror("send");
		return -1;
	}
	closedir(dir);
	return 0;
}
