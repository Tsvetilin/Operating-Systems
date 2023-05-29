
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <err.h>
#include <string.h>


void read_fd(int, uint8_t);
void read_fd(int fd, uint8_t line) {
        uint8_t sym, line_count = 1;
        int bytes_count;

        if ( (write(1, &line_count, sizeof(line_count))) == -1) {
                err(1, "ERR: writing");
        }

        while ((bytes_count = read(fd, &sym, sizeof(sym))) > 0) {
                if ( (write(1, &sym, sizeof(sym))) == -1) {
                        err(1, "ERR: writing");
                }
                if (line && sym == '\n') {
                        line_count++;
                        dprintf(1, "%d: ", line_count);
                }
        }

        if (bytes_count == -1) {
                err(2, "ERR: reading");
        }
}

int main(int argc, char** argv) {
        if (argc == 1) {
                read_fd(0, 0);
        } else if (argc > 1) {
                uint8_t count_line = 0;
                if (strcmp(argv[1], "-n") == 0) {
                        count_line = 1;
                        if (argc == 2) {
                                read_fd(0, count_line);
                        }
                }
                for (int i = 1+count_line; i < argc; i++) {
                        int fd;
                        if ( strcmp(argv[i], "-") != 0) {
                            fd = open(argv[i], O_RDONLY);
                                if (fd == -1) {
                                        err(3, "ERR: opening file");
                                }
                        } else {
                                fd = 0;
                        }
                        read_fd(fd, count_line);
                }
        }
}


// Напишете програма на C, която да работи подобно на командата cat, реализирайки
// само следната функционалност:
// • общ вид на изпълнение: ./main [OPTION] [FILE]...
// • ако е подаден като първи параметър -n, то той да се третира като опция, което кара програмата ви да номерира (глобално) всеки изходен ред (започвайки от 1).
// • програмата извежда на STDOUT
// • ако няма подадени параметри (имена на файлове), програмата чете от STDIN
// • ако има подадени параметри – файлове, програмата последователно ги извежда
// • ако някой от параметрите е тире (-), програмата да го третира като специално име за STDIN
// Примерно извикване:
// $ cat a.txt
// a1
// a2
// $ cat b.txt
// b1
// b2
// b3
// $ echo -e "s1\ns2" | ./main -n a.txt - b.txt
// 1 a1
// 2 a2
// 3 s1
// 4 s2
// 5 b1
// 6 b2
// 7 b3

// Забележка: Погледнете setbuf(3) и strcmp(3).
