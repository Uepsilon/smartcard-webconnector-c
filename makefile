# define the compiler
CC = gcc

# define compiler flags
CFLAGS = -l nfc -l curl

# define the script name
TARGET = webconnector

all: $(TARGET).c
	$(CC) $(TARGET).c $(CFLAGS) -o $(TARGET).out

clean:
	$(RM) $(TARGET).out
