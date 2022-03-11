obj-m:=process_list.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
	gcc user.c -o user
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	find . -type f -executable -delete