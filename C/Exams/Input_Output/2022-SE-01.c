#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>

int fds[2] = {-1, -1};

typedef struct __attribute__((packed)){
    uint32_t magic;
    uint32_t count;
} data_header;

typedef struct __attribute__((packed)){
    uint32_t magic1;
    uint32_t magic2;
    uint16_t reserved;
    uint64_t count;
} comparator_header;

typedef struct __attribute__((packed)) {
    uint16_t type;
    uint16_t reserved[3];
    uint32_t offset1;
    uint32_t offset2;
} comparator_data;

void close_all(void);
int open_safe(const char* file_name, int flag);
off_t stat_safe(const char* file_name, int compare);
void lseek_safe(int fd, off_t offset, int whence, int file_size, int header_size,const char* file_name);
void check_read_bytes(int bytes_read, int expected_bytes, const char* file_name);
void check_magic(uint32_t current, uint32_t expected, const char* file_name);
void check_count(int expected, int current, const char* file_name);
uint64_t read_num(int fd, const char* file_name);
void swap(int fd, uint64_t a, uint64_t b, uint32_t offset1, uint32_t offset2, uint16_t type, int file_size, int header_size, const char* file_name);
void write_safe(int fd, uint64_t num, const char* file_name);
void swap_nums(int fd, uint64_t a, uint64_t b, uint32_t offset1, uint32_t offset2, int file_size, int header_size, const char* file_name);

void write_safe(int fd, uint64_t num, const char* file_name) {
    int bytes_written;
    if ((bytes_written = write(fd, &num, sizeof(num))) != sizeof(num)) {
        check_read_bytes(bytes_written, sizeof(num), file_name);
    }
}

void swap_nums(int fd, uint64_t a, uint64_t b, uint32_t offset1, uint32_t offset2, int file_size, int header_size, const char* file_name) {
    lseek_safe(fd, offset1, SEEK_SET, file_size, header_size, file_name);
    write_safe(fd, b, file_name);
    lseek_safe(fd, offset2, SEEK_SET, file_size, header_size, file_name);
    write_safe(fd, a, file_name);
}

void swap(int fd, uint64_t a, uint64_t b, uint32_t offset1, uint32_t offset2, uint16_t type, int file_size, int header_size, const char* file_name) {
    if (type == 0) {
        if (a > b) {
            swap_nums(fd, a, b, offset1, offset2, file_size, header_size, file_name);
        }
    } else {
        if (a < b) {
            swap_nums(fd, a, b, offset1, offset2, file_size, header_size, file_name);
        }
    }
}

uint64_t read_num(int fd, const char* file_name) {
    int bytes_count;
    uint64_t num;
    if ((bytes_count = read(fd, &num, sizeof(num))) != sizeof(num)) {
        check_read_bytes(bytes_count, sizeof(num), file_name);
    }

    return num;
}

void check_count(int expected, int current, const char* file_name) {
    if (expected != current) {
        errx(8, "Invalid number of elements in file %s", file_name);
    }
}

void check_magic(uint32_t current, uint32_t expected, const char* file_name) {
    if (current != expected) {
        errx(7, "Invalid magic value in %s header", file_name);
    }
}

void check_read_bytes(int bytes_read, int expected_bytes, const char* file_name) {
    if (bytes_read == -1) {
        err(6, "Could not read from file %s", file_name);
    } else if (bytes_read != expected_bytes) {
        errx(6, "Could not read from file %s", file_name);
    }
}

void lseek_safe(int fd, off_t offset, int whence, int file_size, int header_size,const char* file_name) {
    if (offset < header_size || offset > file_size) {
        errx(5, "Invalid offset");
    }
    if (lseek(fd, offset, whence) == -1) {
        close_all();
        err(4, "Could not lseek file %s", file_name);
    }
}

