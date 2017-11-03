obj-m += uart_loop.o

KDIR := ~/kernel
PWD := $(shell pwd)
CROSS=arm-eabi-

uart_loop:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

