#include "func.h"
#define CONCURRENT_MAX 10 //应用层可以处理最大连接
#define BACKLOG 5         //已连接套接字个数
#define BUFFER_SIZE 1024  //收发消息最大字节数
#define SERVER_PORT 33333 //端口

int main(int argc, char **argv)
{
    int online_cnt = 0;
    char input_msg[BUFFER_SIZE];
    char send_msg[BUFFER_SIZE];
    char recv_msg[BUFFER_SIZE];
    //if(argc != 3)
    //{
    //    printf("error args\n");
    //    return -1;
    //}
    //ser.sin_port = htons(atoi(argv[2]));
    //ser.sin_addr.s_addr = inet_addr(argv[1]);

    //Socket
    //服务器套接字
    int sfd;
    //客户端文字描述符
    int client_fds[CONCURRENT_MAX] = {0};
    sfd  =  socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == sfd)
    {
        perror("socket");
        return -1;
    } 
    //server
    struct sockaddr_in ser;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(SERVER_PORT);
    ser.sin_addr.s_addr = inet_addr("127.0.0.1");
    bzero(&(ser.sin_zero), 8);

    //bind
    if(-1 == bind(sfd, (struct sockaddr*)&ser, sizeof(struct sockaddr)))
    {
        perror("bind");
        return -1;
    }
    //listen
    if(-1 == listen(sfd, BACKLOG))
    {
        perror("listen");
        return -1;
    }

    fd_set ser_fd_set;
    struct timeval tv;
    int maxfd = -1;
    int ret;
    while(1)
    {
        printf("while(1)\n");
        //阻塞在select函数, 20秒轮询一次, 有请求时, 马上处理, 没有时，20秒循环一次
        tv.tv_sec = 2;
        //
        tv.tv_usec = 0;
        FD_ZERO(&ser_fd_set);
        //STDIN_FILENO
        FD_SET(STDIN_FILENO, &ser_fd_set);
        if(maxfd < STDIN_FILENO){
            maxfd = STDIN_FILENO;
        }
        FD_SET(sfd, &ser_fd_set);
        if(maxfd < sfd){
            maxfd = sfd;
        }
        //printf("%d\n", __LINE__);
        //printf("%d\n", CONCURRENT_MAX);
        //客户端
        for(int i = 0; i < CONCURRENT_MAX; ++i){
            if(client_fds[i] != 0)
            {
                FD_SET(client_fds[i], &ser_fd_set);
                if(maxfd < client_fds[i]){
                    maxfd = client_fds[i];
                }
            }
        }
        //select, 加上&tv到底什么用？还没懂。不加的话，有请求立即响应，加上也是一样，只不过20秒返回ret一个0，然后打印超时，再回到select继续阻塞。没懂
        ret = select(maxfd + 1, &ser_fd_set, NULL, NULL, NULL);
        //printf("%d : ret = %d\n", __LINE__, ret);
        if(ret < 0)
        {
            perror("select 出错");
            continue;
        }
        else if(ret == 0)
        {
            printf("select 超时\n");
            continue;
        }
        else{
            if(FD_ISSET(STDIN_FILENO, &ser_fd_set))
            {
                printf("发送消息: \n");
                memset(input_msg, 0, BUFFER_SIZE);
                memset(send_msg, 0, BUFFER_SIZE);
                ret = read(STDIN_FILENO, input_msg, BUFFER_SIZE);
                if(ret <= 0)
                {
                    printf("byebye\n");
                    break;
                }
                sprintf(send_msg, "[服务器消息]:%s", input_msg);
                for(int i = 0; i < CONCURRENT_MAX; ++i){
                    if(client_fds[i] != 0){
                        printf("client_fds[%d] = %d\n", i, client_fds[i]);
                        if(-1 == send(client_fds[i], send_msg, BUFFER_SIZE, 0))
                        {
                            perror("send");
                        }
                    }
                }
            }
            if(FD_ISSET(sfd, &ser_fd_set))
            {
                //有新连接
                printf("有新连接\n");
                //if(online_cnt >= CONCURRENT_MAX)
                //{
                //    //printf("超过最大连接数\n");
                //    continue;
                //}
                online_cnt++;
                struct sockaddr_in cli;
                socklen_t address_len;
                int cli_sock_fd = accept(sfd, (struct sockaddr*)&cli, &address_len);
                printf("cli_sock_fd = %d\n",cli_sock_fd);
                if(-1 == cli_sock_fd)
                {
                    perror("accept");
                    return -1;
                }
                else{
                    int index = -1;
                    for(int i = 0; i < CONCURRENT_MAX; ++i){
                        if(client_fds[i] == 0)
                        {
                            index = i;
                            client_fds[i] = cli_sock_fd;
                            break;
                        }
                    }
                    if(index >= 0){
                        printf("新客户端(%d)加入成功 %s:%d\n", index, inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
                        bzero(input_msg, BUFFER_SIZE);
                        input_msg[0] = index+'0';
                        send(cli_sock_fd, input_msg, BUFFER_SIZE, 0);
                    }
                    else{
                        printf("客户端连接数达到最大值，新客户加入失败 %s:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
                        bzero(input_msg, BUFFER_SIZE);
                        //strcpy(input_msg, "服务器加入的客户端数已达到最大值，无法加入!\n");
                        strcpy(input_msg, "full");
                        send(cli_sock_fd, input_msg, BUFFER_SIZE, 0);
                    }
                }
            }
            for(int i = 0; i < CONCURRENT_MAX; ++i)
            {
                if(client_fds[i] != 0){
                    if(FD_ISSET(client_fds[i], &ser_fd_set)){
                        bzero(recv_msg, BUFFER_SIZE);
                        int byte_num = recv(client_fds[i], recv_msg, BUFFER_SIZE, 0);
                        if(byte_num > 0)
                        {
                            if(byte_num > BUFFER_SIZE)
                            {
                                byte_num = BUFFER_SIZE;
                            }
                            recv_msg[byte_num] = '\0';
                            printf("客户端(%d):%s\n", i, recv_msg);
                            bzero(send_msg, BUFFER_SIZE);
                            sprintf(send_msg, "[%d]:%s", i, recv_msg);
                            //strcpy(send_msg, recv_msg);
                            send(client_fds[i], send_msg, BUFFER_SIZE, 0);
                        }
                        else if(byte_num < 0)
                        {
                            printf("从客户端(%d)接受消息出错.\n", i);
                        }
                        else{
                            FD_CLR(client_fds[i], &ser_fd_set);
                            client_fds[i] = 0;
                            printf("客户端(%d)退出了\n", i);
                        }
                    }
                }
            }
        }
    }
    close(sfd);
}
