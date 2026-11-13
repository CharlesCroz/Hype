##################

DIRS := $(shell ls -d */)

all:
	@for d in $(DIRS); do  \
        if [ -e $${d}/Makefile ]; then make -C $${d} ; fi \
    done

clean:
	@for d in $(DIRS); do  \
        if [ -e $${d}/Makefile ]; then make -C $${d} clean ; fi \
    done
	rm -rf export