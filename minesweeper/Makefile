
CROSS_COMPILE ?= arm-linux-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
STRIP := $(CROSS_COMPILE)strip
TARGET_CC := $(CROSS_COMPILE)gcc

LIB_DIR := ../install
CFLAGS += -fno-builtin -nostdinc -nostdlib -fno-stack-protector -nostartfiles
GCCSYSTEM_DIR :=/opt/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3
CFLAGS += -isystem $(GCCSYSTEM_DIR)/include
CFLAGS += -I$(GCCSYSTEM_DIR)/include-fixed
CFLAGS += -I$(LIB_DIR)/usr/include
CFLAGS += -march=armv5
CFLAGS += -I../linux_header_goldfish/include 
CFLAGS += -msoft-float 

all: minesweeper

minesweeper: minesweeper.c
	$(CC) $(CFLAGS) -I$(LIB_DIR)/include -g  -c $<
	$(LD) -static -T $(LIB_DIR)/user1.ld -o $@ -L$(LIB_DIR)/usr/lib -L$(LIB_DIR)/lib $(LIB_DIR)/usr/lib/crt1.o $(LIB_DIR)/usr/lib/crti.o minesweeper.o -lminigui_ths -lpthread -lm -lrt -lc $(GCCSYSTEM_DIR)/libgcc.a $(LIB_DIR)/usr/lib/crtn.o
	$(STRIP) $@	

clean:
	rm -rf minesweeper *.o
