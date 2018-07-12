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
	printf("connect success!\n");
	fd_set rdset;
	int fd,flag=0;
	int readret,sendret;
	char buf[1000]={0};
	char cmd[32]={0};
	train t;
	int flushflag = 0;
	int cmdflag = 1;
	int byeflag = 0;
	while(1){
		if(flushflag == 0){
			printf("ftp>");
			fflush(stdout);
		}
		FD_ZERO(&rdset);
		FD_SET(0,&rdset);
		FD_SET(sfd,&rdset);
		ret=select(sfd+1,&rdset,NULL,NULL,NULL);
		//printf("41:readret=%d,ret=%d\n",readret,ret);
		if(-1==ret){
			printf("select error\n");
			return;
		}
		if(ret>0){
			if(FD_ISSET(0,&rdset)){	
				//printf("(0,&rdset)\n");
				memset(buf,0,sizeof(buf));
				memset(&t,0,sizeof(t));
				readret=read(0,buf,sizeof(buf));
				//printf("readret=%d\n",readret);
				if(readret<=0){
				    printf("read error\n");
				    break;
				}
				t.len=readret-1;
				strncpy(t.buf,buf,t.len);
				sendret=send(sfd,&t,t.len+4,0);//发送命令
				system("clear");
				//printf("sendret=%d\n",sendret);
				if(-1==sendret){
				    printf("76:byebye\n");
				    continue;
				}
				if(4==sendret){
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
				//如何判断指令是否合法
			}
			if(FD_ISSET(sfd,&rdset)){	
				//printf("(sfd,&rdset)\n");
				if(readret<=0){
				    printf("byebye(服务器断开)\n");
				    break;
				}
				if(!strcmp(buf,"cd\n")||!strcmp(buf,"pwd\n")||!strncmp(buf,"cd ",3)){
						memset(buf,0,sizeof(buf));
						recv(sfd,&len,sizeof(int),0);
						recv(sfd,buf,len,0);
						printf("%s\n",buf);
				}
				else if(!strcmp(buf,"ls\n")||!strncmp(buf,"ls ",3)){
					recv_ls(sfd);
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
					//printf("120\n");
				}
				recv(sfd,&cmdflag,sizeof(int),0);
				if(0==cmdflag){
					printf("输入的指令不合法,请输入合法指令(cd,ls,remove,puts,gets,pwd)\n");
				}
				readret=0;
				flushflag = 0;
			}
			//printf("oooo\n");
		}
	}
	close(sfd);
	return 0;
}
