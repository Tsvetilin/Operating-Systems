#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Expected 2 args");
    }

    const char* file_name = argv[1];
    int fd;
    if (( fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Error while opening file %s", file_name);
    }

    char c;
    int lines = 0, words = 0, chars = 0;
    int bytes_counter;
    while((bytes_counter = read(fd, &c, 1)) == 1) {
        if (c == '\n') {
            lines++;
            words++;
        }

        if (c == ' ') {
            words++;
        }

        chars++;
    }

    if (bytes_counter == -1) {
        err(3, "Error while reading from file %s", file_name);
    }

    dprintf(1, "Files %s has:\nLines: %d\nWords: %d\nCharacters: %d\n", file_name, lines, words, chars);
}

//Реализирайте команда wc, с един аргумент подаден като входен параметър