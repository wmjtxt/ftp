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

int recv_file(int new_fd)
{
	int fd;
	int len;
	int flag=0;
	char buf[1000]={0};
	struct dirent *p;
	struct stat statbuf;//文件信息
	recv(new_fd,&len,sizeof(int),0);//接收文件名长度
	memset(buf,0,sizeof(buf));
	if(-1==recv(new_fd,buf,len,0)){//接收文件名
		perror("recv");
		return -1;
	}
    fd=open(buf,O_RDWR);//打开文件
	if(-1==fd)
	{
		fd=open(buf,O_RDWR|O_CREAT,0666);//若文件不存在，创建文件
		if(-1==fd)
		{
			perror("open error");
			return -1;
		}
	}else{//说明文件已存在，断点续传flag置为1
		flag=1;
		stat(buf,&statbuf);//根据文件名读取文件信息
	}
	//记录文件大小,断点续传
	long recvsize = statbuf.st_size;//已下载文件大小,单位为B
	long totalsize;//文件总大小,单位为B
	int percent;//下载百分比,用于打印进度条,范围0-100
	int char_count = 0;
	recv(new_fd,&totalsize,sizeof(long),0);//接收文件大小

	send(new_fd,&flag,sizeof(int),0);
	//printf("client,flag=%d\n",flag);
	if(flag==1)
	{
		//发送断点位置
		send(new_fd,&statbuf.st_size,sizeof(long),0);
	}
	//printf("client,size=%d\n",(int)statbuf.st_size);
	lseek(fd,(int)statbuf.st_size,SEEK_SET);//找到断点位置

	//printf("recvsize=%d\n",(int)recvsize);
	//printf("totalsize=%d\n",(int)totalsize);

	while(1)
	{
	    recv_n(new_fd,(char*)&len,sizeof(int));
	    if(len>0)
	    {
			recvsize += (long)len;
			percent = 100*(1.0*recvsize / totalsize);
			char_count = display_progress(percent,char_count);
	        memset(buf,0,sizeof(buf));
	        if(-1==recv_n(new_fd,buf,len))
	        {
				printf("\ndownload break off\n");
	            break;
	        }
	        write(fd,buf,len);
	    }else{
			printf("\ndownload file success\n");
	        break;
	    }
	}
	return 0;
}

int send_file(int sfd,char* buf)
{
	int fd;
	train t;
	char fname[128]={0};
	memset(&t,0,sizeof(t));
	memset(fname,0,sizeof(fname));
	strncpy(fname,buf+5,strlen(buf)-5);
	fd=open(fname,O_RDONLY);
	if(-1==fd)
	{
		perror("open file error");
		return;
	}

	//记录文件大小
	struct stat statbuf;
	int ret = fstat(fd,&statbuf);//读取文件信息
	if(-1==ret)
	{
	    perror("fstat error");
	    return -1;
	}
	long sendsize = 0;//已上传文件大小,单位为B
	long totalsize = statbuf.st_size;//文件总大小,单位为B
	int percent;//上传百分比,用于打印进度条,范围0-100
	int char_count = 0;

	//先传输文件名
	t.len=strlen(fname);
	strcpy(t.buf,fname);
	send(sfd,&t,4+t.len,0);//发送文件名小火车

	int fflag = 0;//文件是否存在标志
	long len = 0;//已上传文件大小
	recv(sfd,&fflag,sizeof(int),0);
	if(1==fflag)
	{
		recv(sfd,&len,sizeof(long),0);
	}
	lseek(fd,(int)len,SEEK_SET);//找到断点位置
	sendsize += len;
	//开始文件传输
	while(memset(&t,0,sizeof(t)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0)//读取文件内容到小火车
	{
		sendsize += (long)t.len;
		percent = 100*(1.0*sendsize / totalsize);//long->double->int
		char_count = display_progress(percent,char_count);
	    if(-1==send_n(sfd,(char*)&t,4+t.len))//发送
	    {
			printf("send_n error\n");
	        return -1;
	    }
	}
	//发送结束标志
	int flag = 0;
	send(sfd,&flag,sizeof(flag),0);
	close(fd);
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
