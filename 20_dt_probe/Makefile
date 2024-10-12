obj-m += dt_probe.o

all: module dt
	@echo 'Built Device Tree Overlay and kernel module'

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
dt: rpi_testoverlay.dts bbb_testoverlay.dts
	dtc -@ -I dts -O dtb -o rpi_testoverlay.dtbo rpi_testoverlay.dts
	dtc -@ -I dts -O dtb -o BBB-TESTOVERLAY.dtbo bbb_testoverlay.dts
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf *.dtbo
