# maximum number of CPU cores supported
.equ MAX_CPU, 8

# set up a stack for all CPU cores
# each core is allocated a 4kb (2^12 or 4096 bytes) stack space
.section .bss
.align 16
.global cpu_stack
cpu_stack:
	.space 4096 * MAX_CPU

# qemu -kernel loads the kernel at 0x80000000
# and causes each core (or hart, stands for hardware thread) to jump there
# this address is set as a starting address in the linker script (linker_script.ld) 
# to directly map the base address (also 0x80000000) of the DRAM of QEMU's Generic Virtual Platform (virt)
.section .text
.global entry_point
entry_point:
	# stack pointer is set to point to: cpu_stack + 4096 * (mhartid + 1) - 1

	# a0 = cpu_stack
	# a1 = 4096
	# a2 = mhartid (hart id, starting from 0)
	la a0, cpu_stack
	li a1, 4096
	csrr a2, mhartid

	# a2 = mhartid + 1
	addi a2, a2, 1
	# a1 = 4096 * (mhartid + 1)
	mul a1, a1, a2
	# a0 = cpu_stack + 4096 * (mhartid + 1)
	add a0, a0, a1

	addi sp, a0, -1

	# jump to main() in main.c
	call main

spin:
	jal x0, spin
