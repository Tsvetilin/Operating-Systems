#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
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

int main(void) {
    const char* etc = "/etc/passwd";
    const char* copy = "./passwd";

    int fd1, fd2;

    if ((fd1 = open(etc, O_RDONLY)) == -1) {
        err(1, "Error while opening file %s", etc);
    }

    if ((fd2 = open(copy, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
        close_all();
        err(2, "Error while opening file %s", copy);
    }

    char c;
    int bytes_count;
    while ((bytes_count = read(fd1, &c, sizeof(c))) == sizeof(c)) {
        if ( c == ':') {
            c = '?';
        }

        if ((write(fd2, &c, sizeof(c))) != sizeof(c)) {
            close_all();
            err(3, "Error while writing in file %s", copy);
        }
    }

    if (bytes_count == -1) {
        close_all();
        err(4, "Error while reading from file %s", etc);
    }

    close_all();
    exit(0);
}

//Koпирайте файл /etc/passwd в текущата ви работна директория и променете разделителят на копирания файл от ":", на "?"