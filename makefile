# Grupo 18 \
Tiago Badalo 55311 \
João Feliciano 52814 \
Miguel Sousa 48109

OBJ_dir = object
SRC_dir = source
INC_dir = include
BIN_dir = binary
LIB_dir = lib
PROTOBUF_dir = /home/aluno-di/SD/protobuf-c/protobuf-c/

OBJETOS = message.o message-private.o sdmessage.pb-c.o inet.o 
OBJETOS_SERVER = table_server.o table_server-private.o network_server.o network_server-private.o table_skel.o table_skel-private.o list-private.o list.o serialization.o table-private.o table.o network_server-private.o  $(OBJETOS)
OBJETOS_CLIENT = table_client.o table_client-private.o  $(OBJETOS)
OBJETOS_LIB = client_stub.o network_client.o data.o entry.o client_stub-private.o stats-private.o

CC = gcc
PTHREAD = -pthread
CFLAGS = -g -Wall $(PTHREAD) -I $(INC_dir) -D THREADED 
PROTOBUFLAG = -lprotobuf-c 
ZOOKEEPERFLAG = -lzookeeper_mt

vpath %.o $(OBJ_dir)

%.o: $(SRC_dir)/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<

$(LIB_dir)/client-lib.o $(BIN_dir)/table-client $(BIN_dir)/table-server : $(OBJETOS_LIB) $(OBJETOS_CLIENT) $(OBJETOS_SERVER) 
	ld -r $(addprefix $(OBJ_dir)/,$(OBJETOS_LIB)) -o $(LIB_dir)/client-lib.o 
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJETOS_CLIENT))  $(LIB_dir)/client-lib.o -I$(PROTOBUF_dir) -L$(PROTOBUF_dir) $(PROTOBUFLAG) -o $(BIN_dir)/table_client $(ZOOKEEPERFLAG) 
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJETOS_SERVER))  $(LIB_dir)/client-lib.o $(PTHREAD) -I$(PROTOBUF_dir) -L$(PROTOBUF_dir) $(PROTOBUFLAG) -o $(BIN_dir)/table_server $(ZOOKEEPERFLAG)
	
clean:
	rm -f $(OBJ_dir)/*.o
	rm -f $(BIN_dir)/*
	rm -f $(LIB_dir)/*
