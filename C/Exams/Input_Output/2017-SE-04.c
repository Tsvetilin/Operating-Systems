#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

void close_safe(int fd);
void write_safe(int fd, char c, int size);
int open_safe(const char* file_name);
void read_safe(int fd, const char* file_name);

void close_safe(int fd) {
    int errno_ = errno;
    if (fd >= 0) {
        close(fd);
    }

    errno = errno_;
}

void write_safe(int fd, char c, int size) {
    if (write(1, &c, size) != size) {
        close_safe(fd);
        err(1, "Could not write to stdout");
    }
}

int open_safe(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        err(3, "Could not open file %s", file_name);
    }

    return fd;
}

void read_safe(int fd, const char* file_name) {
    char c;
    int bytes_read;
    while((bytes_read = read(fd, &c, sizeof(c))) == sizeof(c)) {
        write_safe(fd, c, sizeof(c));
    }

    if (bytes_read == -1) {
        close_safe(fd);
        err(2, "Could not read from %s", file_name);
    }

    write_safe(fd, '\n', sizeof(char));
    close(fd);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        read_safe(0, "stdin");
        exit(0);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-") == 0) {
            read_safe(0, "stdin");
        } else {
            int fd = open_safe(argv[i]);
            read_safe(fd, argv[i]);
        }
    }

    exit(0);
}

//Напишете програма на C, която да работи подобно на командата cat, реализирайки
//само следната функционалност:
//• програмата извежда на STDOUT
//• ако няма подадени параметри, програмата чете от STDIN
//• ако има подадени параметри – файлове, програмата последователно ги извежда
//• ако някой от параметрите започва с тире (-), програмата да го третира като специално име
//        за STDIN
//        Примерно извикване:
//$ ./main f - g
//– извежда съдържанието на файла f, после STDIN, след това съдържанието на файла g.