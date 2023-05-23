
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

    uint16_t number;
    int fd, bytes_count;
    struct stat st;

    if (stat(file_name, &st) == -1) {
        err(6, "Error while stat file %s", file_name);
    }

    if (st.st_size == 0) {
        errx(7, "File %s is empty", file_name);
        exit(0);
    }

    if ((st.st_size % sizeof(uint16_t)) != 0) {
        errx(8, "File %s does not contain only uint16_t numbers", file_name);
        exit(0);
    }

    if (( fd = open(file_name, O_RDONLY)) == -1) {
        err(3, "Error while opening file %s", file_name);
    }



    if (strcmp(option, min) == 0) {
        uint16_t minN;
        if (read(fd, &minN, sizeof(minN)) == -1) {
            close_safe(fd);
            err(5, "Error while reading from file %s", file_name);
        }
        while ((bytes_count = read(fd, &number, sizeof(number))) == sizeof(number)) {
            if (minN > number) {
                minN = number;
            }
        }
        dprintf(1, "%d\n", minN);
    } else if (strcmp(option, max) == 0) {
        uint16_t maxN = 0;
        while ((bytes_count = read(fd, &number, sizeof(number))) == sizeof(number)) {
            if (maxN < number) {
                maxN = number;
            }
        }
        dprintf(1, "%d\n", maxN);
    } else {
        while ((bytes_count = read(fd, &number, sizeof(number))) == sizeof(number)) {
            dprintf(1, "%d\n", number);
        }
    }

    if (bytes_count == -1) {
        close_safe(fd);
        err(4, "Error while reading from %s", file_name);
    }

    close_safe(fd);
    exit(0);
}


//Напишете програма, която приема точно 2 аргумента. Първият може да бъде само --min, --max или --print
//(вижте man 3 strcmp). Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа
//(uint16_t - вижте man stdint.h). Ако първият аргумент е:
//
//--min - програмата отпечатва кое е най-малкото число в двоичния файл.
//--max - програмата отпечатва кое е най-голямото число в двоичния файл.
//--print - програмата отпечатва на нов ред всяко число.
//Използвайте двоичния файл binary/dump или генерирайте сами такъв (правилно подравнен).