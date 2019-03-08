FTP Server based on C
====

# 1.简介

* 本项目使用socket网络编程中的TCP/IP协议进行客户端和服务器通信
* 使用多线程和epoll来实现多客户端同时连接
* 使用了Makefile

# 2.项目功能:

* 常用命令（ls,ls dir,cd,cd dir,pwd,puts file,gets file,remove file,quit）
* 文件上传下载进度条显示、断点续传

# 3.目录结构

- MultiThread
    - client
    - server
        - include
        - src
    - Makefile

# 4.运行方法

```
$ cd MultiThread
$ make
$ cd MultiThread/server/src
$ ./server ip port threadnum capanum
$ cd MultiThread/client
$ ./client ip port
```
