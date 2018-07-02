基于C语言的FTP服务器
====
# 1.简介
* 本项目使用socket网络编程中的TCP/IP协议进行客户端和服务器通信
* 使用线程池和select来实现多客户端同时链接
* 使用了Makefile功能
# 2.项目功能:
* 用户注册登录及密码验证（未实现）
* 常用命令（ls,ls dir,cd,cd dir,pwd,puts file,gets file,remove file）
* 上传下载进度条
* 下载断点续传
* 上传断点续传(未实现)
* 大文件传输(mmap,未实现)
# 3.实现语言: C
# 4.开发环境: deepin15.6
