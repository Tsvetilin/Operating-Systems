#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <sys/wait.h>

//find DIR_NAME -mindepth 1 -type f -printf "%T@ %f\n" | sort -nr | head -n1 | cut -f2 -d ' '

void check_child_process(void);

void check_child_process(void) {
    int status;
    if (wait(&status) == -1) {
        err(5, "Could not wait for child process to finish");
    }

    if (!WIFEXITED(status)) {
        errx(6, "Child process did not terminate normally");
    }

    if (WEXITSTATUS(status) != 0) {
        errx(7, "Child process exit status not 0");
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Invalid arguments. Usage: %s <dir_name>", argv[0]);
    }

    const char* dir_name = argv[1];
    int a[2];
    if (pipe(a) == -1) {
        err(2, "Could not pipe");
    }

    pid_t child = fork();
    if (child == -1) {
        err(3, "Could not fork");
    }

    if (child == 0) {
        close(a[0]);
        if (dup2(a[1], 1) == -1) {
            err(7, "Could not dup2");
        }

        if (execlp("find", "find", dir_name, "-mindepth", "1", "-type", "f", "-printf", "%T@ %f\n", (char *)NULL) == -1) {
            err(4, "Could not execute find");
        }
    }

    close(a[1]);
    check_child_process();

    int b[2];
    if (pipe(b) == -1) {
        err(2, "Could not pipe");
    }

    if ((child = fork()) == -1) {
        err(3, "Could not fork");
    }

    if (child == 0) {
        close(b[0]);
        if (dup2(a[0], 0) == -1 || dup2(b[1], 1) == -1) {
            err(7, "Could not dup2");
        }
        if (execlp("sort", "sort", "-nr", (char*)NULL) == -1) {
            err(4, "Could not execute sort");
        }
    }

    close(b[1]);
    close(a[0]);
    check_child_process();

    int c[2];
    if (pipe(c) == -1) {
        err(2, "Could not pipe");
    }

    if ((child = fork()) == -1) {
        err(3, "Could not fork");
    }

    if (child == 0) {
        close(c[0]);
        if (dup2(b[0], 0) == -1 || dup2(c[1], 1) == -1) {
            err(7, "Could not dup2");
        }
        if (execlp("head", "head", "-n1", (char*)NULL) == -1) {
            err(4, "Could not execute head");
        }
    }

    close(c[1]);
    close(b[0]);
    check_child_process();

    if (dup2(c[0], 0) == -1) {
        err(7, "Could not dup2");
    }

    if (execlp("cut", "cut", "-f2", "-d", " ", (char*)NULL) == -1) {
        err(4, "Could not execute cut");
    }
}

//Напишете програма на C, която приема параметър – име на директория. Програмата
//трябва да извежда името на най-скоро променения (по съдържание) файл в тази директория и
//нейните под-директории, чрез употреба на външни шел команди през pipe().