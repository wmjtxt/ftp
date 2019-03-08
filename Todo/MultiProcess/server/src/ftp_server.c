#include "../include/ftp.h"


int rm_file(char *path,char *buf)
{
	char pname[32]={0};
	strncpy(pname,buf+7,strlen(buf)-7);
	DIR *dir;
	dir=opendir(path);
	int dfd=dirfd(dir);
	if(-1==unlinkat(dfd,pname,0))
	{
		return -1;
	}else{
		return 0;
	}
}

void help(void)
{
	printf("用户名密码验证程序\n第一个参数为用户名\n");
	exit(-1);
}

void error_quit(char* msg)
{
	perror(msg);
	exit(-2);
}

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
	ret=send(new_fd,&t,t.len+4,0);//t.len为什么要+4?小火车?
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

int filename(char* buf)
{
	char fname[128]={0};
	printf("buf1=%s\n",buf);
	memset(fname,0,sizeof(fname));
	strncpy(fname,(buf)+7,strlen(buf)-7);
	printf("fname=%s\n",fname);
	strcpy(buf,fname);
	return 0;
}

int ls_any(int new_fd,char* path1,char* buf)
{
	char path[128]={0};
	strcpy(path,path1);
	int ret=sendls(new_fd,buf);
	if(-1==ret)
	{
		char buf1[128]={0};
		train t;
	    strncpy(buf1,buf+3,strlen(buf)-3);
		if(!strcmp(buf1,".."))
		{
			int plen;
			plen=strlen(path);
			if(path[plen-1]=='/'){
				plen--;
			}
			while(path[plen-1]!='/')
			{
				plen--;
			}
			plen=plen-1;
			memset(buf1,0,sizeof(buf1));
			strncpy(buf1,path,plen);
			strcpy(path,buf1);
		}else{
			sprintf(path,"%s/%s",path,buf1);
		//	strcpy((*path+strlen((*path),buf1);
		}
		t.len=strlen(path);
	    strcpy(t.buf,path);
		int ret=sendls(new_fd,path);
		if(-1==ret)
		{
			perror("send");
			return -1;
		}
	}
	return 0;
}

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
	int ret;
	train t;//小火车
	memset(&t,0,sizeof(t));
	int len;
	char buf[1000]={0};
	int flag = 0;//文件是否存在
	int fileErrorFlag = 0;//文件发送是否错误标志
	struct stat statbuf;
	long recvsize = 0;
	recv(new_fd,&fileErrorFlag,sizeof(int),0);
	if(1==fileErrorFlag)
	{
		return -1;
	}
	recv(new_fd,&len,sizeof(int),0);//接收火车头
	memset(buf,0,sizeof(buf));
	if(-1==recv(new_fd,buf,len,0)){//接收文件名
		perror("recv");
		return -1;
	}
	fd = open(buf,O_RDWR);//打开文件
	if(-1==fd)
	{
		fd=openat(dfd,buf,O_RDWR|O_CREAT,0666);//若文件不存在，则新建文件
		if(-1==fd)
		{
		    perror("open error");
		    return -1;
		}
	}else{
		flag = 1;
		stat(buf,&statbuf);
		recvsize = statbuf.st_size;//已上传文件大小,单位为B
	}
	//记录文件大小,断点续传
	//int totalsize;//文件总大小,单位为B
	//int percent;//上传百分比,用于打印进度条,范围0-100
	//int char_count = 0;

	send(new_fd,&flag,sizeof(int),0);//告知客户端文件是否存在
	if(1==flag)
	{
		send(new_fd,&recvsize,sizeof(long),0);//发送断点位置
	}
	lseek(fd,(int)recvsize,SEEK_SET);//找到断点位置
	//开始接收文件
	while(1)
	{
		recv(new_fd,&len,sizeof(int),0);
		if(len==0) break;
	    recv_n(new_fd,buf,len);
	    write(fd,buf,len);
	}
	close(fd);
	return 0;
}

