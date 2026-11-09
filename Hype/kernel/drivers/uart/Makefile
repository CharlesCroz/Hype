##################
# general options
TARGET = uart
EXPORT_PUBLIC = uart.h
EXPORT_KERNEL = uart.a __uart.h

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

TARGET := $(OBJ)/$(TARGET)

## compiler and linker flags
COMMON_FLAGS := -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
export CFLAGS  = $(COMMON_FLAGS) -Wall -O2 -fmessage-length=0 -I$(INC)
export LDFLAGS = $(COMMON_FLAGS) -Lobj

#################
## Rules

all: $(TARGET).a

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(OBJ)/%.o:$(SRC)/%.S
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(TARGET).a: $(C_OBJS) $(S_OBJS)
	arm-none-eabi-ar rcs $@ $(C_OBJS) $(S_OBJS)

clean:
	rm -rf $(OBJ)