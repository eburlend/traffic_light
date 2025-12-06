CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -Iinclude
LDFLAGS ?=

SRC := src/main.c src/traffic.c src/hal-api.c
BIN := traffic_light.out

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(LDFLAGS)

run: all
	@echo "Running program (emulated mode, no sudo needed)..."
	./${BIN}

clean:
	rm -f $(BIN)

.PHONY: all run clean
