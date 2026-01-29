CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
TARGET = patient_data_logger
SRCS = patient_data_logger.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

install: $(TARGET)
	sudo install -m 755 $(TARGET) /usr/local/bin/

.PHONY: all clean run install
