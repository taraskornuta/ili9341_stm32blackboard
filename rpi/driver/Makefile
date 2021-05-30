TARGET = ili9341_spi_fb

obj-m += $(TARGET).o

ccflags-y := -std=gnu99 -Wno-declaration-after-statement
module-objs := ili9341_spi_fb.o ili9341_spi_bus.o

EXTRA_CFLAGS += -I/usr/lib/gcc/arm-linux-gnueabihf/8/include/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean


