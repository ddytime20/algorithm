CC = gcc
RM = rm -rf
TARGET = ac_main
C_SRC = $(wildcard *.c)
C_OBJ = ${patsubst %.c, %.o, $(C_SRC)}
C_FLAGS = -Wall -O -g 
LDFLAGS = -DDEBUG

$(TARGET): $(C_OBJ)
	$(CC) $^ $(LDFLAGS) $(LDFLAGS) -o $@

%.o:%.c
	$(CC) $(C_FLAGS) $(LDFLAGS) -o $@ -c $^

clean:
	$(RM) $(C_OBJ) $(TARGET)


