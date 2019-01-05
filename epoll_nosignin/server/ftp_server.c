#include "factory.h"

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
		fd_set rdset;
		int fd,recvlen,rmret;
		char buf[128]={0};
		short putsflag=0;
		int logflag=0;
		char passwd[128]={0};
		char pname[128]={0};
		train t;
		int epfd=epoll_create(1);
		struct epoll_event event;
		struct epoll_event *evs=(struct epoll_event *)calloc(pf->pth_num+1,sizeof(event));
		memset(&event,0,sizeof(event));
		event.events=EPOLLIN;
		event.data.fd=pcur->new_fd;
		//注册sfd描述符的可读事件
		ret=epoll_ctl(epfd,EPOLL_CTL_ADD,pcur->new_fd,&event);
		if(-1==ret)
		{
			perror("epoll_ctl");
			return -1;
		}
		while(1)
		{
//			printf("begin\n");
			memset(evs,0,(pf->pth_num+1)*sizeof(event));
			ret=epoll_wait(epfd,evs,pf->pth_num+1,-1);
			if(-1==ret)
			{
				perror("error epoll\n");
				break;
			}
			for(int i = 0; i < ret; i++)
            {
                if(evs[i].data.fd==pcur->new_fd)
                {
//					printf("rdset\n");
					if(1)
					{
						recvlen=0;
						memset(buf,0,sizeof(buf));
						recv(pcur->new_fd,&recvlen,sizeof(int),0);
						recv(pcur->new_fd,buf,recvlen,0);//接收命令
						cmd_syslog(buf,time(NULL));
					}
//					printf("recv %s,size %d,logflag=%d\n",buf,recvlen,logflag);
					if(recvlen<0)
					{
//						printf("73:byebye\n");
						close(pcur->new_fd);
						break;
					}else if(recvlen==0){
//						printf("77:byebye\n");
						break;
					}else if(recvlen<=3){
						if(!strcmp(buf,"cd")||!strcmp(buf,"cd "))
						{
							t.len=9;
							strcpy(pcur->path,"/home/wmj");
							strcpy(t.buf,"/home/wmj");
    			       		send(pcur->new_fd,&t,t.len+4,0);
    			        }else if(!strcmp(buf,"ls")||!strcmp(buf,"ls "))
						{
							sendls(pcur->new_fd,pcur->path);
						}else if(strcmp(buf,"pwd")==0)
						{
							t.len=strlen(pcur->path);
							strcpy(t.buf,pcur->path);
							send(pcur->new_fd,&t,t.len+4,0);
						}else{
						//	printf("95:byebye\n");
							break;
						}
					}else if(recvlen>3)
    					{
						if(strncmp(buf,"cd ",3)==0)
						{
							send_cd(buf,&pcur);
						}
						else if(strncmp(buf,"ls ",3)==0)
						{
							ls_any(pcur->new_fd,pcur->path,buf);
						}
						else if(!strncmp(buf,"puts ",5))
						{
							recv_file(pcur->new_fd,pcur->path);
						}
						else if(strncmp(buf,"gets ",5)==0)
						{
							send_file(pcur->new_fd,pcur->path,buf);
						}
						else if(strncmp(buf,"remove ",7)==0)
						{
							rmret=rm_file(pcur->path,buf);
							send(pcur->new_fd,&rmret,sizeof(int),0);
						}
						else{
//							printf("124:else\n");
						}
					}else{
						continue;
					}
					recvlen=0;
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
