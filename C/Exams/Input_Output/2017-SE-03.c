#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>

int fds[3];

void close_all(void);
void lseek_safe(int fd, off_t offset, int whence);
void write_safe(int fd, uint8_t byte, int size, const char* file_name);

void write_safe(int fd, uint8_t byte, int size, const char* file_name) {
    if (write(fd, &byte, size) != size) {
        close_all();
        err(4, "Could not write to file %s", file_name);
    }

}
void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 3; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

void lseek_safe(int fd, off_t offset, int whence) {
    if (lseek(fd, offset, whence) == -1) {
        close_all();
        err(7, "Could not lseek");
    }
}

typedef struct {
    uint16_t offset;
    uint8_t original_byte;
    uint8_t new_byte;
}triple;

int main(int argc, char** argv) {
    if (argc != 4) {
        errx(1, "Invalid arguments. Usage: %s <file1> <file2> <file3>", argv[0]);
    }

    const char *f1 = argv[1], *f2 = argv[2], *f3 = argv[3];

    if ((fds[0] = open(f1, O_RDONLY)) == -1) {
        err(2, "Could not open file %s" , f1);
    }
    if ((fds[1] = open(f2, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        close_all();
        err(3, "Could not open file %s", f2);
    }


    uint8_t current_byte;
    int bytes_read;
    while ((bytes_read = read(fds[0], &current_byte, sizeof(current_byte))) == sizeof(current_byte)) {
        write_safe(fds[1], current_byte, sizeof(current_byte), f2);
    }


    if (bytes_read == -1) {
        close_all();
        err(8, "Could not read from file %s", f1);
    }

    if ((fds[2] = open(f3, O_RDONLY)) == -1) {
        close_all();
        err(3, "Could not open file %s", f3);
    }

    struct stat s2;
    if (stat(f2, &s2) == -1) {
        close_all();
        err(4, "Could not stat file %s", f2);
    }

    triple t;
    while ((bytes_read = read(fds[2], &t, sizeof(t))) == sizeof(t)) {
        if (t.offset > s2.st_size) {
            close_all();
            errx(5, "Invalid offset");
        }
        lseek_safe(fds[1], t.offset, SEEK_SET);
        if (read(fds[1], &current_byte, sizeof(current_byte)) != sizeof(current_byte)) {
            close_all();
            err(6, "Error reading from file %s", f1);
        }

        if (current_byte != t.original_byte) {
            close_all();
            errx(7, "Byte %u does not exist in file %s", t.original_byte, f1);
        }

        lseek_safe(fds[1], t.offset, SEEK_SET);
        write_safe(fds[1], t.new_byte, sizeof(t.new_byte), f2);
    }

    close_all();
    exit(0);
}

//Напишете програма на C, която приема три параметъра, имена на двоични файлове.
//Примерно извикване:
//$ ./main patch.bin f1.bin f2.bin
//        Файловете f1.bin и f2.bin се третират като двоични файлове, състоящи се от байтове (uint8_t).
//Файлът patch.bin е двоичен файл, състоящ се от наредени тройки от следните елементи (и техните
//        типове):
//• отместване uint16_t
//• оригинален байт uint8_t
//• нов байт uint8_t
//        Програмата да създава файла f2.bin като копие на файла f1.bin, но с отразени промени на базата
//на файла patch.bin, при следния алгоритъм:
//• за всяка наредена тройка от patch.bin, ако на съответното отместване (в байтове) спрямо
//        началото на файла е записан байта оригинален байт, в изходния файл се записва нов байт.
//Ако не е записан такъв оригинален байт или такова отместване не съществува, програмата
//да прекратява изпълнението си по подходящ начин;
//• всички останали байтове се копират директно.
//Забележка: Наредените тройки във файла patch.bin да се обработват последователно.
//Примерен f1.bin:
//00000000: f5c4 b159 cc80 e2ef c1c7 c99a 2fb0 0d8c ...Y......../...
//00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
//00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
//00000030: 35af 6be6 5a71 b23a 0e8d 08de def2 214c 5.k.Zq.:......!L
//        Примерен patch.bin:
//00000000: 0200 b159 3000 35af ...Y0.5.
//Примерен f2.bin:
//00000000: f5c4 5959 cc80 e2ef c1c7 c99a 2fb0 0d8c ..YY......../...
//00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
//00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
//00000030: afaf 6be6 5a71 b23a 0e8d 08de def2 214c ..k.Zq.:......!L