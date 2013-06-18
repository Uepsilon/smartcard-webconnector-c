# define the compiler
CC = gcc

# define compiler flags
CFLAGS = -lnfc

# define the script name
TARGET = webconnector

all: $(TARGET).c
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).out

clean:
	$(RM) $(TARGET).out
