#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>

int fds[2];

void close_all(void);
int open_read(const char* file_name);
int open_creat(const char* file_name);
uint16_t manchester_encode(uint8_t num);
void write_safe(int fd, uint16_t num, const char* file_name);

void write_safe(int fd, uint16_t num, const char* file_name) {
    int bytes_written;
    if ((bytes_written = write(fd, &num, sizeof(num))) != sizeof(num)) {
        close_all();
        if (bytes_written == -1) {
            err(4, "Could not write to file %s", file_name);
        } else {
            errx(4, "Could not write to file %s", file_name);
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

uint16_t manchester_encode(uint8_t num) {
    uint16_t result = 0;
    for (int i = 7; i >= 0; i--) {
        if (num >> i & 1) {
            result ^= (1 << (2*i +1));
        } else {
            result ^= (1 << (2*i));
        }
    }

    return result;
}


int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <input_file> <output_file>", argv[0]);
    }

    const char *input_file = argv[1], *output_file = argv[2];

    fds[0] = open_read(input_file);
    fds[1] = open_creat(output_file);


    uint8_t num;
    int bytes_read;
    while((bytes_read = read(fds[0], &num, sizeof(num))) > 0) {
        uint16_t result = manchester_encode(num);
        write_safe(fds[1], result, output_file);
    }

    if (bytes_read == -1) {
        close_all();
        err(3, "Could not read from file %s", input_file);
    }

    close_all();
    exit(0);
}

//Инженерите от съседната лабораторя ползват специализиран хардуер и софтуер за
//прехвърляне на данни по радио, но за съжаление имат два проблема:
//• в радио частта: дълги поредици битове само 0 или само 1 чупят преноса;
//• в софтуерната част: софтуерът, който ползват, може да прехвърля само файлове с четен брой
//        байтове дължина.
//Помогнете на колегите си, като напишете програма на C, която решава тези проблеми, като подготвя
//файлове за прехвърляне. Програмата трябва да приема два задължителни позиционни аргумента
//– имена на файлове. Примерно извикване:
//$ ./main input.bin output.bin
//        Програмата чете данни от input.bin и записва резултат след обработка в output.bin. Програмата
//        трябва да работи като encoder, който имплементира вариант на Manchester code, т.е.:
//• за всеки входен бит 1 извежда битовете 10, и
//• за всеки входен бит 0 извежда битовете 01
//Например, следните 8 бита вход
//1011 0110 == 0xB6
//по описаният алгоритъм дават следните 16 бита изход
//1001 1010 0110 1001 == 0x9A69