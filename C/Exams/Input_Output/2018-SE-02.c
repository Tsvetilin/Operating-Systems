
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/param.h>

int fds[4];

void close_all(void);
int comp(const void* a, const void* b);
void merge(int firstLength, int secondLength, const char* output_file, const char* temp1, const char* temp2);
void write_safe(int fd, uint32_t num, const char* file_name);
void load_arr(uint32_t* arr, int size);
void load_file(int fd, uint32_t* arr, int size, const char* file_name);
uint32_t readNumber(int fd, const char* file_name);
void split(int fd, char* file_name, int size);

int comp(const void* a, const void* b) {
    const uint32_t* num1 = (const uint32_t*)a;
    const uint32_t* num2 = (const uint32_t*)b;

    if (*num1 < *num2) {
        return -1;
    } else if (*num1 > *num2) {
        return 1;
    } else {
        return 0;
    }
}

void write_safe(int fd, uint32_t num, const char* file_name) {
    if (write(fd, &num, sizeof(num)) != sizeof(num)) {
        close_all();
        err(7, "Could not write to file %s", file_name);
    }
}

void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 4; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

void load_arr(uint32_t* arr, int size) {
    for (int i = 0; i < size; i++) {
        if (read(fds[0], &arr[i], sizeof(uint32_t)) == -1) {
            close_all();
            err(5, "Could not read from file");
        }
    }
}

void load_file(int fd, uint32_t* arr, int size, const char* file_name) {
    for (int i = 0; i < size; i++) {
        write_safe(fd, arr[i], file_name);
    }
}

uint32_t readNumber(int fd, const char* file_name) {
    uint32_t number;
    if (read(fd, &number, sizeof(number)) == -1) {
        close_all();
        err(10, "Could not read from file %s", file_name);
    }

    return number;
}

void merge(int firstLength,  int secondLength, const char* output_file, const char* temp1, const char* temp2) {
    if ((fds[1] = open(output_file, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        close_all();
        err(6, "Could not open file %s", output_file);
    }

    uint32_t num1 = readNumber(fds[2], temp1), num2 = readNumber(fds[3], temp2);
    int minSize = MIN(firstLength, secondLength), counter = 0;
    while (counter < minSize) {
        if (num1 < num2) {
            write_safe(fds[1], num1, output_file);
            num1 = readNumber(fds[2], temp1);
        } else {
            write_safe(fds[1], num2, output_file);
            num2 = readNumber(fds[3], temp2);
        }
        counter++;
    }

    while (counter < firstLength) {
        write_safe(fds[1], num1, output_file);
        num1 = readNumber(fds[2], temp1);
        counter++;
    }

    while (counter < secondLength) {
        write_safe(fds[1], num2, output_file);
        num2 = readNumber(fds[3], temp2);
        counter++;
    }
}

void split(int fd, char* file_name, int size) {
    uint32_t* half = malloc(size * sizeof(uint32_t));
    load_arr(half, size);
    qsort(half, size, sizeof(uint32_t) ,comp);
    load_file(fd, half, size, file_name);
    free(half);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <input_file> <output_file>", argv[0]);
    }

    const char *input_file = argv[1], *output_file = argv[2];
    struct stat s1;

    if (stat(input_file, &s1) == -1) {
        err(2, "Could not stat file %s", input_file);
    }

    if (s1.st_size % 4 != 0) {
        err(3, "File %s does not contain only uint32_t numbers", input_file);
    }

    if ((fds[0] = open(input_file, O_RDONLY)) == -1) {
        err(4, "Could not open file %s", input_file);
    }

    int halfSizeLength = s1.st_size / 2;
    int otherSizeLength = halfSizeLength + (s1.st_size % 2);
    char temp1[11] = "tempXXXXXX";
    char temp2[11] = "tempXXXXXX";

    if ((fds[2] = mkstemp(temp1)) == -1) {
        close_all();
        err(7, "Could not open file %s", temp1);
    }
    split(fds[2], temp1, halfSizeLength);
    if ((fds[3] = mkstemp(temp2)) == -1) {
        close_all();
        err(8, "Could not open file %s", temp2);
    }
    split(fds[3], temp2, otherSizeLength);
    merge(halfSizeLength, otherSizeLength, output_file, temp1, temp2);

    close_all();
    if (unlink(temp1) == -1) {
        err(11, "Could not remove file %s", temp1);
    }
    if (unlink(temp2) == -1) {
        err(11, "Could not remove file %s", temp2);
    }
}

//Напишете програма на C, която приема два параметъра – имена на файлове:
//• примерно извикване: ./main input.bin output.bin
//• файловете input.bin и output.bin се третират като двоични файлове, състоящи се от uint32_t
//        числа
//• файлът input.bin може да съдържа максимум 4194304 числа
//• файлът output.bin трябва да бъде създаден от програмата и да съдържа числата от input.bin,
//сортирани във възходящ ред
//• endianness-ът на машината, създала файла input.bin е същият, като на текущата машина
//• ограничения на ресурси: програмата трябва да работи с употреба на максимум 9 MB RAM и
//64 MB дисково пространство.