#include "factory.h"

int send_cd(char* buf,pNode_t* pcur)
{
	int plen,flag=0;
	char addpath[128]={0};
	train t;
    strncpy(addpath,buf+3,strlen(buf)-3);
	if(!strcmp(addpath,".."))
	{
		plen=strlen((*pcur)->path);
		while((*pcur)->path[plen---1]!='/');
		memset(addpath,0,sizeof(addpath));
		strncpy(addpath,(*pcur)->path,plen);
		strcpy((*pcur)->path,addpath);
	}else{
		plen=strlen(addpath);
		int i=plen,j=0;
		char pname[32]={0};
		while(addpath[--i]=='/');
		plen=i+1;
		i=0;
		printf("plen=%d\n",plen);
		addpath[plen]='/';
		plen++;
		while(i<plen)
		{
			if(addpath[i]=='/'){
				printf("pname=%s\n",pname);
		//		pname[j++]='\0';
				DIR *dir;
				dir=opendir((*pcur)->path);
				struct dirent *p;
				if(NULL==dir)
				{
					printf("opendir error\n");
					return -1;
				}
				while((p=readdir(dir))!=NULL)
				{
				printf("p->d_name=%s\n",p->d_name);
				printf("pname=%s\n",pname);
					if(!strcmp(p->d_name,pname)&&p->d_type==4)
					{
						sprintf((*pcur)->path,"%s/%s",(*pcur)->path,pname);
						flag=1;
						break;
					}
				}
				if(flag==0){
					return -1;
				}
				memset(pname,0,sizeof(pname));
				j=0;
				i++;
			}else{
				pname[j++]=addpath[i++];
			}
		}
	//	sprintf((*pcur)->path,"%s/%s",(*pcur)->path,addpath);
	//	strcpy((*pcur)->path+strlen((*pcur)->path),addpath);
	}
//	if((*pcur)->path[plen-1]=='/'){
//		plen--;
//	}
	printf("path=%s\n",(*pcur)->path);
	t.len=strlen((*pcur)->path);
    strcpy(t.buf,(*pcur)->path);
    int ret=send((*pcur)->new_fd,&t,t.len+4,0);
	if(-1==ret)
	{
		perror("send");
		return -1;
	}
	return 0;
}
