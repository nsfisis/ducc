SRC_DIR := src
TARGET ?= ducc
BUILD_ROOT_DIR := build
BUILD_DIR := $(BUILD_ROOT_DIR)/.$(TARGET)

OBJECTS := \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/ast.o \
	$(BUILD_DIR)/cli.o \
	$(BUILD_DIR)/codegen.o \
	$(BUILD_DIR)/common.o \
	$(BUILD_DIR)/fs.o \
	$(BUILD_DIR)/io.o \
	$(BUILD_DIR)/json.o \
	$(BUILD_DIR)/parse.o \
	$(BUILD_DIR)/preprocess.o \
	$(BUILD_DIR)/sys.o \
	$(BUILD_DIR)/token.o \
	$(BUILD_DIR)/tokenize.o

.PHONY: all
all: $(BUILD_DIR) $(BUILD_ROOT_DIR)/$(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# TODO: provide release build?
$(BUILD_ROOT_DIR)/$(TARGET): $(OBJECTS)
	$(CC) -Wall -MMD -g -O0 --std=c23 -o $@ $^

# TODO: provide release build?
$(BUILD_DIR)/%.o: src/%.c
	$(CC) -c $(CFLAGS) -Wall -MMD -g -O0 --std=c23 -o $@ $<

-include $(BUILD_DIR)/*.d
