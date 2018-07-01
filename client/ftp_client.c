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
	while(1){
		if(flushflag == 0){
			printf(">");
			fflush(stdout);
		}
		FD_ZERO(&rdset);
		FD_SET(0,&rdset);
		FD_SET(sfd,&rdset);
		ret=select(sfd+1,&rdset,NULL,NULL,NULL);
		printf("41:readret=%d,ret=%d\n",readret,ret);
		if(-1==ret){
			printf("select error\n");
			return;
		}
		if(ret>0){
			if(FD_ISSET(0,&rdset)){	
				printf("(0,&rdset)\n");
				memset(buf,0,sizeof(buf));
				memset(&t,0,sizeof(t));
				readret=read(0,buf,sizeof(buf));
				t.len=readret-1;
				strncpy(t.buf,buf,t.len);
				sendret=send(sfd,&t,t.len+4,0);//发送命令
				system("clear");
				if(-1==sendret){
				    printf("76:byebye\n");
				    break;
				}
				flushflag = 1;
				if(t.len>5&&!strncmp(buf,"puts ",5)){//puts file
					printf("t.len=%d,t.buf=%s\n",t.len,t.buf);
					if(-1==send_file(sfd,t.buf)){
						printf("\nsend_file error\n");
						break;
					}else{
						printf("\nputs file success\n");
					}
					readret=0;
					flushflag = 0;
					continue;
				}
			}
			if(FD_ISSET(sfd,&rdset)){	
				printf("(sfd,&rdset)\n");
				if(readret<0){
				    printf("112:byebye\n");
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
				flushflag = 0;
			}
		}
	}
	close(sfd);
	return 0;
}
