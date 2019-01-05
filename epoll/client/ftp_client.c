#include "func.h"
int main(int argc,char** argv)
{
	if(argc!=3){
		printf("error args\n");
		return -1;
	}
	int sfd;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd){
		perror("socket");
		return -1;
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret,len=-1;
	ret=connect(sfd,(struct sockaddr*)&ser,sizeof(ser));
	if(-1==ret){
		printf("errno=%d\n",errno);
		perror("connect");
		return -1;
	}
	fd_set rdset;
	int fd,flag=0;
	int readret,sendret;
	char buf[1000]={0};
	char cmd[32]={0};
	int logflag=0;
	char *passwd,pname[20]={0};
	train t;
	int epfd=epoll_create(1);
	struct epoll_event event,evs[3];
	memset(&event,0,sizeof(event));
	event.events=EPOLLIN;
	event.data.fd=sfd;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	if(-1==ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	memset(&event,0,sizeof(event));
	event.events=EPOLLIN;
	event.data.fd=0;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,0,&event);
	if(-1==ret)
	{
		perror("epoll_ctl");
		return -1;
	}
	int i;
	while(1){
	    memset(evs,0,sizeof(evs));
		if(logflag==0)
		{
			printf("请输入用户名:\n");
		}
        ret = epoll_wait(epfd,evs,3,-1);
		printf("41:readret=%d,ret=%d\n",readret,ret);
		if(-1==ret){
			printf("epoll error\n");
			return;
		}
		if(ret>0){
            for(i = 0; i < ret; i++){
                if(evs[i].data.fd == 0){
				    printf("0\n");
				    if(logflag==1)
				    {
				    	memset(buf,0,sizeof(buf));
				    	memset(&t,0,sizeof(t));
				    	readret=read(0,buf,sizeof(buf));
				    	t.len=readret-1;
				    	strncpy(t.buf,buf,t.len);
				    	sendret=send(sfd,&t,t.len+4,0);//发送命令
				    	system("clear");
				    }
				    if(logflag==0)
				    {
				    	memset(pname,0,sizeof(pname));
				    	memset(&t,0,sizeof(t));
				    	read(0,pname,sizeof(pname));
				    	t.len=strlen(pname)-1;
				    	strncpy(t.buf,pname,strlen(pname)-1);
				    	sendret=send(sfd,&t,t.len+4,0);//发送用户名
				    	if(-1==sendret){
	//			    	    printf("66:byebye\n");
				    	    break;
				    	}
				    	memset(&t,0,sizeof(t));
				    	passwd=getpass("请输入密码:");
				    	t.len=strlen(passwd);
				    	strcpy(t.buf,passwd);
				    	sendret=send(sfd,&t,t.len+4,0);//发送密码
				    	readret=strlen(passwd)+strlen(pname)-1;
				    }
				    if(-1==sendret){
	//			        printf("76:byebye\n");
				        break;
				    }
				    if(t.len>5&&!strncmp(buf,"puts ",5)){//puts file
				    	if(-1==send_file(sfd,t.buf)){
				    		printf("send_file error\n");
				    		break;
				    	}
				    	readret=0;
				    }
//				    printf("sendret=%dreadret=%d\n",sendret,readret);
//				    printf("71:buf=%s\n",buf);
				}
                if(evs[i].data.fd == sfd){
				    printf("rdset\n");
				    if(readret<=0){
//				        printf("112:byebye\n");
				        break;
				    }
				    if(logflag==0){
				    	recv(sfd,&logflag,sizeof(int),0);
				    	if(logflag==0)
				    	{
				    		printf("用户名或密码错误！\n");
				    		readret=0;
				    		continue;
				    	}else{
				    		printf("登录成功！\n");
				    	}
				    }
				    else if(!strcmp(buf,"cd\n")||!strcmp(buf,"pwd\n")||!strncmp(buf,"cd ",3)){
				    		memset(buf,0,sizeof(buf));
				    		recv(sfd,&len,sizeof(int),0);
				    		recv(sfd,buf,len,0);
				    		printf("%s\n",buf);
				    }
				    else if(!strcmp(buf,"ls\n")||!strncmp(buf,"ls ",3)){
				    	recv_ls(sfd);
				    }
				    else if(strncmp(buf,"gets ",5)==0){
				    	recv_file(sfd);
				    }
				    else if(strncmp(buf,"remove ",7)==0)
				    {
				    	recv(sfd,&flag,sizeof(int),0);
				    	if(flag==0){
				    		printf("remove success!\n");
				    	}else if(flag==-1){
				    		printf("remove failed!\n");
				    	}
				    }
				    readret=0;
                }
			}
		}
	}
	close(sfd);
	return 0;
}
