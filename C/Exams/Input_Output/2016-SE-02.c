#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdio.h>

struct pair
{
    uint32_t x;
    uint32_t y;
};

int fds[3];

void close_all(void);
void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 3; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

int main(int argc, char **argv)
{

    if (argc != 4)
        errx(1, "ERROR: Arg count");

    int fd1, fd2;

    if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
        err(2, "ERROR: %s", argv[1]);
    }

    if ((fd2 = open(argv[2], O_RDONLY)) == -1) {
        close_all();
        err(2, "ERROR: %s", argv[2]);
    }

    struct stat s;

    if (fstat(fd1, &s) == -1) {
        close_all();
        err(7, "Could not fstat");
    }

    if (s.st_size % 8 != 0) {
        close_all();
        errx(2, "ERROR: f1 file size must be divisible by 8");
    }

    struct pair p;

    int bytes_count = 0;

    int fd3;
    if ((fd3 = open(argv[3], O_WRONLY | O_CREAT, S_IRWXU)) == -1) {
        close_all();
        err(4, "ERROR: Opening file %s", argv[3]);
    }

    while ((bytes_count = read(fd1, &p, sizeof(p))) > 0)
    {
        if (lseek(fd2, p.x * 4, SEEK_SET) == -1) {
            close_all();
            err(5, "ERROR: lseek");
        }

        uint32_t buf;

        for (uint32_t i = 0; i < p.y; ++i)
        {
            if (read(fd2, &buf, sizeof(buf)) == -1) {
                close_all();
                err(4, "ERROR: Reading");
            }

            if (write(fd3, &buf, sizeof(buf)) == -1) {
                close_all();
                err(5, "ERROR: Writing");
            }
        }
    }

    if (bytes_count == -1) {
        close_all();
        err(3, "ERROR: Reading");
    }

    close_all();
    exit(0);
}