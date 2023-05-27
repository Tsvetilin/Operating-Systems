#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

int pf[2];

void close_all(void);
void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < 2; i++) {
        if (pf[i] >= 0) {
            close(pf[i]);
        }
    }

    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Invalid number of arguments. Usage: %s <string>", argv[0]);
    }

    const char *str = argv[1];
    int str_size = strlen(str);
    if(pipe(pf) == -1) {
        err(1, "Could not create pipe");
    }

    const pid_t child_pid = fork();
    if (child_pid == -1) {
        close_all();
        err(2, "Could not fork.");
    }

    if(child_pid == 0) {
        close(pf[1]);
        dup2(pf[0], 0);
        if (execlp("wc", "wc", "-c", (char *)NULL) == -1) {
            close_all();
            err(3, "Could not execlp.");
        }
    }

    close(pf[0]);
    if (write(pf[1], str, str_size) != str_size) {
        close_all();
        err(4, "Error while writing");
    }

    close(pf[1]);
    wait(NULL);
    exit(0);
}


//T3 - Напишете програма на C, която демонстрира употребата на dup/dup2 между parent и child процеси.
//Parent-ът трябва да изпраща стринга, получен като първи аргумент на командния ред към child-а,
//където той да може да се чете от stdin. Child процесът да изпълняваwc -c.