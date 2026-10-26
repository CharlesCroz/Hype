##################
# general options

TARGET = boot


###########################

### OBJS
SRC := ./src
OBJ := ./obj
INC := ./include
_dummy := $(shell mkdir -p $(OBJ))

C_HDRS := $(wildcard $(INC)/*.h)

C_SRCS := $(wildcard $(SRC)/*.c)
C_OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(C_SRCS))

S_SRCS := $(wildcard $(SRC)/*.S)
S_OBJS := $(patsubst $(SRC)/%.S, $(OBJ)/%.o, $(S_SRCS))

TARGET := $(OBJ)/$(TARGET)

# compiler and linker flags
COMMON_FLAGS := -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
export CFLAGS  = $(COMMON_FLAGS) -Wall -O2 -fmessage-length=0 -I$(INC)
export LDFLAGS = $(COMMON_FLAGS) -Wl,-build-id=none -specs=Xilinx.spec -Wl,-T -Wl,lscript.ld -Lobj

#################
all: $(TARGET).elf

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(OBJ)/%.o:$(SRC)/%.S
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(TARGET).elf: $(C_OBJS) $(S_OBJS)
	arm-none-eabi-gcc $(LDFLAGS) -o $@ $(C_OBJS) $(S_OBJS) -Wl,--start-group,-lgcc,-lc,--end-group

clean:
	rm -f **/*.o *~ **/*.elf