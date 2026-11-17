all:
	make -C drivers
	make -C autogen
	make -C kernel

autogen: FORCE
	python3 conftool/process_config.py conftool/configs/platform.yml conftool/configs/sample-conf.yml conftool/templates/ ./autogen/

clean:
	make -C autogen clean
	make -C drivers clean
	make -C kernel clean

FORCE:

