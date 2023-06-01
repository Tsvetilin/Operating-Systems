#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

int fds[2];

int open_safe(const char* file_name, int flags, mode_t mode);
void close_all(void);
void load_arr(uint16_t* numbers, const char* file_name);
int comp(const void* a, const void* b);
void load_file(uint16_t* numbers, int numbers_count,  const char* file_name);

int comp(const void* a, const void* b) {
    const uint16_t* first = (const uint16_t*)a;
    const uint16_t* second = (const uint16_t*)b;

    if(*first < *second) {
        return -1;
    } else if (*first > *second) {
        return 1;
    }

    return 0;
}

void load_arr(uint16_t* numbers, const char* file_name) {
    uint16_t number;
    int bytes_count, index = 0;
    while((bytes_count = read(fds[0], &number, sizeof(number))) == sizeof(number)) {
        numbers[index] = number;
        index++;
    }

    if (bytes_count == -1) {
        close_all();
        err(4, "Could not read from %s", file_name);
    }
}

void load_file(uint16_t* numbers, int numbers_count,  const char* file_name) {
    for (int i = 0; i < numbers_count; i++) {
        if (write(fds[1], &numbers[i], sizeof(numbers[i])) == -1) {
            close_all();
            err(5, "Could not write to %s", file_name);
        }
    }
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

int open_safe(const char* file_name, int flags, mode_t mode) {
    int fd;
    if ((fd = open(file_name, flags, mode)) == -1) {
        close_all();
        err(1, "Could not open file %s", file_name);
    }

    return fd;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage %s <input_file> <output_file>", argv[0]);
    }

    const char *input_file = argv[1], *output_file = argv[2];

    struct stat s1;

    if(stat(input_file, &s1) == -1) {
        err(2, "Could not stat %s", input_file);
    }

    if(s1.st_size % 2 != 0) {
        errx(3, "File %s does not contain uint16_t numbers only", input_file);
    }

    fds[0] = open_safe(input_file, O_RDONLY, S_IRUSR | S_IWUSR);
    fds[1] = open_safe(output_file, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    int numbers_count = s1.st_size / sizeof(uint16_t);
    uint16_t* numbers = malloc(numbers_count * sizeof(uint16_t));
    load_arr(numbers, input_file);
    qsort(numbers, numbers_count, sizeof(uint16_t), comp);
    load_file(numbers, numbers_count, output_file);
    close_all();
    free(numbers);
    exit(0);
}

//Напишете програма на C, която приема два параметъра – имена на файлове:
//• примерно извикване: ./main input.bin output.bin
//• файловете input.bin и output.bin се третират като двоични файлове, състоящи се от uint16_t
//        числа
//• файлът input.bin може да съдържа максимум 65535 числа
//• файлът output.bin трябва да бъде създаден от програмата и да съдържа числата от input.bin,
//сортирани във възходящ ред
//• endianness-ът на машината, създала файла input.bin е същият, като на текущата машина
//• ограничения на ресурси: програмата трябва да работи с употреба на максимум 256 KB RAM
//и 2 MB дисково пространство.