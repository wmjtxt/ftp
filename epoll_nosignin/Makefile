#CC = gcc
#SRVDIR = ./server
#CLIDIR = ./client
CFLAGS = -w -lpthread -lcrypt
#EXEC = server client
#
#$(SRVDIR)/server : $(SRVDIR)/*.c
tcp: ./server/*.c ./client/*.c
	gcc ./server/*.c -o ./server/server $(CFLAGS)
	gcc ./client/*.c -o ./client/client $(CFLAGS)
.PHONY:clean
clean:
	rm ./server/server ./client/client
