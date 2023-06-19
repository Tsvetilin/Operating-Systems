#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>

int fds[3];

uint16_t read_num(int fd, const char* file_name);
int open_read(const char* file_name);
int open_create(const char* file_name);
void write_safe(int fd, uint16_t num, const char* file_name);
void close_all(void);

void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 3; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

void write_safe(int fd, uint16_t num, const char* file_name) {
    int bytes_written;
    if ((bytes_written = write(fd, &num, sizeof(num))) != sizeof(num)) {
        if (bytes_written == -1) {
            err(5, "Could not write to file %s", file_name);
        } else {
            errx(5, "Could not write to file %s", file_name);
        }
    }
}

int open_create(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

int open_read(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

uint16_t read_num(int fd, const char* file_name) {
    int bytes_read;
    uint16_t b;

    if ((bytes_read = read(fd, &b, sizeof(b))) != sizeof(b)) {
        if (bytes_read == -1) {
            err(4, "Could not read from file %s", file_name);
        } else {
            errx(4, "Could not read from file %s", file_name);
        }
    }

    return b;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <SCL> <SDL>", argv[0]);
    }

    const char *SCL = argv[1], *SDL = argv[2], *result = "result.txt";
    struct stat s1, s2;

    if (stat(SCL, &s1) == -1 || stat(SDL, &s2) == -1) {
        err(2, "Could not stat");
    }

    if (s1.st_size * 8 != (s2.st_size / 2)) {
        errx(3, "Incosistent file content");
    }

    fds[0] = open_read(SCL);
    fds[1] = open_read(SDL);
    fds[2] = open_create(result);

    int bytes_read;
    uint8_t b;
    while ((bytes_read = read(fds[0], &b, sizeof(b))) == sizeof(b)) {
        for (int i = 7; i >= 0; i--) {
            uint16_t num = read_num(fds[1], SDL);
            if ((b >> i) & 1) {
                write_safe(fds[2], num, result);
            }
        }
    }

    if (bytes_read == -1) {
        err(4, "Could not read from file %s", SCL);
    }
}

//Инженерите от съседната лабораторя работят с комплекти SCL/SDL файлове, напр.
//input.scl/input.sdl.
//В SCL файла са записани нива на сигнали (ниско 0 или високо 1), т.е., файлът се третира като състоящ се от битове.
//В SDL файла са записани данни от тип uint16_t, като всеки елемент съответства
//позиционно на даден бит от SCL файла.
//Помогнете на колегите си, като напишете програма на C, която да бъде удобен инструмент за
//        изваждане в нов файл само на тези SDL елементи, които са имали високо ниво в SCL файла,
//запазвайки наредбата им.