#include "factory.h"

int ls_any(int new_fd,char* path1,char* buf)
{
	char path[128]={0};
	strcpy(path,path1);
	int ret=sendls(new_fd,buf);
	if(-1==ret)
	{
		char buf1[128]={0};
		train t;
	    strncpy(buf1,buf+3,strlen(buf)-3);
		if(!strcmp(buf1,".."))
		{
			int plen;
			plen=strlen(path);
			if(path[plen-1]=='/'){
				plen--;
			}
			while(path[plen-1]!='/')
			{
				plen--;
			}
			plen=plen-1;
			memset(buf1,0,sizeof(buf1));
			strncpy(buf1,path,plen);
			strcpy(path,buf1);
		}else{
			sprintf(path,"%s/%s",path,buf1);
		//	strcpy((*path+strlen((*path),buf1);
		}
		t.len=strlen(path);
	    strcpy(t.buf,path);
		int ret=sendls(new_fd,path);
		if(-1==ret)
		{
			perror("send");
			return -1;
		}
	}
	return 0;
}
