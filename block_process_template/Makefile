obj-m += main.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc cat_nonblock.c -o cat_nonblock

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f cat_nonblock
