#include "func.h"
#define BUFFER_SIZE 1024
#define SERVER_PORT 33333 //端口

int main(int argc, char **argv)
{
    //判定参数输入是否有错
	//if(argc != 3)
	//{
	//	printf("error args\n");
	//	return -1;
	//}
	//ser.sin_port = htons(atoi(argv[2]));
	//ser.sin_addr.s_addr = inet_addr(argv[1]);
    //
	struct sockaddr_in ser;
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_port = htons(SERVER_PORT);
	ser.sin_addr.s_addr = inet_addr("127.0.0.1");

    //Socket
	int sfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sfd)
	{
		perror("socket error");
		return -1;
	}

    char recv_msg[BUFFER_SIZE];
    char input_msg[BUFFER_SIZE];
	int ret;
	ret = connect(sfd, (struct sockaddr*)&ser, sizeof(struct sockaddr_in));
	if(-1 == ret)
	{
		perror("connect error");
		return -1;
	}
    //printf("连接成功\n");
	fd_set cli_fd_set;
    struct timeval tv;
	while(1)
	{
        tv.tv_sec  =  20;
        tv.tv_usec  =  0;
		FD_ZERO(&cli_fd_set);
		FD_SET(STDIN_FILENO, &cli_fd_set);
		FD_SET(sfd, &cli_fd_set);
		ret = select(sfd + 1, &cli_fd_set, NULL, NULL, &tv);
		if(ret > 0)
		{
			if(FD_ISSET(STDIN_FILENO, &cli_fd_set))
			{
                bzero(input_msg, BUFFER_SIZE);
				ret = read(0, input_msg, BUFFER_SIZE);
				if(ret <= 0)
				{
					printf("54byebye\n");
					break;
				}
				ret = send(sfd, input_msg, BUFFER_SIZE, 0);
				if(-1 == ret)
				{
					perror("send error");
					return -1;
				}
			}
			if(FD_ISSET(sfd, &cli_fd_set))
			{
				memset(recv_msg, 0, BUFFER_SIZE);
				long byte_num = recv(sfd, recv_msg, BUFFER_SIZE, 0);
                if(byte_num > 0){
                    if(byte_num > BUFFER_SIZE)
                    {
                        byte_num = BUFFER_SIZE;
                    }
                    recv_msg[byte_num] = '\0';
                    if(recv_msg[0] - '0' >= 0 && recv_msg[0] - '0' <= 9)
                    {
                        printf("连接成功, id = ");
                    }
				    printf("%s\n", recv_msg);
                    if(strcmp(recv_msg,"full") == 0)
                    {
                        printf("服务器加入的客户端已达到最大值，无法加入!\n");
                        exit(0);
                    }
                }
				else if(-1 == byte_num)
				{
					perror("recv");
				}
                else{
					printf("服务器退出\n");
                    exit(0);
				}
			}
		}
	}
	close(sfd);
    return 0;
}
