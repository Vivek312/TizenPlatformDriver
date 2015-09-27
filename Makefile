# The following line contains the file(s) from which the module will be built
obj-m += myfifo.o
#obj-m += load1.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
