# Connect or invoke hw_server
connect -url tcp:127.0.0.1:3121

# Reset Zynq 7000 (target 1)
targets -set -nocase -filter {name =~"APU*" && level == 0}
rst -system
after 500

# Load bitstream (target 4, the xc7z020 weirdo at the end)
targets -set -nocase -filter {name !~"APU*" && level == 0}
fpga -file ./HW/Vitis/Kernel/_ide/bitstream/design_1_wrapper.bit

# Set things related to PL memory mapping ? Todo : research more
targets -set -nocase -filter {name =~"APU*" && level == 0}
loadhw -hw ./HW/Vitis/Zybo_GPIO_led_sws_INT_btn/export/Zybo_GPIO_led_sws_INT_btn/hw/Zybo_GPIO_led_sws_INT_btn.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1

# Initializing cores ? Todo : research more
source ./HW/Vitis/Kernel/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config

# Send actual ELF to memory (using CPU0, I guess)
targets -set -nocase -filter {name =~ "*A9*#0"}
dow ./HW/Vitis/Kernel/Debug/Kernel.elf
configparams force-mem-access 0

# Starts CPU0
con
