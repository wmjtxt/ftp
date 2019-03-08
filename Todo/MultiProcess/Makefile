
CC := gcc
CXXFLAGS := -w -g -lpthread -lcrypt
SRC1 := server/src/*.c
SRC2 := client/*.c
SERVER := server/src/server
CLIENT := client/client

$(SERVER) : $(SRC1) $(SRC2)
	$(CC) $(SRC1) -o $(SERVER) $(CXXFLAGS)
	$(CC) $(SRC2) -o $(CLIENT) -w

.PHONY:clean
clean:
	rm -rf $(SERVER)
	rm -rf $(CLIENT)
