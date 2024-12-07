# Define variables
SRC_DIR = ../../src
BUILD_DIR = ../../build
TARGET = $(BUILD_DIR)/test_preprocesing

# Default target (runs with "make")
$(TARGET): $(BUILD_DIR)
	gcc -Wall -Wextra -O2 -I$(SRC_DIR) $(shell find $(SRC_DIR) -type f -name '*.c') main.c -o $(TARGET)

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
