SRC_DIR := src
BUILD_DIR := build
TARGET ?= ducc

OBJECTS := \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/ast.o \
	$(BUILD_DIR)/cli.o \
	$(BUILD_DIR)/codegen.o \
	$(BUILD_DIR)/common.o \
	$(BUILD_DIR)/io.o \
	$(BUILD_DIR)/parse.o \
	$(BUILD_DIR)/preprocess.o \
	$(BUILD_DIR)/sys.o \
	$(BUILD_DIR)/tokenize.o

.PHONY: all
all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CC) -MD -g -O0 -o $@ $^

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.c
	$(CC) -c -MD -g -O0 -o $@ $<

-include $(BUILD_DIR)/*.d
