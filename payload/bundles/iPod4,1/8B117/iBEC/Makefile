CROSS ?= arm-elf-
CC = $(CROSS)gcc
LD = $(CROSS)ld
SRC = ../../../..
LOADADDR = 0x41000000
OUTPUT = iPod4,1_4.1_iBEC.payload
OBJCOPY = $(CROSS)objcopy
OBJECTS = entry.o main.o common.o commands.o task.o lock.o aes.o bdev.o image.o nvram.o filesystem.o kernel.o lzss.o
CFLAGS = -I. -I./.. -I./../.. -I./$(SRC)/include -nostdlib -mlittle-endian -mthumb-interwork
LDFLAGS = -Ttext=$(LOADADDR) -mthumb-interwork -nostdlib -lc -lm -lgcc

%.o: $(SRC)/%.S
	$(CC) -c $(<) -o $(@) $(INCLUDES) $(CFLAGS) 

%.o: $(SRC)/%.c
	$(CC) -c $(<) -o $(@) $(INCLUDES) $(CFLAGS) 

all: payload

payload: payload.elf
	mv payload $(SRC)/../staging/$(OUTPUT)
	
payload.elf: $(OBJECTS)
	$(CC) -o $(@) $(OBJECTS) $(LDFLAGS)
	$(OBJCOPY) -O binary $(@) payload

clean:
	rm -rf *.o *.elf payload
	
