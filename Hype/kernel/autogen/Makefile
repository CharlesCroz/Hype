###########################

### Kernel includes
K_INC := ../boot/include
D_INC := ../drivers/export/include

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

TARGETS := $(S_OBJS) $(C_OBJS)

# compiler and linker flags
COMMON_FLAGS := -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard
export CFLAGS = $(COMMON_FLAGS) -Wall -O2 -fmessage-length=0 -I$(INC) -I$(K_INC) -I$(D_INC)

#################
all: $(TARGETS)

$(OBJ)/%.o:$(SRC)/%.c
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

$(OBJ)/%.o:$(SRC)/%.S
	arm-none-eabi-gcc -c $(CFLAGS) -o $@ $<

clean:
	rm -f **/*.o *~

veryclean: clean
	rm -rf $(SRC)/*.c $(SRC)/*.S $(INC)/*.h $(INC)/*.ld $(OBJ)
