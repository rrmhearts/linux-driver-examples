export CROSS_COMPILE=
export ARCH=x86_64

TARGET_MODULE:=sample-module

# If we are running by kernel building system
ifneq ($(KERNELRELEASE),)
	$(TARGET_MODULE)-objs := char_example.o
	obj-m := $(TARGET_MODULE).o
# If we running without kernel build system
else
	BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)
all : 
# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules
clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean
load:
	sudo insmod ./$(TARGET_MODULE).ko

unload:
	sudo rmmod ./$(TARGET_MODULE).ko
endif
