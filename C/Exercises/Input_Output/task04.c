#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int open_safe(const char* file_name);
void write_safe(int fd, char c, const char* file_name);
void copy_content(int fd1, const char* f1, int fd2, const char* f2);
void truncate_safe(int fd, const char* file_name);
void lseek_safe(int fd, off_t position, int flags, const char* file_name);
void close_all();

int fds[3] = {0, 0, 0};

void close_all() {
    int errno_ = errno;
    for (int i = 0; i < 3; i++) {
        if (fds[i] > 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

int open_safe(const char* file_name) {
    int fd;
    if (( fd = open(file_name, O_RDWR | O_CREAT, S_IRWXU)) == -1) {
        close_all();
        err(2, "Error while opening file %s", file_name);
    }

    return fd;
}

void write_safe(int fd, char c, const char* file_name) {
    if (write(fd, &c, 1) != 1) {
        close_all();
        err(3, "Error while writing in file %s", file_name);
    }
}

void copy_content(int fd1, const char* f1, int fd2, const char* f2) {
    int bytes_count;
    char c;
    while((bytes_count = read(fd1, &c, 1)) == 1) {
        write_safe(fd2, c, f2);
    }

    if(bytes_count == -1) {
        close_all();
        err(4, "Error while reading from file %s", f1);
    }
}

void lseek_safe(int fd, off_t position, int flags, const char* file_name) {
    if (lseek(fd, position, flags) == -1) {
        close_all();
        err(6, "Error while lseek in file %s", file_name);
    }
}

void truncate_safe(int fd, const char* file_name) {
    lseek_safe(fd, 0, SEEK_SET, file_name);
    if (ftruncate(fd, 0) == -1) {
        close_all();
        err(5, "Error while truncating file %s", file_name);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    const char *file1 = argv[1], *file2 = argv[2];
    char temp[11] = "tempXXXXXX";
    fds[0]  = open_safe(file1);
    fds[1] = open_safe(file2);
    fds[2] = mkstemp(temp);
    truncate_safe(fds[2], temp);
    copy_content(fds[0], file1, fds[2], temp);

    truncate_safe(fds[0], file1);
    copy_content(fds[1], file2, fds[0], file1);

    truncate_safe(fds[1], file2);
    lseek_safe(fds[2], 0, SEEK_SET, temp);
    copy_content(fds[2], temp, fds[1], file2);

    close_all();

    if (unlink(temp) != 0) {
        err(7, "Error while unlinking file %s", temp);
    }
}

//Реализирайте команда swap, разменяща съдържанието на два файла, подадени като входни параметри.
