#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <err.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    const char *file1 = argv[1], *file2 = argv[2];
    int fd1, fd2;

    if (( fd1 = open(file1, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file1);
    }

    if (( fd2 = open(file2, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
        err(2, "Could not open file %s", file2);
    }

    char c;
    while ((read(fd1, &c, 1)) == 1) {
        if (write(fd2, &c, 1) != 1) {
            err(3, "Error writing in file %s", file2);
        }
    }

    close(fd1);
    close(fd2);
}

//Копирайте съдържанието на файл1 във файл2