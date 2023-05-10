#include "stdio.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdint.h"
#include "err.h"

int main(int argc, char **argv)
{
        if (argc != 2)
        {
                errx(1, "Invalid number of arguments! Usage: %s <file_name>", argv[0]);
        }

        char *file_name = argv[1];

        int fd;

        if ((fd = open(file_name, O_RDWR)) < 0)
        {
                err(2, "Cannot open file %s", file_name);
        }

        uint32_t bytes[256] = {0};

        uint8_t buf;
        int bytes_read = 0;
        while ((bytes_read = read(fd, &buf, sizeof(buf))) > 0)
        {
                bytes[buf] += 1;
        }

        if (bytes_read < 0)
        {
                err(3, "Cannot read from file %s", file_name);
        }

        if (lseek(fd, 0, SEEK_SET) < 0)
        {
                err(4, "Cannot lseek file %s", file_name);
        }

        for (int i = 0; i < 256; i++)
        {
                uint8_t c = i;
                for (int j = 0; j < bytes[i]; j++)
                {
                        if (write(fd, &c, sizeof(c)) < 0)
                        {
                                err(5, "Cannot write to file %s", file_name);
                        }
                }
        }

        close(fd);

        return 0;
}