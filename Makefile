CC = arm-none-eabi-gcc
HEX = arm-none-eabi-objcopy

RM=rm -rf
BINARY_BASE_NAME=Debug

BASE_DIR = ..
SRC_DIR = Src
BIN_BASE_DIR = Bin
BIN_DIR = $(BIN_BASE_DIR)/$(BINARY_BASE_NAME)
LIB_DIR = $(BASE_DIR)/STM32Cube_FW_F0_V1.4.0
HYBRID_LIB_DIR = $(BASE_DIR)/Common
HYBRID_LIB_SRC =

ELF_FILE = $(BIN_DIR)/$(BINARY_BASE_NAME).elf
BIN_FILE = $(BIN_DIR)/$(BINARY_BASE_NAME).bin
MAP_FILE = $(BIN_DIR)/$(BINARY_BASE_NAME).map

INCLUDE_DIRS= $(LIB_DIR)/Drivers/CMSIS/Include \
			  $(LIB_DIR)/Drivers/CMSIS/Device/ST/STM32F0xx/Include \
			  $(LIB_DIR)/Drivers/STM32F0xx_HAL_Driver/Inc \
			  $(HYBRID_LIB_DIR)/Inc \
			  Inc
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))

DEFINES := "USE_HAL_DRIVER" "STM32F072xB"
DEFINE_FLAGS := $(addprefix -D,$(DEFINES))

LINK_SCRIPT="$(LIB_DIR)/STM32F072RB_FLASH.ld"

LINKER_FLAGS=-lm -mthumb -mcpu=cortex-m0  -Wl,--gc-sections -T$(LINK_SCRIPT) -static  -Wl,--start-group -lm -Wl,--end-group -Wl,-cref "-Wl,-Map=$(MAP_FILE)" -Wl,--defsym=malloc_getpagesize_P=0x1000

COMMON_FLAGS=-c -g -O2 -mcpu=cortex-m0 -std=gnu99 -Wall -mthumb
ASSEMBLER_FLAGS=$(COMMON_FLAGS) -x assembler-with-cpp
COMPILER_FLAGS=$(COMMON_FLAGS) -ffunction-sections -fdata-sections $(DEFINE_FLAGS)

SRC := $(wildcard $(SRC_DIR)/*.c) \
	   $(addprefix $(HYBRID_LIB_DIR)/Src/, $(HYBRID_LIB_SRC)) \
	   $(wildcard $(LIB_DIR)/Drivers/STM32F0xx_HAL_Driver/Src/*.c) \
	   $(LIB_DIR)/Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/system_stm32f0xx.c

SRC := $(filter-out $(LIB_DIR)/Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_msp_template.c,$(SRC))

SRCASM := $(LIB_DIR)/Drivers/CMSIS/Device/ST/STM32F0xx/Source/Templates/gcc/startup_stm32f072xb.s

OBJS := $(SRC:%.c=$(BIN_DIR)/%.o) $(SRCASM:%.s=$(BIN_DIR)/%.o)

all: $(OBJ) $(ELF_FILE) $(BIN_FILE) load

$(ELF_FILE): $(OBJS)
	$(CC) $(OBJS) $(LINKER_FLAGS) -o "$(ELF_FILE)"

$(BIN_FILE): $(ELF_FILE)
	$(HEX) -O binary "$<" "$@"

load: $(BIN_FILE)
	openocd -f interface/stlink-v2-1.cfg -f target/stm32f0x_stlink.cfg -c init -c "reset init" -c halt -c "flash write_image erase $(BIN_FILE) 0x08000000" -c "verify_image $(BIN_FILE)" -c "reset run" -c shutdown

.PHONY: clean
clean:
	$(RM) $(BIN_BASE_DIR)

$(BIN_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_FLAGS) $< -o $@

$(BIN_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASSEMBLER_FLAGS) $(INCLUDE_FLAGS) $< -o $@
	
$(BIN_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	$(CC) $(ASSEMBLER_FLAGS) $(INCLUDE_FLAGS) $< -o $@
