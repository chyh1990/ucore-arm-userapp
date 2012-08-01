
export CROSS_COMPILE=arm-linux-
export LIBC_DIR="`pwd`/install"
export GCC_DIR=/opt/FriendlyARM/toolschain/4.4.3/lib/gcc/arm-none-linux-gnueabi/4.4.3
export CFLAGS="-isystem $GCC_DIR/include -I$LIBC_DIR/usr/include -march=armv5 -I`pwd`/linux_header_goldfish/include -g -ggdb -D_DEBUG"


