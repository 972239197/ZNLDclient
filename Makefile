#CC=gcc
CC=mipsel-openwrt-linux-gcc

DIR_INC = ./include

DIR_SRC = ./src

DIR_OBJ = ./obj

DIR_BIN = ./bin

SRC = $(wildcard ${DIR_SRC}/*.c)

OBJ = $(patsubst %.c,${DIR_OBJ}/%.o,$(notdir ${SRC}))

TARGET = mqtt_client

BIN_TARGET = ${DIR_BIN}/${TARGET}

CFLAGS = -g -O -Wall -I${DIR_INC}

LDLIB = -lpthread -ljson-c -ldl -lm

${BIN_TARGET}:${OBJ}
	$(CC) $(OBJ) $(LDLIB) -o $@

${DIR_OBJ}/%.o:${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -f $(shell find -name "*.o")
	rm -f $(BIN_TARGET)


