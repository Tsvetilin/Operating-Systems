#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        errx(1, "Expected 4 arguments");
    }

    const char* f1 = argv[1];
    const char* f2 = argv[2];
    struct stat s1;
    struct stat s2;

    if (stat(f1, &s1) == -1) {
        err(2, "Error while stat file %s", f1);
    }

    if (stat(f2, &s2) == -1) {
        err(3, "Error while stat file %s", f2);
    }

    uint32_t size1 = s1.st_size;
    uint32_t size2 = s2.st_size;

    if (size1 != size2) {
        errx(4, "files are not consistent %s, %s", f1, f2);
    }

    struct triple_t{
        uint16_t offset;
        uint8_t b1;
        uint8_t b2;
    }__attribute__((packed)); // Best possible alignment (i.e. remove padding)

    int fd1 = open(f1, O_RDONLY);
    int fd2 = open(f2, O_RDONLY);

    if (fd1 == -1) {
        errx(5, "ERROR: while opening %s", f1);
    }


    if (fd2 == -1) {
        const int _errno = errno;
        close(fd1);
        errno = _errno;
        errx(6, "ERROR: while opening file %s", f2);
    }

    const char* f3 = argv[3];
    int fd3 = open(f3, O_APPEND | O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

    if (fd3 == -1) {
        const int _errno = errno;
        close(fd1);
        close(fd2);
        errno = _errno;
        err(7, "ERROR: while opening file %s", f3);
    }

    ssize_t rd1 = -1;
    ssize_t rd2 = -1;
    struct triple_t p;

    for (p.offset = 0; p.offset < size1; p.offset++){
        if ( (rd1 = read(fd1, &p.b1, sizeof(p.b1))) != sizeof(p.b1)){
            close(fd1);
            close(fd2);
            close(fd3);
            errx(7, "ERROR: while reading file %s", f1);
        }

        if ( (rd2 = read(fd2, &p.b2, sizeof(p.b2))) != sizeof(p.b2)){
            close(fd1);
            close(fd2);
            close(fd3);
            errx(7, "ERROR: while reading file %s", f2);
        }

        if (p.b1 != p.b2){
            ssize_t wr = write(fd3, &p, sizeof(p));
            if ( wr == -1 || wr != sizeof(p)) {
                const int _errno = errno;
                close(fd1);
                close(fd2);
                close(fd3);
                errno = _errno;
                errx(8, "ERROR: while writing in %s", f3);
            }
        }
    }

    if (rd1 == -1 || rd2 == -1){
        const int _errno = errno;
        close(fd1);
        close(fd2);
        close(fd3);
        errno = _errno;
        errx(9, "ERROR: while reading from file %s and %s", f1, f2);
    }

    close(fd1);
    close(fd2);
    close(fd3);

    exit(0);
}

//v2

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>

int fds[3];

typedef struct __attribute__((packed)) {
    uint16_t offset;
    uint8_t original_byte;
    uint8_t new_byte;
} triplet;

int open_read(const char* file_name);
int open_create(const char* file_name);
void write_safe(int fd, const triplet t);
void close_all(void);

void write_safe(int fd, const triplet t) {
    if (write(fd, &t, sizeof(t)) != sizeof(t)) {
        close_all();
        errx(6, "Could not write to file");
    }
}

int open_read(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        close_all();
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

int open_create(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        close_all();
        err(2, "Could not open file %s", file_name);
    }

    return fd;
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

int main(int argc, char** argv) {
    if (argc != 4) {
        errx(1, "Invalid arguments. Usage: %s <f1> <f2> <patch>", argv[0]);
    }

    const char* f1 = argv[1], *f2 = argv[2], *patch = argv[3];
    struct stat s1, s2;
    if (stat(f1, &s1) == -1 || stat(f2, &s2) == -1) {
        err(3, "Could not stat");
    }

    if (s1.st_size != s2.st_size) {
        errx(4, "Inconsistent file content");
    }

    fds[0] = open_read(f1);
    fds[1] = open_read(f2);
    fds[2] = open_create(patch);

    triplet t;

    uint8_t original_byte, new_byte;
    int bytes_count1, bytes_count2;
    uint16_t offset = 0;

    while (((bytes_count1 = read(fds[0], &original_byte, sizeof(original_byte))) == sizeof(original_byte)) &&
           (bytes_count2 = read(fds[1], &new_byte, sizeof(new_byte)) == sizeof(new_byte))) {
        if (original_byte != new_byte) {
            t.original_byte = original_byte;
            t.new_byte = new_byte;
            t.offset = offset;
            write_safe(fds[2], t);
        }
        offset++;
    }

    if (bytes_count1 == -1 || bytes_count2 == -1) {
        close_all();
        err(9, "Could not read from file");
    }

    close_all();
    exit(0);
}

//Напишете програма на C, която приема три параметъра – имена на двоични файлове.
//Примерно извикване: $ ./main f1.bin f2.bin patch.bin
//        Файловете f1.bin и f2.bin се третират като двоични файлове, състоящи се от байтове (uint8_t).
//Файлът f1.bin e “оригиналният” файл, а f2.bin е негово копие, което е било модифицирано по някакъв начин (извън обхвата на тази задача).
//Файлът patch.bin е двоичен файл, състоящ се от наредени тройки от следните елементи (и техните типове): •
//отместване (uint16_t) – спрямо началото на f1.bin/f2.bin
//• оригинален байт (uint8_t) – на тази позиция в f1.bin • нов байт (uint8_t) – на тази позиция в f2.bin 18
//Вашата програма да създава файла patch.bin, на базата на съществуващите файлове f1.bin и f2.bin, като описва вътре само разликите между двата файла.
//Ако дадено отместване съществува само в единия от файловете f1.bin/f2.bin, програмата да прекратява изпълнението си по подходящ начин.
//
//Примерен f1.bin:
//00000000: f5c4 b159 cc80 e2ef c1c7 c99a 2fb0 0d8c ...Y......../...
//00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
//00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
//00000030: 35af 6be6 5a71 b23a 0e8d 08de def2 214c 5.k.Zq.:......!L
//
//        Примерен f2.bin:
//00000000: f5c4 5959 cc80 e2ef c1c7 c99a 2fb0 0d8c ..YY......../...
//00000010: 3c83 6fed 6b46 09d2 90df cf1e 9a3c 1f05 <.o.kF.......<..
//00000020: 05f9 4c29 fd58 a5f1 cb7b c9d0 b234 2398 ..L).X...{...4#.
//00000030: afaf 6be6 5a71 b23a 0e8d 08de def2 214c ..k.Zq.:......!L
//
//        Примерен patch.bin:
//00000000: 0200 b159 3000 35af ...Y0.5.
