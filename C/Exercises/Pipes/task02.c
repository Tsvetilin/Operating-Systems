#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

void close_safe(int fd);

void close_safe(int fd) {
    int errno_ = errno;
    close(fd);
    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Expected 2 arguments. Usage: %s <string>", argv[0]);
    }

    int pf[2];

    if (pipe(pf) == -1) {
        err(3, "Could not create pipe");
    }

    const char* str = argv[1];
    int str_size = strlen(str);
    pid_t child = fork();

    if (child == -1) {
        err(2, "Could not fork process");
    }

    if (child == 0) {
        close(pf[1]);
        char buf;

        int bytes_count;
        while((bytes_count = read(pf[0], &buf, sizeof(buf))) == sizeof(buf)) {
            if (write(1,  &buf, sizeof(buf)) != sizeof(buf)) {
                err(4, "Error while writing to stdin");
            }
        }

        if (bytes_count == -1) {
            close_safe(pf[0]);
            err(5, "Error while reading from stdin");
        }

        close_safe(pf[0]);
        exit(0);
    }

    close_safe(pf[0]);
    if (write(pf[1], str, str_size) != str_size) {
        close_safe(pf[1]);
        err(4, "Error while writing to stdin");
    }
    close_safe(pf[1]);
    wait(NULL);

    exit(0);
}

//T2 - Напишете програма на C, която демонстрира комуникация през pipe между parent и child процеси.
//Parent-ът трябва да изпраща стринга, получен като първи аргумент на командния ред към child-а, който да го отпечатва на стандартния изход.