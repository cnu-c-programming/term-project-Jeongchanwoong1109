CC     = gcc
CFLAGS = -Wall -Wextra -g
SRCS   = main.c student.c file_io.c command.c
HDRS   = student.h file_io.h command.h

.PHONY: all admin client clean

all: admin client

admin: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -DADMIN_MODE $(SRCS) -o admin_shell

client: $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -DCLIENT_MODE $(SRCS) -o client_shell

clean:
	rm -f admin_shell client_shell *.o
