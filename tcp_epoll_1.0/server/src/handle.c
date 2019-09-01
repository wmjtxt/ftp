 ///
 /// @file    :handle.c
 /// @author  :wmjtxt(972213032@qq.com)
 /// @date    :2019-03-11 09:13:40
 /// @quote   :
 ///
 
#include "../include/ftp.h"

int handle(pNode_t pcur, char *buf, int recvlen){
    printf("handle\n");
	train t;
    memset(&t,0,sizeof(t));
    int res = 0;
    int cmdflag = 1;
	if(recvlen==0){//输入ctrl+c和直接输入回车,recvlen都是0,怎么区分开?
		int byeflag = 0;
		recv(pcur->new_fd,&byeflag,sizeof(int),0);
		//printf("byeflag=%d\n",byeflag);
		if(byeflag){
			cmdflag = 0;
			printf("用户输入回车\n");
		}
		else{
			printf("byebye(用户退出,ctrl+c)\n");
            res =  1;
		}
	}else if(recvlen<=3){
		if(!strcmp(buf,"cd")||!strcmp(buf,"cd "))
		{
			t.len=1;
			strcpy(pcur->path,pcur->rpath);
			strcpy(t.buf,"/");
            int cdflag = 0;
            send(pcur->new_fd,&cdflag,sizeof(int),0);
       		send(pcur->new_fd,&t,t.len+4,0);
			printf("cd success\n");
        }else if(!strcmp(buf,"ls")||!strcmp(buf,"ls "))
		{
			int lsret = sendls(pcur->new_fd,pcur->path);
			if(lsret == -1){
				printf("ls failed\n");
			}else{
				printf("ls success\n");
			}
		}else if(strcmp(buf,"pwd")==0)
		{
            int pwdflag = 0;
            send(pcur->new_fd,&pwdflag,sizeof(int),0);
			t.len=strlen(pcur->path)-strlen(pcur->rpath);
			if(t.len == 0){
				strcpy(t.buf,"/");
				t.len++;
			}else{
				strncpy(t.buf,pcur->path+strlen(pcur->rpath),t.len);
            }
			printf("t.len = %d t.buf = %s\n",t.len,t.buf);
			if(-1 != send(pcur->new_fd,&t,t.len+4,0)){
				printf("pwd success\n");
			}else 
				printf("pwd failed\n");
		}else{//输入非法指令
			cmdflag = 0;
			printf("%d\n",__LINE__);
		}
	}else if(recvlen > 3){
		if(strncmp(buf,"cd ",3)==0)//cd ../有bug
		{
			int cdflag = send_cd(buf,pcur);
			if(cdflag == -1){
                send(pcur->new_fd,&cdflag,sizeof(int),0);
				printf("%s failed\n",buf);
			}else{
                send(pcur->new_fd,&cdflag,sizeof(int),0);
                memset(&t,0,sizeof(t));
			    t.len=strlen(pcur->path)-strlen(pcur->rpath);
			    if(t.len == 0){
			    	strcpy(t.buf,"/");
			    	t.len++;
			    }else{
			    	strncpy(t.buf,pcur->path+strlen(pcur->rpath),t.len);
                }
                send(pcur->new_fd,&t,t.len+4,0);
				printf("79%s success\n",buf);
			}
		}
		else if(strncmp(buf,"ls ",3)==0)
		{
			int lsret = ls_any(pcur, buf);
			if(lsret == -1){
				printf("ls failed\n");
			}else{
				printf("ls success\n");
			}
		}
		else if(!strncmp(buf,"puts ",5))
		{
			int recvret = recv_file(pcur->new_fd,pcur->path);
			if(recvret == -1){
				printf("upload failed\n");
			}else{
				printf("upload success\n");
			}
		}
		else if(strncmp(buf,"gets ",5)==0)
		{
			int sendret = send_file(pcur->new_fd,pcur->path,buf);
			if(sendret == -1){
				printf("download failed\n");
			}else{
				printf("download success\n");
			}
		}
		else if(strncmp(buf,"remove ",7)==0)
		{
			int rmret=rm_file(pcur->path,buf);
			if(rmret == -1){
				printf("remove file failed\n");
			}else{
				printf("remove file success\n");
			}
			send(pcur->new_fd,&rmret,sizeof(int),0);
		}
		else if(!strcmp(buf,"quit") || !strcmp(buf,"exit")){
			printf("%d : quit(用户退出)\n",__LINE__);
            res = 1;
		}
		else{
			cmdflag = 0;
			printf("%d输入的指令不合法！\n",__LINE__);
		}
	}
	send(pcur->new_fd,&cmdflag,sizeof(int),0);
	return res;
}
