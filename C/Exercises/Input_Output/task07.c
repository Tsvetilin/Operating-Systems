#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

const uint8_t files_count = 2;
int fds[2] = {0, 0};

void close_all(void);
void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < files_count; i++) {
        if (fds[i] > 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        errx(1, "Expected at least 3 args");
    }

    const char *dir = argv[argc-1];

    struct stat dir_stats;

    if (stat(dir, &dir_stats) == -1) {
        err(2, "Cannot stat dir %s", dir);
    }

    if(!S_ISDIR(dir_stats.st_mode)) {
        errx(3, "Expected %s to be a directory", dir);
    }

    char *dirName;
    if (dir[strlen(dir) - 1] != '/') {
        dirName = malloc(strlen(dir) + 1);
        strcpy(dirName, dir);
        strcat(dirName, "/");
    } else {
        dirName = malloc(strlen(dir));
        strcpy(dirName, dir);
    }

    for (int i = 1; i < (argc - 1); i++) {
        const char* source = argv[i];
        char* target = malloc(strlen(dirName) + strlen(source) + 1);
        strcpy(target, dirName);
        strcat(target, source);

        if (( fds[0] = open(source, O_RDONLY)) == -1) {
            err(4, "Error while opening file %s", source);
        }

        if (( fds[1] = open(target, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
            close_all();
            err(5, "Error while opening file %s", target);
        }

        int bytes_count;
        char c;
        while (( bytes_count = read(fds[0], &c, sizeof(c))) == sizeof(c)) {
            if (( write(fds[1], &c, sizeof(c))) != sizeof(c)) {
                close_all();
                err(6, "Error while writing to file %s", target);
            }
        }

        if (bytes_count == -1) {
            close_all();
            err(7, "Error while reading from file %s", source);
        }

        close_all();
        free(target);
    }

    free(dirName);
    exit(0);
}

//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.