CC := gcc
CFLAGS := -std=c23 -Wall -Wextra -pedantic-errors -O2
TARGET := bin/main
SRC := main.c

.PHONY: all build run test clean

$(TARGET): $(SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -rf bin
