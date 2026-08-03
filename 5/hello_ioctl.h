#ifndef HELLO_IOCTL_H
#define HELLO_IOCTL_H

#include <linux/ioctl.h>

#define HELLO_MAGIC 'h'
#define HELLO_RESET    _IO(HELLO_MAGIC, 1)
#define HELLO_GET_SIZE _IOR(HELLO_MAGIC, 2, int)

#endif
