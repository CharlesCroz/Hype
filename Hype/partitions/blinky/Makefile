###########################
TARGET := blinky

###########################
## Local objects

### Folders
SRC := ./src
OBJ := ./obj
INC := ./include
_dummy := $(shell mkdir -p $(OBJ))

## Sources
C_HDRS := $(wildcard $(INC)/*.h)

C_SRCS := $(wildcard $(SRC)/*.c)
C_OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(C_SRCS))

S_SRCS := $(wildcard $(SRC)/*.S)
S_OBJS := $(patsubst $(SRC)/%.S, $(OBJ)/%.o, $(S_SRCS))

LIBS  := $(wildcard $(OBJ)/*.a)

TARGET := $(OBJ)/$(TARGET)

## compiler and linker flags
COMMON_FLAGS := -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -nostdlib
export CFLAGS  = $(COMMON_FLAGS) -Wall -O2 -fmessage-length=0 -I$(INC) -I$(AUTOGEN_INC) -I$(EXPORT_INC)
export LDFLAGS = $(COMMON_FLAGS) -Wl,-T -Wl,_lscript.ld -Lobj -L$(AUTOGEN_INC) -L$(EXPORT_INC)

###########################
## Hype objects and headers
EXPORT_OBJS := $(wildcard ../../drivers/export/partitions/obj/*.o)
EXPORT_OBJS := $(wildcard ../../drivers/export/partitions/obj/*.o)
AUTOGEN_OBJS := $(wildcard ../../autogen/partitions/obj/*.o)
EXPORT_INC := ../../drivers/export/partitions/include
AUTOGEN_INC := ../../autogen/partitions/include


#################
## Rules

all: $(TARGET).elf

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(OBJ)/%.o:$(SRC)/%.S
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(TARGET).elf: $(C_OBJS) $(S_OBJS)
	arm-none-eabi-gcc $(LDFLAGS) -o $@ $(C_OBJS) $(S_OBJS) $(AUTOGEN_OBJS) $(EXPORT_OBJS) $(LIBS) -Wl,--start-group,-lgcc,-lc,--end-group

clean:
	rm -rf $(OBJ)
