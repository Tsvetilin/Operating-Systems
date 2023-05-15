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