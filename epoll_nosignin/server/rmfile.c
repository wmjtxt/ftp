#include "factory.h"

int rm_file(char *path,char *buf)
{
	char pname[32]={0};
	strncpy(pname,buf+7,strlen(buf)-7);
	DIR *dir;
	dir=opendir(path);
	int dfd=dirfd(dir);
	if(-1==unlinkat(dfd,pname,0))
	{
		return -1;
	}else{
		return 0;
	}
}
