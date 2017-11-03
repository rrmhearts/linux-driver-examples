obj-m += i2c_accel.o

KDIR := ~/kernel
PWD := $(shell pwd)
CROSS=arm-eabi-

i2c_accel:
	make -C $(KDIR) M=$(PWD) ARCH=arm CROSS_COMPILE=$(CROSS) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

