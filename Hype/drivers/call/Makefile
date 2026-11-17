##################
# general options
TARGET = call

# Export
EXPORT_DIR ?= ../export/partitions/
EXPORT_LIB = $(EXPORT_DIR)/obj/
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

all: $(C_OBJS) $(S_OBJS)
	cp $(OBJ)/*.o $(EXPORT_LIB)/
	cp $(INC)/*.h $(EXPORT_INC)/

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

clean:
	rm -rf $(OBJ) 
