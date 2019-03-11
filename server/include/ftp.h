#ifndef __FTP_H__
#define __FTP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <winsock.h>
//#include <winsock2.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <syslog.h>
#include <shadow.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>

#define FILENAME "file"

typedef void* (*threadfunc)(void*);

typedef struct{
	int len;
	char buf[1000];
}train,*ptrain;

typedef struct tag_node{
	int new_fd;
	int sfd;
	char path[128];
	char rpath[128];//根目录
	struct tag_node* pnext;
}Node_t,*pNode_t;

typedef struct{
	pNode_t phead,ptail;
	int capability;
	int size;
	pthread_mutex_t mutex;
}que_t,*pque;

typedef struct{
    pthread_t *pth;
	int pth_num;
	pthread_cond_t cond;
	que_t queue;//放描述符的队列
	threadfunc pfunc;	
	short startflg;
}factory,*pfac;

int handle(pNode_t, char*, int);
#endif
