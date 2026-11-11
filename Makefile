CC ?= gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lpthread -lrt

TARGET = src/mem_sensor

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(TARGET).o
	$(CC) $^ $(LDLIBS) -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean