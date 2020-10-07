make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- spl/u-boot-spl.bin
/opt/tool/gcc-linaro-7.4.1-2019.02-i686_arm-linux-gnueabi/bin/arm-linux-gnueabi-objdump -D spl/u-boot-spl > spl/uboot-spl.s
