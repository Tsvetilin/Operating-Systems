#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>

const uint8_t files_count = 2;
int fds[2] = {0, 0};

void close_all(void);
void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < files_count; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    const char *file1 = argv[1], *file2 = argv[2];

    if (( fds[0] = open(file1, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file1);
    }

    if (( fds[1] = open(file2, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        close_all();
        err(2, "Could not open file %s", file2);
    }

    char c;
    int bytes_counter;
    while ((bytes_counter = read(fds[0], &c, sizeof(c))) == sizeof(c)) {
        if (write(fds[1], &c, sizeof(c)) != sizeof(c)) {
            close_all();
            err(3, "Error writing in file %s", file2);
        }
    }

    if (bytes_counter == -1) {
        close_all();
        err(4, "Error reading from file %s", file1);
    }

    close_all();
    exit(0);
}

//Копирайте съдържанието на файл1 във файл2.