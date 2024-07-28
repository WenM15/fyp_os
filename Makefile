K=kernel

OBJS = \
       $K/entry_point.o \
       $K/main.o \
       $K/init.o \
       $K/trap_vector.o \
       $K/process.o \
       $K/spinlock.o \

ifndef TOOLPREFIX
# inferring the correct TOOLPREFIX if not set
# if riscv64-unknown-elf-objdump that supports the elf64-littleriscv is present
# then we assume the toolchain is correctly installed
TOOLPREFIX := $(shell if riscv64-unknown-elf-objdump -i 2>&1 | grep 'elf64-littleriscv' >/dev/null 2>&1;\
	      then echo 'riscv64-unknown-elf-'; \
	      else echo "Error: riscv64 version of GCC/binutils not found." 1>&2; \
	      fi)
endif

# $(info TOOLPREFIX inferred: $(TOOLPREFIX))

CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

# Wall: Warning All
# Werror: Warning Error
# O: Basic compiler optimization
# ggdb: Generate debugging information in default format, with additional support for GDB
CFLAGS = -Wall -Werror -O -ggdb

# Explained in note_kernel/note_mcmodel
CFLAGS += -mcmodel=medany

# MD: Makefile dependency, generate dependency files alongside object files. Useful for determining which source file need to be
# recompiled when a header file changes
CFLAGS += -MD

# ffreestanding: Freestanding environment, meaning it doesn't rely on standard libraries and runtime
# fno-common: Prevents compiler from treating uninitialized global variables as common symbols (same symbol found in multiple source files treated as common/same)
# nostdlib: Prevents linking with standard library
# mno-relax: Disables instruction relaxation, an optimization technique that replaces instructions with more efficient sequences
CFLAGS += -ffreestanding -fno-common -nostdlib -mno-relax

# I: adds current directories to the list of directories to be searched for header files (useless but just in case a header file is moved to this directory)
CFLAGS += -I

# test whether the compiler supports stack protection, disable it if yes.
CFLAGS += $(shell if $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1; \
	  then echo -fno-stack-protector; \
	  fi)

# Disable PIE (Position Independent Executable)
# PIE is a security feature that allows executable to be loaded at random memory address

# ifneq: if not equal (to empty string in this case, no strings provided in 2nd argument)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep 'no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep 'nopie'),)
CFLAGS += -fno-pie -nopie
endif

# -z: used to specify additional control options
# max-page-size: a specific -z option that sets the virtual page size to n bytes, and aligns section to n-byte boundary
LDFLAGS = -z max-page-size=4096

all: $K/kernel

$K/kernel: $(OBJS) $K/linker_script.ld
	$(LD) $(LDFLAGS) -T $K/linker_script.ld -o $K/kernel $(OBJS)

$K/%.o: $K/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$K/%.o: $K/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

# Prevent deletion of intermediate files, e.g. object files, after the first build.
# This ensures that object files for unchanged source files are preserved
# across multiple builds, allowing the disk image to be incrementally updated.
# Object files are only removed when 'make clean' is run.
.PRECIOUS: %.o

# include dependency files in the Makefile
-include kernel/*.d user/*.d

clean:
	rm -f $K/*.o $K/*.d $K/kernel

# set up for QEMU
QEMU = qemu-system-riscv64

ifndef CPUS
CPUS := 3
endif

QEMUOPTS = -machine virt -bios none -m 128M -smp $(CPUS) -nographic
QEMUOPTS += -kernel $K/kernel

qemu: $K/kernel
	$(QEMU) $(QEMUOPTS)
