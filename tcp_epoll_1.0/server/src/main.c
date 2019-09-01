#include "../include/ftp.h"

void* thread(void* p)
{
	pfac pf=(pfac)p;
	pque pq=&pf->queue;
	pNode_t pcur;
	int ret;
    int quitflag = 0;
	while(1)
	{
        quitflag = 0;
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
                	recvlen=0;
                	//初始化buf
                	memset(buf,0,sizeof(buf));
                	//接收命令
                	recv(pcur->new_fd,&recvlen,sizeof(int),0);
                	int cmdret = recv(pcur->new_fd,buf,recvlen,0);
                	//命令log
                	cmd_syslog(buf,time(NULL));
                	//printf("recvlen=%d\n",recvlen);
                    quitflag = handle(pcur, buf, recvlen);
                	printf("cmd = %s\n",buf);
				}
			}
            printf("quitflag = %d\n",quitflag);
            if(quitflag == 1)
                break;
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
        printf("__LINE__ pid = %d\n",f.pth[0]);
	}
	close(new_fd);
	close(sfd);
}
