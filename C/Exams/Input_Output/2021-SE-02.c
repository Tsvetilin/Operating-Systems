#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>

int fds[2];
const int BUFF_SIZE = 4096;

void close_all(void);
bool get_bit(uint16_t x, int idx);
uint8_t set_bit(uint8_t x, int idx, bool bit);
uint8_t manchester_decode_byte(uint16_t x);
void manchester_decode_bytes(uint16_t* in_buf, uint8_t* out_buf, int n);

void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 2; i++) {
        if (fds[i] >= 0) {
            close(fds[i]);
        }
    }

    errno = errno_;
}

bool get_bit(uint16_t x, int idx) {
    return ((x & (1 << idx)) != 0);
}

uint8_t set_bit(uint8_t x, int idx, bool bit) {
    if (bit) {
        return (1 << idx) | x;
    } else {
        return (~(1 << idx)) & x;
    }
}

uint8_t manchester_decode_byte(uint16_t x) {
    uint8_t result = 0;

    for (int i = 0; i < 16; i += 2) {
        bool first = get_bit(x, i);
        bool second = get_bit(x, i + 1);

        if (!first && second) {
            result = set_bit(result, i / 2, 0);
        } else if (first && !second) {
            result = set_bit(result, i / 2, 1);
        } else {
            errx(3, "Invalid Sequence");
        }
    }

    return result;
}

void manchester_decode_bytes(uint16_t* in_buf, uint8_t* out_buf, int n) {
    for (int i = 0; i < n; i++) {
        out_buf[i] = manchester_decode_byte(in_buf[i]);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        warnx("%s encodes a file to manchester encoding", argv[0]);
        errx(1, "Usage: %s <input file> <output file>", argv[0]);
    }

    const char *file_in = argv[1], *file_out = argv[2];
    if ((fds[0] = open(file_in, O_RDONLY)) == -1) {
        err(1, "Could not open file %s", file_in);
    }
    if ((fds[1] = open(file_out, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        close_all();
        err(1, "Could not open file %s", file_out);
    }

    uint16_t in_buf[BUFF_SIZE];
    uint8_t out_buf[BUFF_SIZE];

    int bytes_read;
    while((bytes_read = read(fds[0], in_buf, sizeof(in_buf))) > 0) {
        if (bytes_read % 2 != 0) {
            close_all();
            errx(2, "We read an odd number of bytes");
        }

        manchester_decode_bytes(in_buf, out_buf, bytes_read / 2);
        if (write(fds[1], out_buf, bytes_read / 2) != (bytes_read / 2)) {
            close_all();
            errx(1, "Could not write all data to file %s", argv[2]);
        }
    }

    if (bytes_read < 0) {
        close_all();
        err(4, "Could not read data from %s", file_in);
    }

    close_all();
    exit(0);
}

//Вашите колеги от съседната лаборатория са написали програма на C, която може да обработва подаден входен двоичен файл
//и на негова база генерира изходен двоичен файл.
//Програмата работи като encoder, който имплементира вариант на Manchester code, т.е.:
//• за всеки входен бит 1 извежда битовете 10, и
//• за всеки входен бит 0 извежда битовете 01
//Например следните 8 бита вход
//1011 0110 == 0xB6
//по описания алгоритъм дават следните 16 бита изход
//1001 1010 0110 1001 == 0x9A69
//Напишете програма на C, която извършва обратния процес, т.е., декодира файлове, създадени от
//        горната програма.
//Примерно извикване:
//$ ./main input.bin output.bin
