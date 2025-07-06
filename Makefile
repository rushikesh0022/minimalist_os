CC = aarch64-none-elf-gcc
LD = aarch64-none-elf-ld
OBJCOPY = aarch64-none-elf-objcopy
CFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -mcpu=cortex-a53
ASM_FLAGS = -mcpu=cortex-a53

SRCS = boot.S kernel.c uart.c allocator.c
OBJS = $(SRCS:.S=.o)
OBJS := $(OBJS:.c=.o)
TARGET = kernel8.img

.PHONY: all clean qemu run

all: $(TARGET)

%.o: %.S
	$(CC) $(ASM_FLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(LD) -T linker.ld -o kernel8.elf $(OBJS)
	$(OBJCOPY) -O binary kernel8.elf $@

qemu: $(TARGET)
	qemu-system-aarch64 -M virt -cpu cortex-a53 -nographic -kernel $(TARGET)

# Add a run target that calls qemu
run: qemu

clean:
	rm -f *.o *.elf *.img
