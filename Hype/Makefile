all:
	python3 conftool/process_config.py conftool/configs/platform.yml conftool/configs/sample-conf.yml conftool/templates/ kernel/autogen/
	make -C kernel/drivers
	make -C kernel/autogen
	make -C kernel/boot

clean:
	make -C kernel/autogen clean
	make -C kernel/drivers clean
	make -C kernel/boot clean