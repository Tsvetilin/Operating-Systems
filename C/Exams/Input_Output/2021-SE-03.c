#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

int fds[2];

void close_all(void);
int open_read(const char* file_name);
int open_creat(const char* file_name);
void write_safe(int fd, const char* str, const char* file_name);
uint16_t read_num(int fd, const char* file_name);

uint16_t read_num(int fd, const char* file_name) {
    int bytes_read;
    uint16_t num;

    if ((bytes_read = read(fd, &num, sizeof(num))) != sizeof(num)) {
        if (bytes_read == -1) {
            err(5, "Could not read from file %s", file_name);
        } else if (bytes_read > 0) {
            errx(5, "Could not read from file %s", file_name);
        }
    }

    return num;
}

void write_safe(int fd, const char* str, const char* file_name) {
    int bytes_read;
    if ((bytes_read = write(fd, str, strlen(str))) != (ssize_t)strlen(str)) {
        if (bytes_read == -1) {
            err(7, "Could not write to file %s", file_name);
        } else {
            errx(7, "Could not write to file %s", file_name);
        }
    }
}

int open_read(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

int open_creat(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 2; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <input_file> <output_file>", argv[0]);
    }

    const char *input_file = argv[1], *output_file = argv[2];

    struct stat s;
    if (stat(input_file, &s) == -1) {
        err(3, "Could not stat file %s", input_file);
    }

    int nums_count = s.st_size / 2;
    if (s.st_size % sizeof(uint16_t) != 0 || nums_count > 524288) {
        errx(4, "Invalid input file content");
    }

    fds[0] = open_read(input_file);
    fds[1] = open_creat(output_file);
    write_safe(fds[1], "#include <stdint.h>\n\n", output_file);
    write_safe(fds[1], "uint32_t arrN = ", output_file);
    dprintf(fds[1], "%ld;\n", s.st_size / 2);
    write_safe(fds[1], "const uint16_t arr[] = {", output_file);

    uint16_t num;
    for (int i = 0; i < nums_count; i++) {
        num = read_num(fds[0], input_file);
        dprintf(fds[1], "%d, ", num);
    }

    num = read_num(fds[0], input_file);
    dprintf(fds[1], "%d};\n", num);
}

//Напишете програма на C, която приема два позиционни параметъра – имена на файлове. Примерно
//        извикване:
//$ ./main input.bin output.h
//        Файлът input.bin е двоичен файл с елементи uint16_t числа, създаден на little-endian машина.
//Вашата програма трябва да генерира C хедър файл, дефиниращ масив с име arr, който:
//• съдържа всички елементи от входния файл;
//• няма указана големина;
//• не позволява промяна на данните.
//Генерираният хедър файл трябва да:
//• съдържа и uint32_t променлива arrN, която указва големината на масива;
//• бъде валиден и да може да се #include-ва без проблеми от C файлове, очакващи да “виждат”
//arr и arrN.
//За да е валиден един входен файл, той трябва да съдържа не повече от 524288 елемента.
