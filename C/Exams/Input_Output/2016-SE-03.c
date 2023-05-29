#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/param.h>
#include <stdint.h>

int compareUint32_t(const void* a, const void* b);
void sortFile(const char* file_name);
void split(int halfCount, int fd, const char* temp_file, const char* source_file);
int openReadSafe(const char* file_name);
uint32_t readNumber(int fd, const char* file_name);
void merge(const char* temp1, const char* temp2, int temp1Size, int temp2Size, const char* source_file);
void writeSafe(int fd, uint32_t number, const char* file_name);

int compareUint32_t(const void* a, const void* b) {
    const uint32_t* num1 = (const uint32_t*)a;
    const uint32_t* num2 = (const uint32_t*)b;

    if (*num1 < *num2) {
        return -1;
    } else if (*num1 > *num2) {
        return 1;
    } else

        return 0;
}

int openReadSafe(const char* file_name) {
    int fd;
    if (( fd = open(file_name, O_RDONLY)) == -1) {
        err(10, "Error while opening file %s", file_name);
    }

    return fd;
}

uint32_t readNumber(int fd, const char* file_name) {
    uint32_t number;
    if (read(fd, &number, sizeof(number)) == -1) {
        err(11, "Error while reading from file %s", file_name);
    }

    return number;
}

void writeSafe(int fd, uint32_t number, const char* file_name) {
    if (write(fd, &number, sizeof(uint32_t)) == -1) {
        err(10, "Error while writing to file %s", file_name);
    }
}

void merge(const char* temp1, const char* temp2, int temp1Size, int temp2Size, const char* source_file) {
    int fd1 = openReadSafe(temp1);
    int fd2 = openReadSafe(temp2);
    int fd;

    if (( fd = open(source_file, O_RDWR | O_TRUNC)) == -1) {
        err(11, "Error while reading from file %s", source_file);
    }

    uint32_t num1 = readNumber(fd1, temp1);
    uint32_t num2 = readNumber(fd2, temp2);
    int counter = 0, minSize = MIN(temp1Size, temp2Size);

    while (counter < minSize) {
        if (num1 < num2) {
            writeSafe(fd, num1, source_file);
            dprintf(1, "%d\n", num1);
            num1 = readNumber(fd1, temp1);
        } else {
            writeSafe(fd, num2, source_file);
            dprintf(1, "%d\n", num2);
            num2 = readNumber(fd2, temp2);
        }
        counter++;
    }

    while (counter < temp1Size) {
        writeSafe(fd, num1, source_file);
        dprintf(1, "%d\n", num1);
        num1 = readNumber(fd1, temp1);
        counter++;
    }

    while (counter < temp2Size) {
        writeSafe(fd, num2, source_file);
        dprintf(1, "%d\n", num2);
        num2 = readNumber(fd2, temp2);
        counter++;
    }
}

void split(int halfCount, int fd, const char* temp_file, const char* source_file) {
    uint32_t number;
    uint32_t* half = malloc(halfCount * sizeof(uint32_t));
    if (half == NULL) {
        close(fd);
        errx(7, "Error while memory allocation");
    }
    int index = 0, bytes_count;
    while (index < halfCount && (bytes_count = read(fd, &number, sizeof(uint32_t))) == sizeof(uint32_t)) {
        half[index] = number;
        index++;
    }

    if (bytes_count == -1) {
        close(fd);
        errx(8, "Error while reading from file %s", source_file);
    }

    qsort(half, halfCount, sizeof(uint32_t), compareUint32_t);
    int fd1;
    if ((fd1 = open(temp_file, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU)) == -1) {
        free(half);
        err(9, "Error while opening file %s", source_file);
    }

    for (int i = 0; i < halfCount; i++) {
        writeSafe(fd1, half[i], temp_file);
    }

    close(fd1);
    free(half);

}

void sortFile(const char* file_name) {
    struct stat st;
    if (stat(file_name, &st) == -1) {
        err(2, "Error stat file %s", file_name);
    }

    if ((st.st_size % sizeof(uint32_t)) != 0) {
        errx(3, "File %s does not contain only unit32_t numbers", file_name);
        exit(0);
    }
    ssize_t fileSize = st.st_size;
    if ((fileSize > 100000000)) {
        errx(4, "File %s contains too many numbers", file_name);
    }

    int fd = openReadSafe(file_name);
    const char *temp1 = "temp1.txt", *temp2 = "temp2.txt";
    int numbersCount = fileSize / sizeof(uint32_t);
    int halfCount = numbersCount / 2;
    int otherHalfCount = numbersCount - halfCount; // just in case it is and odd count
    split(halfCount, fd, temp1, file_name);
    split(otherHalfCount, fd, temp2, file_name);
    close(fd);

    merge(temp1, temp2, halfCount, otherHalfCount, file_name);
}

int main(int argc, char** argv){
    if (argc != 2) {
        errx(1, "Usage: %s <file_name>\n", argv[0]);
    }

    const char* file_name = argv[1];
    sortFile(file_name);
}

//Напишете програма на C приемаща параметър – име на (двоичен) файл с uint32_t
//  числа. Програмата трябва да сортира файла. Ограничения:
//• Числата биха могли да са максимум 100 000 000 на брой.
//• Програмата трябва да работи на машина със същия endianness, както машината, която е
//        създала файла.
//• Програмата трябва да работи на машина с 256 MB RAM и 8 GB свободно дисково пространство.
