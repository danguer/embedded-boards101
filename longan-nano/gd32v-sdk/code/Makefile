BUILD_DIR := build
RISCV_ARCH := rv32imac
ifeq ($(OS),Windows_NT)
RISCV_ARCH_CC := rv32imac
else
RISCV_ARCH_CC := rv32imac_zicsr
endif
RISCV_ABI := ilp32
CC := riscv64-unknown-elf-gcc
LD := riscv64-unknown-elf-ld
OBJCOPY := riscv64-unknown-elf-objcopy
CFLAGS := -march=$(RISCV_ARCH_CC) -mabi=$(RISCV_ABI) \
	-Isrc \
	-I$(RISCV_PATH)/include \
	-I$(GD32_LIBRARY)/Firmware/RISCV/drivers \
	-I$(GD32_LIBRARY)/Firmware/GD32VF103_standard_peripheral \
	-I$(GD32_LIBRARY)/Firmware/GD32VF103_standard_peripheral/Include \
	-D HXTAL_VALUE=8000000

SYSTEM_SRC_DIR := $(GD32_LIBRARY)/Firmware/GD32VF103_standard_peripheral
SYSTEM_OBJS := $(BUILD_DIR)/system/system_gd32vf103.o

LIBRARY_SRC_DIR := $(SYSTEM_SRC_DIR)/Source
LIBRARY_SRCS := $(wildcard $(LIBRARY_SRC_DIR)/*.c)
LIBRARY_OBJS := $(addprefix $(BUILD_DIR)/library,$(patsubst %.c,%.o,$(subst $(LIBRARY_SRC_DIR),,$(LIBRARY_SRCS))))

DRIVERS_SRC_DIR := $(GD32_LIBRARY)/Firmware/RISCV/drivers
DRIVERS_OBJS :=  $(BUILD_DIR)/drivers/n200_func.o

STUBS_SRC_DIR := $(GD32_LIBRARY)/Firmware/RISCV/stubs
STUBS_SRCS := $(wildcard $(STUBS_SRC_DIR)/*.c)
STUBS_OBJS := $(addprefix $(BUILD_DIR)/stubs,$(patsubst %.c,%.o,$(subst $(STUBS_SRC_DIR),,$(STUBS_SRCS))))

SRC_OBJS := $(BUILD_DIR)/src/systick.o

AS_SRC_DIR := $(GD32_LIBRARY)/Firmware/RISCV/env_Eclipse
AS_OBJS := $(BUILD_DIR)/as/start.o \
	$(BUILD_DIR)/as/entry.o \
	$(BUILD_DIR)/as/handlers.o

ALL_OBJS := $(SRC_OBJS) \
	$(DRIVERS_OBJS) \
	$(LIBRARY_OBJS) \
	$(SYSTEM_OBJS) \
	$(STUBS_OBJS) \
	$(AS_OBJS) \
	$(BUILD_DIR)/src/main.o

all: prepare blink

prepare:
ifeq ($(OS),Windows_NT)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	if not exist $(BUILD_DIR)\src mkdir $(BUILD_DIR)\src
	if not exist $(BUILD_DIR)\as mkdir $(BUILD_DIR)\as
	if not exist $(BUILD_DIR)\library mkdir $(BUILD_DIR)\library
	if not exist $(BUILD_DIR)\drivers mkdir $(BUILD_DIR)\drivers
	if not exist $(BUILD_DIR)\stubs mkdir $(BUILD_DIR)\stubs
	if not exist $(BUILD_DIR)\system mkdir $(BUILD_DIR)\system
else
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/src
	mkdir -p $(BUILD_DIR)/as
	mkdir -p $(BUILD_DIR)/library
	mkdir -p $(BUILD_DIR)/drivers
	mkdir -p $(BUILD_DIR)/stubs
	mkdir -p $(BUILD_DIR)/system
endif

$(BUILD_DIR)/library/%.o: $(LIBRARY_SRC_DIR)/%.c
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@

$(BUILD_DIR)/drivers/%.o: $(DRIVERS_SRC_DIR)/%.c
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@


$(BUILD_DIR)/stubs/%.o: $(STUBS_SRC_DIR)/%.c
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@

$(BUILD_DIR)/system/%.o: $(SYSTEM_SRC_DIR)/%.c
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@

$(BUILD_DIR)/as/%.o: $(AS_SRC_DIR)/%.S
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@

$(BUILD_DIR)/as/%.o: $(AS_SRC_DIR)/%.c
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@

$(BUILD_DIR)/src/%.o: src/%.c
	$(CC) \
	$(CFLAGS) \
	-c $< \
	-o $@


blink: $(ALL_OBJS)
	$(LD) \
		$(ALL_OBJS) \
		$(RISCV_PATH)/lib/$(RISCV_ARCH)/$(RISCV_ABI)/libc_nano.a \
		-o $(BUILD_DIR)/blink.elf \
		-T $(GD32_LIBRARY)/Firmware/RISCV/env_Eclipse/GD32VF103xB.lds \
		-m elf32lriscv  \
		-Map "$(BUILD_DIR)/blink.map"

	$(OBJCOPY) \
		-O ihex \
		$(BUILD_DIR)/blink.elf \
		$(BUILD_DIR)/blink.hex

	$(OBJCOPY) \
		-O binary \
		$(BUILD_DIR)/blink.elf \
		$(BUILD_DIR)/blink.bin

clean:
ifeq ($(OS),Windows_NT)
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
else
	rm -rf $(BUILD_DIR)
endif