int send_file(int sfd,char *path,char* buf)
{
	DIR *dir;
	dir=opendir(path);//锁定文件夹
	if(NULL==dir)
	{
		return -1;
	}
	int dfd=dirfd(dir);
	int fd;
	int fileErrorFlag = 0;
	train t;
	char fname[128]={0};
	memset(&t,0,sizeof(t));
	memset(fname,0,sizeof(fname));
	strncpy(fname,buf+5,strlen(buf)-5);
//	sprintf(fname,"%c%s%c",'"',buf1,'"');
	printf("fname=%s\n",fname);
	fd=openat(dfd,fname,O_RDWR);//锁定文件
	if(-1==fd)
	{
		fileErrorFlag = 1;
		send(sfd,&fileErrorFlag,sizeof(int),0);
		perror("open file error");
		return -1;
	}else{
		printf("file is\n");
		send(sfd,&fileErrorFlag,sizeof(int),0);
	}
	t.len=strlen(fname);
	strcpy(t.buf,fname);
	send(sfd,&t,4+t.len,0);//传输文件名
	//传输文件大小
	struct stat statbuf;
	int ret = fstat(fd,&statbuf);//根据文件描述符读取文件信息
	if(-1==ret)
	{
	    perror("fstat error");
	    return -1;
	}
	send(sfd,&statbuf.st_size,sizeof(long),0);//发送文件总大小

	int flag=0;//文件是否存在
	long len=0;//已下载文件大小
	recv(sfd,&flag,sizeof(int),0);
	//printf("server,flag=%d\n",flag);
	if(flag==1)
	{
		recv(sfd,&len,sizeof(long),0);//接收断点位置
	}
	//printf("server,len=%d\n",(int)len);
	lseek(fd,(int)len,SEEK_SET);//找到断点位置
	//开始文件传输
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

void login_syslog(char *usrname,time_t t)
{
	printf("username:%s,",usrname);
	struct tm *p;
	p=localtime(&t);
	syslog(LOG_NOTICE,"LOGIN  username:%s,time:%04d-%02d-%02d %02d:%02d\n",usrname,p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min);
	return;
}

void cmd_syslog(char *cmd,time_t t)
{
	struct tm *p;
	p=localtime(&t);
	syslog(LOG_NOTICE,"CMD cmdname:%s,time:%04d-%02d-%02d %02d:%02d\n",cmd,p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min);
	return;
}

int send_cd(char* buf,pNode_t* pcur)
{
	int plen,rplen,flag=0;
	char addpath[128]={0};
	train t;
    strncpy(addpath,buf+3,strlen(buf)-3);
	if(!strcmp(addpath,".."))
	{
		plen=strlen((*pcur)->path);
		rplen=strlen((*pcur)->rpath);
		if(plen > rplen){
			while((*pcur)->path[plen---1]!='/');//找上一个/
			memset(addpath,0,sizeof(addpath));
			strncpy(addpath,(*pcur)->path,plen);
			strcpy((*pcur)->path,addpath);
		}
	}else{
		plen=strlen(addpath);
		int i=plen,j=0;
		char pname[32]={0};
		while(addpath[--i]=='/');
		plen=i+1;
		i=0;
		printf("plen=%d\n",plen);
		addpath[plen]='/';
		plen++;
		while(i<plen)
		{
			if(addpath[i]=='/'){
				printf("pname=%s\n",pname);
		//		pname[j++]='\0';
				DIR *dir;
				dir=opendir((*pcur)->path);
				struct dirent *p;
				if(NULL==dir)
				{
					printf("opendir error\n");
					return -1;
				}
				while((p=readdir(dir))!=NULL)
				{
				printf("p->d_name=%s\n",p->d_name);
				printf("pname=%s\n",pname);
					if(!strcmp(p->d_name,pname)&&p->d_type==4)
					{
						sprintf((*pcur)->path,"%s/%s",(*pcur)->path,pname);
						flag=1;
						break;
					}
				}
				if(flag==0){
					return -1;
				}
				memset(pname,0,sizeof(pname));
				j=0;
				i++;
			}else{
				pname[j++]=addpath[i++];
			}
		}
	//	sprintf((*pcur)->path,"%s/%s",(*pcur)->path,addpath);
	//	strcpy((*pcur)->path+strlen((*pcur)->path),addpath);
	}
//	if((*pcur)->path[plen-1]=='/'){
//		plen--;
//	}
	printf("path=%s\n",(*pcur)->path);
	t.len=strlen((*pcur)->path) - strlen((*pcur)->rpath);
	if(t.len == 0){
		strcpy(t.buf,"/");
		t.len++;
	}else{
		strncpy(t.buf, (*pcur)->path + strlen((*pcur)->rpath), t.len);
	}
    int ret=send((*pcur)->new_fd,&t,t.len+4,0);
	if(-1==ret)
	{
		perror("send");
		return -1;
	}
	return 0;
}


//显示下载进度条
/*progress为进度百分比，取值为0~100, last_char_count为上一次显示进度条时所用到的字符个数*/
int display_progress(int progress, int last_char_count)
{
	int i = 0;

	/*把上次显示的进度条信息全部清空*/
    for (i = 0; i < last_char_count; i++)
    {
        printf("\b"); 
    }

    /*此处输出‘=’，也可以是其他字符，仅个人喜好*/
    for (i = 0; i < progress; i++)
    {
            printf("=");  
    }
    printf(">>");
    /*输出空格截止到第104的位置，仅个人审美*/
    for (i += 2; i < 104; i++) 
    {
            printf(" ");
    }
    /*输出进度条百分比*/
    i = i + printf("[%d%%]", progress);  
    /*此处不能少，需要刷新输出缓冲区才能显示，
    这是系统的输出策略*/
    fflush(stdout);

    /*返回本次显示进度条时所输出的字符个数*/ 
    return i; 
}
