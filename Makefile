CC = gcc
LD = ld
SRC_DIR = src/
BUILD_DIR = build/
TARGET = kernel.elf

CFLAGS = -ffreestanding -m32 -O2 -nostdlib -g -O0 -fno-omit-frame-pointer
LDFLAGS = -m elf_i386 -T linker.ld

MAIN_FILES = main.c start.c
SRC_DIRS = $(shell find $(SRC_DIR) -name '*.c')
SOURCES = $(MAIN_FILES) $(SRC_DIRS)

OBJECTS = $(patsubst %.c,$(BUILD_DIR)%.o,$(SOURCES))

HEADER_DIRS = $(shell find include -type d)
INCLUDE_FLAGS = $(addprefix -I, $(HEADER_DIRS))

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(TARGET) -Map kernel.map

# Generic rule to compile any .c file from the root directory
$(BUILD_DIR)%.o: %.c
	@mkdir -p $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@

$(BUILD_DIR)%.o: $(SRC_DIR)%.c
	@mkdir -p $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE_FLAGS) -c $< -o $@
