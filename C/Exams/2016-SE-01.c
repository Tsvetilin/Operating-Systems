#include "stdio.h"
#include "string.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdint.h"
#include "err.h"

int main(int argc, char **argv)
{
        char *file_name = argv[1];

        int fd = open(file_name, O_RDWR);

        if (fd < 0)
        {
                err(2, "Cannot open file %s", file_name);
        }

        int bytes[256] = {0};

        uint8_t buf;

        while (read(fd, &buf, sizeof(buf)) > 0)
        {
                bytes[buf] += 1;
        }

        lseek(fd, 0, SEEK_SET);

        for (int i = 0; i < 256; i++)
        {
                uint8_t c = i;
                for (int j = 0; j < bytes[i]; j++)
                {
                        write(fd, &c, sizeof(c));
                }
        }

        close(fd);

        return 0;
}