#include "factory.h"

void login_syslog(char *usrname,time_t t)
{
	printf("username:%s,",usrname);
	struct tm *p;
	p=localtime(&t);
	syslog(LOG_NOTICE,"LOGIN  username:%s,time:%04d-%02d-%02d %02d:%02d\n",usrname,p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min);
	return;
}
