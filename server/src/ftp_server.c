#include "../include/ftp.h"

void* thread(void* p)
{
	pfac pf=(pfac)p;
	pque pq=&pf->queue;
	pNode_t pcur;
	int ret;
	while(1)
	{
		pthread_mutex_lock(&pq->mutex);
		if(0==pq->size)
		{
			pthread_cond_wait(&pf->cond,&pq->mutex);
		}
		que_get(pq,&pcur);
		pthread_mutex_unlock(&pq->mutex);
		printf("accept success\n");
		strcpy(pcur->path,getcwd(NULL,0));
		strcpy(pcur->rpath,getcwd(NULL,0));//记录根目录
		fd_set rdset;
		int fd,recvlen;
		char buf[128]={0};
		short putsflag=0;
		train t;
		while(1)
		{
			FD_ZERO(&rdset);
			FD_SET(pcur->new_fd,&rdset);
			FD_SET(0,&rdset);
			ret=select(pcur->new_fd+1,&rdset,NULL,NULL,NULL);
			if(-1==ret)
			{
				perror("error select\n");
				break;
			}
			if(ret>0)
			{
				if(FD_ISSET(pcur->new_fd,&rdset))
				{	
					//初始化buf
					recvlen=0;
					memset(buf,0,sizeof(buf));
					//接收命令
					recv(pcur->new_fd,&recvlen,sizeof(int),0);
					recv(pcur->new_fd,buf,recvlen,0);
					//命令log
					cmd_syslog(buf,time(NULL));
					if(recvlen<0)//小于0？
					{
						printf("53:byebye\n");
						close(pcur->new_fd);
						break;
					}else if(recvlen==0){
						printf("byebye(用户退出)\n");
						break;
					}else if(recvlen<=3){
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
							printf("输入的指令不合法！\n");
							continue;
						}
					}else if(recvlen>3)
    					{
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
						else{
							printf("输入的指令不合法！\n");
							continue;
						}
					}else{
						continue;
					}
				}
			}
		}
		free(pcur);
		pcur=NULL;
	}
	close(pcur->new_fd);
}

int main(int argc,char *argv[])
{
	if(argc!=5)
	{
		printf("error args\n");
		return -1;
	}
	factory f;
    memset(&f,0,sizeof(f));
    int thread_num=atoi(argv[3]);
    int cap=atoi(argv[4]);
    factory_init(&f,thread_num,cap,thread);
    factory_start(&f);
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
	ret=listen(sfd,thread_num);
	if(-1==ret)
	{
		perror("listen");
		return -1;
	}
	struct sockaddr_in cli;
	memset(&cli,0,sizeof(cli));
	int len=sizeof(cli);
    char buf[5]={0};
	int new_fd;
	pque pq=&f.queue;
	while(1)
	{
		new_fd=accept(sfd,NULL,NULL);
		pNode_t pnew=(pNode_t)calloc(1,sizeof(Node_t));
		pnew->new_fd=new_fd;
		pthread_mutex_lock(&pq->mutex);
		que_insert(pq,pnew);
		pthread_mutex_unlock(&pq->mutex);
		pthread_cond_signal(&f.cond);
	}
	close(new_fd);
	close(sfd);
}
