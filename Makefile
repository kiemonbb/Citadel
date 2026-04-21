CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

SRC_C = $(wildcard src/sys/*.c) startup/system_stm32l0xx.c src/main.c
SRC_AS = startup/startup_stm32l031k6tx.s
OBJ = $(SRC_C:.c=.o) $(SRC_AS:.s=.o)

CFLAGS =	-mcpu=cortex-m0plus 
CFLAGS +=	-DSTM32L031xx 
CFLAGS +=	-Ivendor/cmsis -Ivendor/device -Isrc/startup -Isrc/config -Isrc/sys
CFLAGS +=	-g -O2 -Wall

LINKER_FILE = STM32L031K6TX_FLASH.ld
LDFLAGS = -T $(LINKER_FILE) --specs=nano.specs -specs=nosys.specs

PROGRAMMER = openocd
PROGRAMMER_FLAGS =  -f interface/stlink.cfg -f target/stm32l0.cfg

.PHONY: all clean flash

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

all: main.elf main.bin

main.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

flash: main.elf
	$(PROGRAMMER) $(PROGRAMMER_FLAGS) -c "program $< verify reset exit"

clean:
	rm -f $(OBJ) main.elf main.bin


