###########################

all: $(TARGETS)
	make -C kernel
	make -C partitions

clean:
	make -C kernel clean
	make -C partitions clean


veryclean: clean
	rm -rf kernel/src/*.c partitions/src/*.c kernel/include/*.h partitions/include/*.h
