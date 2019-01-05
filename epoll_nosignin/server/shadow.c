#include "factory.h"

void help(void)
{
	printf("用户名密码验证程序\n第一个参数为用户名\n");
	exit(-1);
}

void error_quit(char* msg)
{
	perror(msg);
	exit(-2);
}

void get_salt(char* salt,char* passwd)
{
	int i,j;
	for(i=0,j=0;passwd[i]&&j!=3;++i)
	{
		if(passwd[i]=='$')
			++j;
	}
	strncpy(salt,passwd,i-1);
}

int login(char* pname,char* passwd)
{
	struct spwd* sp;
	char salt[512]={0};
	printf("1\n");
	if((sp=getspnam(pname))==NULL){
		return 0;
	}
	printf("2\n");
	get_salt(salt,sp->sp_pwdp);
	printf("3\n");
	if(strcmp(sp->sp_pwdp,crypt(passwd,salt))==0){
		printf("验证通过\n");
		return 1;
	}else{
		printf("验证失败\n");
		return 0;
	}
}
