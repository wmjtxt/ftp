#include "factory.h"

int filename(char* buf)
{
	char fname[128]={0};
	printf("buf1=%s\n",buf);
	memset(fname,0,sizeof(fname));
	strncpy(fname,(buf)+7,strlen(buf)-7);
	printf("fname=%s\n",fname);
	strcpy(buf,fname);
	return 0;
}
