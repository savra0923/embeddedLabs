CC ?= gcc
CFLAGS = -Wall -Wextra -g -Iinc
LDLIBS = -lpthread -lrt

BIN_DIR = bin

all: $(BIN_DIR)/cpu_sensor \
     $(BIN_DIR)/mem_sensor \
     $(BIN_DIR)/proc_sensor \
     $(BIN_DIR)/sysmond

$(BIN_DIR):
	mkdir -p $@

$(BIN_DIR)/cpu_sensor: src/cpu_sensor.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@
	rm -f src/cpu_sensor.o

$(BIN_DIR)/mem_sensor: src/mem_sensor.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@
	rm -f src/mem_sensor.o

$(BIN_DIR)/proc_sensor: src/proc_sensor.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@
	rm -f src/proc_sensor.o

$(BIN_DIR)/sysmond: src/sysmond.o \
                    $(BIN_DIR)/cpu_sensor \
                    $(BIN_DIR)/mem_sensor \
                    $(BIN_DIR)/proc_sensor | $(BIN_DIR)
	$(CC) src/sysmond.o $(LDLIBS) -o $@
	rm -f src/sysmond.o

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o
	rm -rf $(BIN_DIR)

.PHONY: all clean