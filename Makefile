SRC_DIR := src
BUILD_DIR := build
TARGET ?= ducc

.PHONY: all
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): main.c
	$(CC) -MD -g -O0 -o $@ $<

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

-include $(BUILD_DIR)/*.d
