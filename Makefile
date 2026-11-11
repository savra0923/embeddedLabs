CC = gcc
CFLAGS = -Wall -Wextra -g
LDLIBS = -lpthread -lrt

all: mem_sensor

mem_sensor: src/mem_sensor.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

src/mem_sensor.o: src/mem_sensor.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o mem_sensor

.PHONY: all clean