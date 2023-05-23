#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>

void close_safe(int fd);
void close_safe(int fd) {
    int errno_ = errno;
    close(fd);
    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    const char *option = argv[1], *file_name = argv[2];
    const char *min = "--min", *max = "--max", *print = "--print";
    if ((strcmp(option, min) != 0) && (strcmp(option, max) != 0) && (strcmp(option, print)) != 0) {
        errx(2, "Invalid option");
    }

    unsigned char byte;
    int fd, bytes_count;
    struct stat st;

    if (stat(file_name, &st) == -1) {
        err(6, "Error while stat file %s", file_name);
    }

    if (st.st_size == 0) {
        errx(7, "File %s is empty", file_name);
        exit(0);
    }

    if ((st.st_size % sizeof(unsigned char)) != 0) {
        errx(8, "File %s does not contain only uint16_t bytes", file_name);
        exit(0);
    }

    if (( fd = open(file_name, O_RDONLY)) == -1) {
        err(3, "Error while opening file %s", file_name);
    }



    if (strcmp(option, min) == 0) {
        unsigned char minByte;
        if (read(fd, &minByte, sizeof(minByte)) == -1) {
            close_safe(fd);
            err(5, "Error while reading from file %s", file_name);
        }
        while ((bytes_count = read(fd, &byte, sizeof(byte))) == sizeof(byte)) {
            if (minByte > byte) {
                minByte = byte;
            }
        }
        dprintf(1, "%u\n", minByte);
    } else if (strcmp(option, max) == 0) {
        unsigned char maxByte = 0;
        while ((bytes_count = read(fd, &byte, sizeof(byte))) == sizeof(byte)) {
            if (maxByte < byte) {
                maxByte = byte;
            }
        }
        dprintf(1, "%u\n", maxByte);
    } else {
        while ((bytes_count = read(fd, &byte, sizeof(byte))) == sizeof(byte)) {
            dprintf(1, "%u\n", byte);
        }
    }

    if (bytes_count == -1) {
        close_safe(fd);
        err(4, "Error while reading from %s", file_name);
    }

    close_safe(fd);
    exit(0);
}

//Модифицирайте предната програма, така че да намерите най-големия байт.
