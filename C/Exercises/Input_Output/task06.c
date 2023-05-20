#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>

void write_safe(int fd, char c, int size, const char* file_name);

void write_safe(int fd, char c, int size, const char* file_name) {
    if (( write(fd, &c, size)) != size) {
        int errno_ = errno;
        close(fd);
        errno = errno_;
        err(2, "Error while writing in file %s", file_name);
    }
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        const char* file_name = argv[i];
        int fd;

        if (( fd = open(file_name, O_RDONLY)) == -1) {
            err(1, "Error while opening file %s", file_name);
        }

        char c;
        int bytes_count;
        while (( bytes_count = read(fd, &c, sizeof(c))) == 1) {
            write_safe(1, c, bytes_count, file_name);
        }

        if (bytes_count == -1) {
            int errno_ = errno;
            close(fd);
            errno = errno_;
            err(3, "Error while reading from file %s", file_name);
        }

        write_safe(1, '\n', 1, file_name);

        close(fd);
    }

    exit(0);
}

//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.