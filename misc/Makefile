
obj-m += misc_example.o
# KDIR := /lib/modules/$(shell uname -r)/build
KDIR := ~/kernel
PWD := $(shell pwd)

# CC := $(CROSS_COMPILE)gcc
CROSS=arm-eabi-

all:
	make -C $(KDIR) M=$(PWD) modules
        
clean:
	make -C $(KDIR) M=$(PWD) clean
