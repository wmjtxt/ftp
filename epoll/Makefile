server/src/server:server/src/ftp_server_func.c server/src/ftp_server.c client/ftp_client_func.c client/ftp_client.c
	gcc client/*.c -o client/client -w
	gcc server/src/*.c -o server/src/server -lpthread -lcrypt -w
.PHONY:clean
clean:rm -rc server/src/server client/client
