##################
# general options
TARGET = uart

# Export
EXPORT_DIR ?= ../export/
EXPORT_LIB = $(EXPORT_DIR)/lib/
EXPORT_INC = $(EXPORT_DIR)/include/


###########################
## Kernel objects
KERNEL_INC = ../../boot/include
AUTOGEN_INC = ../../autogen/include

###########################
## Local objects

### Folders
SRC := ./src
OBJ := ./obj
INC := ./include
_dummy := $(shell mkdir -p $(OBJ))
_dummy := $(shell mkdir -p $(EXPORT_LIB))
_dummy := $(shell mkdir -p $(EXPORT_INC))

## Sources
C_HDRS := $(wildcard $(INC)/*.h)

C_SRCS := $(wildcard $(SRC)/*.c)
C_OBJS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(C_SRCS))

S_SRCS := $(wildcard $(SRC)/*.S)
S_OBJS := $(patsubst $(SRC)/%.S, $(OBJ)/%.o, $(S_SRCS))


## compiler and linker flags
COMMON_FLAGS := -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
export CFLAGS  = $(COMMON_FLAGS) -Wall -O2 -fmessage-length=0 -I$(INC) -I$(AUTOGEN_INC) -I$(KERNEL_INC)
export LDFLAGS = $(COMMON_FLAGS) -Lobj

#################
## Rules

all: $(OBJ)/$(TARGET).a

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(OBJ)/%.o:$(SRC)/%.S
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(OBJ)/$(TARGET).a: $(C_OBJS) $(S_OBJS)
	arm-none-eabi-ar rcs $@ $(C_OBJS) $(S_OBJS)
	cp $(OBJ)/$(TARGET).a $(EXPORT_LIB)/$(TARGET).a
	cp $(INC)/$(TARGET).h $(EXPORT_INC)/$(TARGET).h

clean:
	rm -rf $(OBJ) 