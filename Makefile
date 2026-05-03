SRC_DIR := src
TARGET ?= ducc
BUILD_ROOT_DIR := build
BUILD_DIR := $(BUILD_ROOT_DIR)/.$(TARGET)

OBJECTS := \
	$(BUILD_DIR)/cc1/ast.o \
	$(BUILD_DIR)/cc1/codegen.o \
	$(BUILD_DIR)/cc1/codegen_wasm.o \
	$(BUILD_DIR)/cc1/fs.o \
	$(BUILD_DIR)/cc1/io.o \
	$(BUILD_DIR)/cc1/parse.o \
	$(BUILD_DIR)/cc1/preprocess.o \
	$(BUILD_DIR)/cc1/sys.o \
	$(BUILD_DIR)/cc1/token.o \
	$(BUILD_DIR)/cc1/tokenize.o \
	$(BUILD_DIR)/ducc/cli.o \
	$(BUILD_DIR)/ducc/main.o \
	$(BUILD_DIR)/lib/common.o \
	$(BUILD_DIR)/lib/json.o

.PHONY: all
all: $(BUILD_DIR) $(BUILD_ROOT_DIR)/$(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/cc1
	@mkdir -p $(BUILD_DIR)/ducc
	@mkdir -p $(BUILD_DIR)/lib

# TODO: provide release build?
# TODO: use -std=c23 instead of -std=gnu23
$(BUILD_ROOT_DIR)/$(TARGET): $(OBJECTS)
	$(CC) -Wall -Wextra -MMD -g -O0 -std=gnu23 -o $@ $^

# TODO: provide release build?
# TODO: use -std=c23 instead of -std=gnu23
$(BUILD_DIR)/%.o: src/%.c
	$(CC) -c $(CFLAGS) -Wall -Wextra -MMD -g -O0 -std=gnu23 -o $@ $<

-include $(BUILD_DIR)/*.d
