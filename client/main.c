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
		return -1
	}
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(ser));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret,len=-1;
	ret = connect(sfd,(struct sockaddr*)&ser,sizeof(ser));//connect
	printf("ret = %d\n", ret);
	if(-1==ret){
		printf("errno=%d\n",errno);
		perror("connect");
		return -1;
	}
	printf("connect success!\n");
    printf("请输入指令: cd,ls,remove,puts,gets,pwd");
    printf("需要帮助请输入h\n");

	int readret,sendret;
	char buf[1000]={0};
	char cmd[32]={0};
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
	//fd_set rdset;
	int i;
	int logflag=0;
    int flushflag = 0;
	int fd,flag=0;
	int cmdflag = 1;
	int byeflag = 0;
    int hflag = 0;
	while(1){
		if(flushflag == 0){
			printf("ftp>");
			fflush(stdout);
		}
        ret = epoll_wait(epfd,evs,3,-1);
		//printf("70 : readret=%d, ret=%d\n",readret,ret);
		if(-1==ret){
			printf("epoll_wait error\n");
			return;
		}
		//printf("76: ret = %d\n", ret);
		if(ret>0){
            for(i = 0; i < ret; i++){
			    if(evs[i].data.fd == 0){	
			    	//printf("(0,&rdset)\n");
			    	memset(buf,0,sizeof(buf));
			    	memset(&t,0,sizeof(t));
			    	readret=read(0,buf,sizeof(buf));
			    	//printf("84: readret=%d\n",readret);
			    	if(readret <= 0){
			    	    printf("read error\n");
			    	    break;
			    	}
			    	t.len=readret-1;
			    	strncpy(t.buf,buf,t.len);
			    	sendret=send(sfd,&t,t.len+4,0);//发送命令
			    	system("clear");
			    	//printf("sendret=%d\n",sendret);
			    	if(-1==sendret){
			    	    printf("95:byebye\n");
			    	    continue;
			    	}
			    	if(4 == sendret){
			    	    //printf("输入回车\n");
			    		byeflag = 1;
			    		send(sfd,&byeflag,sizeof(int),0);
			    	}
			    	flushflag = 1;
			    	//printf("t.len=%d,t.buf=%s\n",t.len,t.buf);
			    	if(!strncmp(buf,"puts ",5)){//puts file
			    		if(-1==send_file(sfd,t.buf)){
			    			printf("\nupload file error\n");
			    			flushflag = 0;
			    			continue;
			    		}else{
			    			printf("\nupload file success\n");
			    		}
			    	}
			    	//quit
			    	if(!strcmp(buf,"quit\n") || !strcmp(buf,"exit\n")){
			    		printf("quit/exit\n");
			    		return;
			    	}
                    // help
			    	if(!strcmp(buf,"h\n") || !strcmp(buf,"help\n")){
			    		printf("指令含义\n");
			    		printf("cd       : 进入根目录\n");
                        printf("cd [dir] : 进入dir文件夹\n");
                        printf("ls       : 输出当前路径下文件信息\n");
                        printf("pwd      : 输出当前路径\n");
                        printf("remove [file] : 删除文件file\n");
                        printf("puts [file]   : 上传文件file\n");
                        printf("gets [file]   : 下载文件file\n");
                        printf("h(elp)    : 显示帮助信息\n");
                        printf("quit/exit : 退出\n");
                        hflag = 1;
			    		break;
			    	}else{
                        hflag = 0;
                    }
			    	//如何判断指令是否合法
			    }
			    if(evs[i].data.fd == sfd){	
			    	//printf("(sfd,&rdset)\n");
			    	if(readret<=0){
			    	    printf("byebye(服务器断开)\n");
			    	    return;
			    	}
			    	if(!strcmp(buf,"cd\n")||!strcmp(buf,"pwd\n")||!strncmp(buf,"cd ",3)){
			    			memset(buf,0,sizeof(buf));
                            int cdflag = 0;
                            recv(sfd,&cdflag,sizeof(int),0);
                            if(cdflag == 0){
			    			    recv(sfd,&len,sizeof(int),0);
			    			    recv(sfd,buf,len,0);
			    			    printf("%s\n",buf);
                            }else if(cdflag == -1){
			    			    printf("cd failed\n");
                            }
			    	}
			    	else if(!strcmp(buf,"ls\n")||!strncmp(buf,"ls ",3)){
                        int lsflag = 0;
			    		recv(sfd,&lsflag,sizeof(int),0);
                        printf("lsflag = %d\n",lsflag);
			    		if(lsflag == 1){
			    			printf("ls success!\n");
			    		    recv_ls(sfd);
			    		}else if(lsflag == 0){
			    			printf("ls failed!\n");
			    		}
			    	}
			    	else if(strncmp(buf,"gets ",5)==0){
			    		if(-1==recv_file(sfd)){
			    			printf("download_file error\n");
			    		}else{
			    			printf("download file success\n");
			    		}
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
			    	else{
			    		printf("170\n");
			    	}
			    	recv(sfd,&cmdflag,sizeof(int),0);
			    	if(0 == cmdflag && !hflag){
			    		printf("输入的指令不合法,请输入合法指令(cd,ls,remove,puts,gets,pwd),需要帮助输入h\n");
                        hflag = 0;
			    	}
			    	readret=0;
			    	flushflag = 0;
			    }
			    printf("180\n");
            }//end for
		}//end if
	}//end while
	close(sfd);
	return 0;
}
