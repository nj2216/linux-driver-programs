obj-m += myramdisk_dt.o

KDIR := /home/student/riscv-workspace/linux

all:
	make -C $(KDIR) M=$(PWD) ARCH=riscv CROSS_COMPILE=riscv64-linux-gnu- modules

clean:
	make -C $(KDIR) M=$(PWD) clean
