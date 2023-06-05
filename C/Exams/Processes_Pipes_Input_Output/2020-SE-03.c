#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>

int fds[2];

struct triple {
    char file_name[8];
    uint32_t offset;
    uint32_t length;
};

void close_all(void);
int open_safe(const char* file_name);
void lseek_safe(int fd, uint32_t offset, int whence);
uint16_t read_num(int fd);

void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 2; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

uint16_t read_num(int fd) {
    int bytes_read;
    uint16_t num;
    if ((bytes_read = read(fd, &num, sizeof(num))) == -1) {
        close_all();
        err(4, "Error while reading");
    }

    return num;
}

int open_safe(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

void lseek_safe(int fd, uint32_t offset, int whence) {
    if (lseek(fd, offset, whence) == -1) {
        close_all();
        err(3, "lseek failed");
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Invalid arguments. Usage %s <file_name>", argv[0]);
    }

    const char* file_name = argv[1];

    fds[0] = open_safe(file_name);

    struct stat st;
    if (stat(file_name, &st) == -1) {
        err(4, "Could not stat file %s", file_name);
    }

    if (st.st_size % sizeof(struct triple) != 0 || (long unsigned int)st.st_size > (sizeof(struct triple) * 8)) {
        errx(5, "Invalid content");
    }

    int triples_count = st.st_size / sizeof(struct triple);
    int pd[2];
    if (pipe(pd) == -1) {
        err(8, "Pipe failed");
    }

    for (int j = 1; j <= triples_count; j++) {
        struct triple t;
        int read_bytes = read(fds[0], &t, sizeof(t));
        if (read_bytes == -1) {
            close_all();
            err(6, "Err while reading from file %s.", file_name);
        }

        const pid_t child = fork();
        if (child == -1) {
            close_all();
            err(7, "Fork failed.");
        }

        if (child == 0) {
            close(pd[0]);
            fds[1] = open_safe(t.file_name);
            lseek_safe(fds[1], t.offset * sizeof(uint16_t), SEEK_SET);
            uint16_t a = 0x0000;
            uint16_t num;
            for (int i = 0; i < (int)t.length; i++) {
                num = read_num(fds[1]);
                a = a ^ num;
            }

            if (write(pd[1], &a, sizeof(a)) == -1) {
                close_all();
                err(8, "Error while writing in pipe.");
            }

            exit(0);
        }
    }
    close(pd[1]);

    uint16_t a = 0x0000;
    uint16_t num;
    for (int i = 0; i < triples_count; i++) {
        num = read_num(pd[0]);
        a = a ^ num;
    }

    dprintf(1, "%dB", a);

    exit(0);
}

//Напишете програма на C, която приема един задължителен позиционен параметър
//- име на файл. Файлът се състои от не повече от 8 наредени тройки елементи:
//• име на файл – точно 8 байта, последният от които задължително е 0x00. Ако името е по-късо
//        от 7 знака, излишните байтове са 0x00;
//• offset – uint32_t, който дава пореден номер на елемент (спрямо N0) във файла;
//• length – uint32_t, който дава брой елементи.
//За всяка наредена тройка програмата трябва да пусне child процес, който да XOR-не (обработи с
//        изключващо-или) елементите (uint16_t) от съответния файл един със друг, и да върне резултата
//        на parent процеса, който от своя страна трябва да XOR-не всички получените резултати и да изведе
//        полученото число в следния формат:
//result: 573B
//Забележка: С пълен брой точки се оценяват решения, в които child процесите работят паралелно