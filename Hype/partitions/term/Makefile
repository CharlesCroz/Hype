EXPORT_OBJS := $(wildcard ../../drivers/export/partitions/obj/*.o)
EXPORT_OBJS := $(wildcard ../../drivers/export/partitions/obj/*.o)
AUTOGEN_OBJS := $(wildcard ../../autogen/partitions/obj/*.o)
EXPORT_INC := ../../drivers/export/partitions/include
AUTOGEN_INC := ../../autogen/partitions/include

all:
	arm-none-eabi-gcc -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -nostdlib -c -o main.o main.c -Wl,-T -Wl,lscript.ld -I$(EXPORT_INC) -I$(AUTOGEN_INC)
	arm-none-eabi-gcc -fPIC -mcpu=cortex-a9 -mfpu=vfpv3 -mfloat-abi=hard -nostdlib -o prog.elf main.o $(EXPORT_OBJS) $(AUTOGEN_OBJS) -I$(EXPORT_INC) -I$(AUTOGEN_INC) -Wl,-T -Wl,lscript.ld
