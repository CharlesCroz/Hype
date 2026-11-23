##################

DIRS := $(shell ls -d */)

all: FORCE
	@for d in $(DIRS); do  \
        if [ -e $${d}/Makefile ]; then make -C $${d} ; fi \
    done
	python3 ./package_partitions.py ./

clean:
	@for d in $(DIRS); do  \
        if [ -e $${d}/Makefile ]; then make -C $${d} clean ; fi \
    done
	rm -f ./partitions.bin

FORCE:

