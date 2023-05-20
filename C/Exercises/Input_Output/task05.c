#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    const char *file1 = argv[1], *file2 = argv[2];
    int fd1, fd2;

    if (( fd1 = open(file1, O_RDONLY )) == -1) {
        err(2, "Error while opening file %s", file1);
    }

    if (( fd2 = open(file2, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
        int errno_ = errno;
        close(fd1);
        errno = errno_;
        err(2, "Error while opening file %s", file2);
    }

    int bytes_count;
    char c;
    while (( bytes_count = read(fd1, &c, sizeof(c))) == 1) {
        if (( write(fd2, &c, bytes_count)) != 1) {
            close(fd1);
            close(fd2);
            err(3, "Error while writing to file %s", file2);
        }
    }

    if (bytes_count == -1) {
        err(4, "Error while reading from file %s", file1);
    }

    close(fd1);
    close(fd2);
    exit(0);
}

//Реализирайте команда cp, работеща с два аргумента, подадени като входни параметри.