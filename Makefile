CC ?= gcc
CFLAGS = -Wall -Wextra -g -Iinc
LDLIBS = -lpthread -lrt

TARGET = bin/sysmond

SRCS = src/sysmond.c \
       src/mem_sensor.c \
	   src/cpu_sensor.c \
	   src/proc_sensor.c

OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)
	rm -f $(OBJS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
