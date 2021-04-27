CC = gcc
RM = rm -rf 
TARGET = main
C_SRC = $(wildcard *.c)
C_OBJ = ${patsubst %.c, %.o, $(C_SRC)}
C_FLAGS = -Wall -O -g -O0
LDFLAGS = 

all: $(TARGET)

$(TARGET): $(C_OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

#$(C_OBJ): $(C_SRC)
#	$(CC) $(C_FLAGS)  -c $^ -o $@

#将所有的.c生成对应的.o
%.o: %.c
	$(CC) $(C_FLAGS)  -c $^ -o $@

clean: 
	$(RM) $(C_OBJ) $(TARGET)

