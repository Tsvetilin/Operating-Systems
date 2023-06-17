#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>
#include <stdint.h>
#include <unistd.h>

struct pos {
        uint16_t pos;
        uint8_t len;
        uint8_t _;
};

int main(int argc, char** argv) {

        if(argc != 5) {
                errx(1, "ERROR: Arg count");
        }

        int dat_fd, idx_fd;

        if(( dat_fd = open(argv[1], O_RDONLY) ) == -1) {
                err(1, "ERORR: Opening %s", argv[1]);
        }

        if(( idx_fd = open(argv[2], O_RDONLY) ) == -1) {
                err(1, "ERORR: Opening %s", argv[2]);
        }

        struct stat s;
        if(fstat(idx_fd, &s) == -1) {
                err(1, "ERROR: stat");
        }

        if(s.st_size % 4 != 0) {
                errx(2, "ERROR: %s format", argv[2]);
        }

        if(fstat(dat_fd, &s) == -1) {
                err(1, "ERROR: stat");
        }
        int dat_size = s.st_size;


        int bytes_count = 0;
        struct pos pos;

        int dat_out, idx_out;
        if((dat_out = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
                err(2, "ERROR: Opening");
        }

        if((idx_out = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
                err(2, "ERROR: Opening");
        }

        uint16_t offset = 0;
        while(( bytes_count = read(idx_fd, &pos, sizeof(pos)) ) > 0) {
                if(pos.pos + pos.len > dat_size) {
                        errx(1, "Incosistent");
                }

                uint8_t buf;
                lseek(dat_fd, pos.pos, SEEK_SET);
                if(read(dat_fd, &buf, sizeof(buf)) == -1) {
                        err(3, "Reading from dat");
                }

                if(buf < 0x41 || buf > 0x5A) {
                        continue;
                }

                if(write(dat_out, &buf, sizeof(buf)) == -1) {
                        err(3, "ERROR: Writing to dat");
                }

                struct pos out_pos;
                out_pos.pos = offset;
                out_pos.len = pos.len;
                out_pos._ = 0;

                if(write(idx_out, &out_pos, sizeof(out_pos)) == -1) {
                        err(3, "ERROR: Writing to idx");
                }

                for(int i = 0; i < pos.len - 1; ++i) {
                        if(read(dat_fd, &buf, sizeof(buf)) == -1) {
                                err(3, "Reading from dat");
                        }

                        if(write(dat_out, &buf, sizeof(buf)) == -1) {
                                err(3, "ERROR: Writing to dat");
                        }
                }

                offset += pos.len;

        }

        if(bytes_count == -1) {
                err(1, "ERROR: Read");
        }

        close(idx_fd);
        close(dat_fd);
        close(dat_out);
        close(idx_out);

        return 0;
}

#v2


#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>

int fds[4];

typedef struct __attribute__((packed)) {
    uint16_t offset;
    uint8_t length;
    uint8_t miss_byte;
} triplet;

void close_all(void);
int open_safe_read(const char* file_name);
int open_safe_create(const char* file_name);
void lseek_safe(int fd, off_t offset, int whence);
bool starts_with_capital(const char* buff);
void write_safe_word(int fd, const char* word);
void write_safe_struct(int fd, const triplet tr);

void write_safe_word(int fd, const char* word) {
    if (write(fd, word, strlen(word)) != (ssize_t)strlen(word)) {
        close_all();
        errx(8, "Could not write to file");
    }
}

void write_safe_struct(int fd, const triplet tr) {
    if (write(fd, &tr, sizeof(tr)) != sizeof(tr)) {
        close_all();
        errx(8, "Could not write to file");
    }
}

bool starts_with_capital(const char* buff) {
    return buff[0] >= 'A' && buff[0] <= 'Z';
}
void lseek_safe(int fd, off_t offset, int whence) {
    if (lseek(fd, offset, whence) == -1) {
        close_all();
        err(4, "Could not lseek");
    }
}

int open_safe_create(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) == -1) {
        close_all();
        err(2, "Could not open file %s", file_name);
    }

    return fd;
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

int open_safe_read(const char* file_name) {
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    return fd;
}

int main(int argc, char** argv) {
    if (argc != 4) {
        errx(1, "Invalid agruments. Usage: %s <f1.dat> <f1.idx> <f2.dat> <f2.idx>", argv[0]);
    }

    const char* source_dat = argv[1], *source_idx = argv[2], *output_dat = argv[3], *output_idx = argv[4];
    fds[0] = open_safe_read(source_dat);
    fds[1] = open_safe_read(source_idx);
    fds[2] = open_safe_create(output_dat);
    fds[3] = open_safe_create(output_idx);

    char buff[4096];
    struct stat st;
    if (stat(source_dat, &st) == -1) {
        close_all();
        err(3, "Could not stat file %s", source_dat);
    }

    triplet tr;
    int bytes_count, offset = 0;
    while((bytes_count = read(fds[1], &tr, sizeof(tr)) > 0)) {
        if (tr.offset + tr.length > st.st_size) {
            close_all();
            errx(7, "Invalid word position or length exceed file size");
        }

        lseek_safe(fds[0], tr.offset, SEEK_SET);
        if (read(fds[0], &buff, tr.length) == -1) {
            close_all();
            err(5, "Could not read from file %s", source_idx);
        }
        buff[tr.length] = '\0';
        if (starts_with_capital(buff)) {
            write_safe_word(fds[2], buff);
            triplet new_tr;
            new_tr.offset = offset;
            new_tr.length = tr.length;
            new_tr.miss_byte = 0;
            offset += new_tr.length;
            write_safe_struct(fds[3], new_tr);
        }

    }

    if (bytes_count == -1) {
        close_all();
        err(5, "Could not read from file %s", source_idx);
    }

    close_all();
    exit(0);
}