int open_safe(const char* file_name, int flag) {
    int fd;
    if ((fd = open(file_name, flag)) == -1) {
        close_all();
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

off_t stat_safe(const char* file_name, int compare) {
    struct stat s;
    if (stat(file_name, &s) == -1) {
        err(3, "Could not stat file %s", file_name);
    }

    if (s.st_size % compare != 0) {
        errx(3, "Invalid content in file %s", file_name);
    }

    return s.st_size;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <data.bin> <comaprator.bin>", argv[0]);
    }

    const char *data = argv[1], *comparator = argv[1];
    off_t data_size = stat_safe(data, sizeof(data_header));
    off_t comparator_size = stat_safe(comparator, sizeof(comparator_header));

    fds[0] = open_safe(data, O_RDWR);
    fds[1] = open_safe(comparator, O_RDONLY);

    data_header d_header;
    int bytes_read = read(fds[0], &d_header, sizeof(data_header));
    check_read_bytes(bytes_read, sizeof(data_header), data);
    check_magic(d_header.magic, 0x21796F4A, data);
    check_count((data_size - sizeof(data_header) / sizeof(uint64_t)), d_header.count, data);

    comparator_header c_header;
    bytes_read = read(fds[1], &c_header, sizeof(comparator_header));
    check_read_bytes(bytes_read, sizeof(comparator_header), comparator);
    check_magic(c_header.magic1, 0xAFBC7A37, comparator);
    check_magic(c_header.magic2, 0x1C27, comparator);
    check_count((comparator_size - sizeof(comparator_header) / sizeof(comparator_data)), c_header.count, comparator);

    comparator_data c_data;
    while((bytes_read = read(fds[1], &c_data, sizeof(c_data))) == sizeof(c_data)) {
        if (c_data.type != 0 && c_data.type != 1) {
            errx(7, "Invalid type in comparator header");
        }

        for (int i = 0; i < 3; i++) {
            if (c_data.reserved[i] != 0) {
                errx(8, "Invalid reserved value in comparator header");
            }
        }

        lseek_safe(fds[0], c_data.offset1, SEEK_SET, data_size, sizeof(d_header), data);
        uint64_t a = read_num(fds[0], data);
        lseek_safe(fds[0], c_data.offset2, SEEK_SET, data_size, sizeof(d_header), data);
        uint64_t b = read_num(fds[0], data);

        if (a != b) {
            swap(fds[0], a, b, c_data.offset1, c_data.offset2, c_data.type, data_size, sizeof(d_header), data);
        }
    }

    check_read_bytes(bytes_read, sizeof(comparator_data), comparator);

    close_all();
    exit(0);
}

//Напишете програма на C, която приема два позиционни аргумента – имена на
//двоични файлове. Примерно извикване: ./main data.bin comparator.bin
//        Файлът data.bin се състои от две секции – 8 байтов хедър и данни. Структурата на хедъра е:
//• uint32_t, magic – магическа стойност 0x21796F4A, която дефинира, че файлът следва тази
//        спецификация;
//• uint32_t, count – описва броя на елементите в секцията с данни.
//Секцията за данни се състои от елементи – uint64_t числа.
//Файлът comparator.bin се състои от две секции – 16 байтов хедър и данни. Структурата на хедъра
//        е:
//• uint32_t, magic1 – магическа стойност 0xAFBC7A37;
//• uint16_t, magic2 – магическа стойност 0x1C27;
//• комбинацията от горните две magic числа дефинира, че файлът следва тази спецификация;
//• uint16_t, reserved – не се използва;
//• uint64_t, count – описва броя на елементите в секциата с данни.
//Секцията за данни се състои от елементи – наредени 6-торки:
//• uint16_t, type – възможни стойности: 0 или 1;
//• 3 бр. uint16_t, reserved – възможни стойности за всяко: 0;
//• uint32_t, offset1;
//• uint32_t, offset2.
//Двете числа offset дефинират отместване (спрямо N0) в брой елементи за data.bin; type дефинира
//операция за сравнение:
//• 0: “по-малко”;
//• 1: “по-голямо”.
//Елементите в comparator.bin дефинират правила от вида:
//• “елементът на offset1” трябва да е “по-малък” от “елементът на offset2”;
//• “елементът на offset1” трябва да е “по-голям” от “елементът на offset2”.
//Програмата трябва да интепретира по ред дефинираните правила в comparator.bin и ако правилото
//не е изпълнено, да разменя in-place елементите на съответните отмествания. Елементи, които са
//равни, няма нужда да се разменят.

