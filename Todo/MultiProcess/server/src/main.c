#include "../include/ftp.h"


void run(pNode_t pcur)
{
	int ret;
    int quitflag = 0;
	while(1)
	{
        quitflag = 0;
		printf("accept success\n");
		strcpy(pcur->path,getcwd(NULL,0));
		strcpy(pcur->rpath,getcwd(NULL,0));//记录根目录
		fd_set rdset;
		int fd,recvlen;
		char buf[128]={0};
		short putsflag = 0;
		int cmdflag = 1;//指令是否合法
		int byeflag = 0;
		train t;
		int epfd = epoll_create(3);
		if(-1 == epfd)
		{
			perror("epoll_create");
			return -1;
		}
		struct epoll_event event;
		struct epoll_event *evs = (struct epoll_event *)calloc(2,sizeof(event));
		memset(&event,0,sizeof(event));
		event.events = EPOLLIN;
		event.data.fd = pcur->new_fd;
		//注册sfd描述符的可读事件
		ret=epoll_ctl(epfd,EPOLL_CTL_ADD,pcur->new_fd,&event);
		if(-1 == ret)
		{
			perror("epoll_ctl");
			return -1;
		}
		while(1)
		{
			cmdflag = 1;
			memset(evs, 0, 2*sizeof(event));
			ret = epoll_wait(epfd, evs, 2, -1);
			if(-1 == ret)
			{
				perror("error epoll\n");
				break;
			}
			for(int i = 0; i < ret; i++)
            {
                if(evs[i].data.fd==pcur->new_fd)
				{	
					//初始化buf
					recvlen=0;
					memset(buf,0,sizeof(buf));
					//接收命令
					recv(pcur->new_fd,&recvlen,sizeof(int),0);
					int cmdret = recv(pcur->new_fd,buf,recvlen,0);
			        if(-1 == cmdret)
			        {
			        	perror("error recv\n");
			        	break;
			        }
					//命令log
					cmd_syslog(buf,time(NULL));
					//printf("recvlen=%d\n",recvlen);
					printf("cmd=%s\n",buf);
					if(recvlen==0){//输入ctrl+c和直接输入回车,recvlen都是0,怎么区分开?
						byeflag = 0;
						recv(pcur->new_fd,&byeflag,sizeof(int),0);
						//printf("byeflag=%d\n",byeflag);
						if(byeflag){
							cmdflag = 0;
							//printf("用户输入回车\n");
						}
						else{
							printf("75 : 用户退出(crtl+c)\n");
                            quitflag = 1;
						}
					}
					//printf("cmdret=%d\n",cmdret);
					if(recvlen<=3){
						if(!strcmp(buf,"cd")||!strcmp(buf,"cd "))
						{
							t.len=1;
							strcpy(pcur->path,pcur->rpath);
							strcpy(t.buf,"/");
    			       		send(pcur->new_fd,&t,t.len+4,0);
							printf("cd success\n");
    			        }else if(!strcmp(buf,"ls")||!strcmp(buf,"ls "))
						{
							int lsret = sendls(pcur->new_fd,pcur->path);
							if(lsret == -1){
								printf("ls failed\n");
							}else{
								printf("ls success\n");
							}
						}else if(strcmp(buf,"pwd")==0)
						{
							t.len=strlen(pcur->path)-strlen(pcur->rpath);
							if(t.len == 0){
								strcpy(t.buf,"/");
								t.len++;
							}
							else
								strncpy(t.buf,pcur->path+strlen(pcur->rpath),t.len);
							printf("t.len = %d t.buf = %s\n",t.len,t.buf);
							if(-1 != send(pcur->new_fd,&t,t.len+4,0)){
								printf("pwd success\n");
							}else 
								printf("pwd failed\n");
						}else{//输入非法指令
							cmdflag = 0;
							printf("输入的指令不合法！\n");
							//continue;
						}
					}else if(recvlen>3){
						if(strncmp(buf,"cd ",3)==0)//cd ../client有bug
						{
							int cdret = send_cd(buf,&pcur);
							if(cdret == -1){
								printf("%s failed\n",buf);
							}else{
								printf("%s success\n",buf);
							}
						}
						else if(strncmp(buf,"ls ",3)==0)
						{
							int lsret = ls_any(pcur->new_fd,pcur->path,buf);
							if(lsret == -1){
								printf("ls failed\n");
							}else{
								printf("ls success\n");
							}
						}
						else if(!strncmp(buf,"puts ",5))
						{
							int recvret = recv_file(pcur->new_fd,pcur->path);
							if(recvret == -1){
								printf("upload failed\n");
							}else{
								printf("upload success\n");
							}
						}
						else if(strncmp(buf,"gets ",5)==0)
						{
							int sendret = send_file(pcur->new_fd,pcur->path,buf);
							if(sendret == -1){
								printf("download failed\n");
							}else{
								printf("download success\n");
							}
						}
						else if(strncmp(buf,"remove ",7)==0)
						{
							int rmret=rm_file(pcur->path,buf);
							if(rmret == -1){
								printf("remove file failed\n");
							}else{
								printf("remove file success\n");
							}
							send(pcur->new_fd,&rmret,sizeof(int),0);
						}
						else if(!strcmp(buf,"quit") || !strcmp(buf,"exit")){
							printf("quit(用户退出)\n");
                            quitflag = 1;
							break;
						}
						else{
							cmdflag = 0;
							printf("输入的指令不合法！\n");
							//continue;
						}
					}
					send(pcur->new_fd,&cmdflag,sizeof(int),0);
				}
			}
            if(quitflag)
                break;
		}
		free(pcur);
		pcur=NULL;
	}
	close(pcur->new_fd);
}

int main(int argc,char *argv[])
{
	if(argc != 4)
	{
		printf("error args\n");
		return -1;
	}
    int cap=atoi(argv[3]);
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	if(-1==ret)
	{
		perror("bind");
		return -1;
	}
	ret=listen(sfd,2);
	if(-1==ret)
	{
		perror("listen");
		return -1;
	}
	struct sockaddr_in cli;
	memset(&cli,0,sizeof(cli));
	int len = sizeof(cli);
    char buf[5]={0};
	int new_fd;
	new_fd = accept(sfd,NULL,NULL);
	pNode_t pnew = (pNode_t)calloc(1,sizeof(Node_t));
	pnew->new_fd = new_fd;
    run(pnew);
	close(new_fd);
	close(sfd);
}
