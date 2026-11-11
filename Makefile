CC = gcc
CFLAGS = -Wall -Wextra -g  -Iinc
LDLIBS = -lpthread -lrt

%: src/%.o
	$(CC) $^ $(LDLIBS) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o *

.PHONY: clean