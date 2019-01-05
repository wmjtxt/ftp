#include "factory.h"

void cmd_syslog(char *cmd,time_t t)
{
	struct tm *p;
	p=localtime(&t);
	syslog(LOG_NOTICE,"CMD cmdname:%s,time:%04d-%02d-%02d %02d:%02d\n",cmd,p->tm_year+1900,p->tm_mon+1,p->tm_mday,p->tm_hour,p->tm_min);
	return;
}
