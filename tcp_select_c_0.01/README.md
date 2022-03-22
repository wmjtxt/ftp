FTP(select)
====

## 1. 基本信息

* 编程语言：C
* 运行环境：Linux Deepin 15.11
* socket、select、多连接
* 模式：多连接

## 2. 项目功能

* 支持10个客户端连接，超过10个后不再接受客户端连接。
* 给服务器发消息。
* 服务器给客户端发消息。

## 3. 目录结构

* server.c
* client.c
* server
* test
    * client
* Makefile

## 4. 运行办法

```sh
$ make
$ ./server
$ cd test/
$ ./client
# 可以打开多个目录，同时跑10个./client
```
