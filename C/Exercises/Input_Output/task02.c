#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <stdint.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Expected 2 args");
    }

    const char* file_name = argv[1];
    int fd;

    if (( fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Error reading from file %s", file_name);
    }

    char c;
    uint8_t line_counter = 0;
    int bytes_counter = 0;
    while ((bytes_counter = read(fd, &c, 1)) == 1) {
        if (write(1, &c, 1) != 1) {
            err(3, "Error writing to stdout");
        }
        if (c == '\n') {
            line_counter++;
            if (line_counter == 10) {
                break;
            }
        }
    }

    if (bytes_counter == -1) {
        err(4, "Error reading from file %s", file_name);
    }

    close(fd);
    exit(0);
}


//Реализирайте команда head без опции (т.е. винаги да извежда на стандартния изход само първите 10 реда от
//съдържанието на файл подаден като първи параматър).