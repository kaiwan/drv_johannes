obj-m += my_dma_memcpy.o

ccflags-y += -DDEBUG
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
