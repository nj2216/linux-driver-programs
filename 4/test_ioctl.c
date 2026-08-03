#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "hello_ioctl.h"

int main(void)
{
    int fd, size;

    fd = open("/dev/hellochar", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("returned: %d | size = %d\n", ioctl(fd, HELLO_GET_SIZE, &size), size);

    printf("returned: %d | size = %d\n", ioctl(fd, HELLO_RESET, NULL), size);

    printf("returned: %d | size = %d\n", ioctl(fd, HELLO_GET_SIZE, &size), size);

    close(fd);
    return 0;
}
