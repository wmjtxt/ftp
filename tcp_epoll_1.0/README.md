FTP(one thread per connection)
====


# 1.基本信息

* 编程语言：C
* 运行环境：Linux Deepin
* socket、epoll、多线程
* 模式：一个线程处理一个连接

# 2.项目功能:

* 命令(ls,ls dir,cd,cd dir,pwd,puts file,gets file,remove file,quit)
* others: 进度条显示、断点续传

|命令|含义|备注|
|-|-|-|
|ls|显示当前路径所有文件详细信息||
|ls dir|显示dir路径下所有文件详细信息||
|cd|进入根目录(即server所在路径)||
|cd dir|进入dir文件夹||
|pwd|显示当前文件路径||
|gets file|下载file(从server所在路径下载到client所在路径)||
|puts file|上传file(从client所在路径上传到server所在路径)||
|remove file|删除file(删除server路径下的file文件)||
|quit/exit|退出||
|help/h|显示help信息||

# 3.目录结构

- client
- server
    - include
    - src
- Makefile

# 4.运行方法

```sh
$ make
$ cd server/src
$ ./server ip port threadnum capanum
$ cd client
$ ./client ip port
#server所在路径即服务器根目录, client所在路径代表客户端本地目录
```
