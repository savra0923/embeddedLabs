CC = gcc
CFLAGS = -Wall -Wextra -g  -Iinc
LDLIBS = -lpthread -lrt

BIN_DIR = bin

# All executables
all: $(BIN_DIR)/cpu_sensor \
     $(BIN_DIR)/mem_sensor \
     $(BIN_DIR)/proc_sensor \
     $(BIN_DIR)/sysmond

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build the three sensor executables
$(BIN_DIR)/cpu_sensor: src/cpu_sensor.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(BIN_DIR)/mem_sensor: src/mem_sensor.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@

$(BIN_DIR)/proc_sensor: src/proc_sensor.o | $(BIN_DIR)
	$(CC) $^ $(LDLIBS) -o $@

# sysmond depends on all three sensor executables
$(BIN_DIR)/sysmond: src/sysmond.o \
                    $(BIN_DIR)/cpu_sensor \
                    $(BIN_DIR)/mem_sensor \
                    $(BIN_DIR)/proc_sensor | $(BIN_DIR)
	$(CC) src/sysmond.o $(LDLIBS) -o $@

%: src/%.o
	$(CC) $^ $(LDLIBS) -o $@

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o *

.PHONY: clean