##################
# general options

TARGET = boot


###########################
### Arch
ARCH_LIB := ../arm/lib
ARCH_INC := ../arm/include

### OBJS
SRC := ./src
OBJ := ./obj
INC := ./include
_dummy := $(shell mkdir -p $(OBJ))
C_SRCS := $(wildcard $(SRC)/*.c)
C_HDRS := $(wildcard $(INC)/*.h)
C_OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(C_SRCS))
TARGET := $(OBJ)/$(TARGET)

# compiler and linker flags
COMMON_FLAGS := -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
export CFLAGS  = $(COMMON_FLAGS) -Wall -O2 -fmessage-length=0 -I$(INC) -I$(ARCH_INC)
export LDFLAGS = $(COMMON_FLAGS) -Wl,-build-id=none -specs=$(SRC)/Xilinx.spec -Wl,-T -Wl,$(SRC)/lscript.ld -L$(ARCH_LIB)

#################
all: $(TARGET).elf

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) $< -o $@

$(TARGET).elf: $(C_OBJS)
	arm-none-eabi-gcc $(LDFLAGS) -o $@ $(C_OBJS) -Wl,--start-group,-lxil,-lgcc,-lc,--end-group

clean:
	rm -f **/*.o *~ **/*.elf