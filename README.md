FTP Server based on C
====
# 1.简介
* 本项目使用socket网络编程中的TCP/IP协议进行客户端和服务器通信
* 使用线程池和select来实现多客户端同时链接
* 使用了Makefile功能
# 2.项目功能:
* 常用命令（ls,ls dir,cd,cd dir,pwd,puts file,gets file,remove file,quit）
* 文件上传下载进度条显示、断点续传
# 3.todo
* 用户注册登录及密码验证
* 大文件传输(mmap,未实现)
# 4.运行方法
```
	cd ftp
	make
	./server ip port threadnum capanum
	./client ip port
```
