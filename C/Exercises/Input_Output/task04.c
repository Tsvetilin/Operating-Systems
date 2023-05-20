#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int open_safe(const char* file_name);
void write_safe(int fd, char c, const char* file_name);
void copy_content(int fd1, const char* f1, int fd2, const char* f2);
void truncate_safe(int fd, const char* file_name);
void lseek_safe(int fd, off_t position, int flags, const char* file_name);

int open_safe(const char* file_name) {
    int fd;
    if (( fd = open(file_name, O_RDWR | O_CREAT, S_IRWXU)) == -1) {
        err(2, "Error while opening file %s", file_name);
    }

    return fd;
}

void write_safe(int fd, char c, const char* file_name) {
    if (write(fd, &c, 1) != 1) {
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
        err(4, "Error while reading from file %s", f1);
    }
}

void lseek_safe(int fd, off_t position, int flags, const char* file_name) {
    if (lseek(fd, position, flags) == -1) {
        err(6, "Error while lseek in file %s", file_name);
    }
}

void truncate_safe(int fd, const char* file_name) {
    lseek_safe(fd, 0, SEEK_SET, file_name);
    if (ftruncate(fd, 0) == -1) {
        err(5, "Error while truncating file %s", file_name);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    const char *file1 = argv[1], *file2 = argv[2], *temp = "temp.txt";

    int fd1 = open_safe(file1);
    int fd2 = open_safe(file2);
    int fd3 = open_safe(temp);
    truncate_safe(fd3, temp);
    copy_content(fd1, file1, fd3, temp);

    truncate_safe(fd1, file1);
    copy_content(fd2, file2, fd1, file1);

    truncate_safe(fd2, file2);
    lseek_safe(fd3, 0, SEEK_SET, temp);
    copy_content(fd3, temp, fd2, file2);

    close(fd1);
    close(fd2);
    close(fd3);
}

//Реализирайте команда swap разменяща съдържанието на два файла, подадени като входни параметри.