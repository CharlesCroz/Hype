all:
	make -C kernel/drivers
	make -C kernel/autogen
	make -C kernel/boot

autogen:
	python3 conftool/process_config.py conftool/configs/platform.yml conftool/configs/sample-conf.yml conftool/templates/ kernel/autogen/

clean:
	make -C kernel/autogen clean
	make -C kernel/drivers clean
	make -C kernel/boot clean
