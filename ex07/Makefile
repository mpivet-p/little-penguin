obj-m += ex07.o

ex07-objs += ex07_main.o ex07_id.o ex07_jiffies.o ex07_foo.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
