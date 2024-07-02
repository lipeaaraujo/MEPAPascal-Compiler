# compiler name
CC = gcc

# flags
CFLAGS = -Wall -g

# executable name
TARGET = compiler

# sources
SRCS = lexer.c parser.c generator.c compiler.c

# obj files
OBJS = $(SRCS:.c=.o)

all: $(TARGET) clean_objs

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# cleaning compiled files
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean

# cleaning object files after compilation
clean_objs:
	rm -f $(OBJS)