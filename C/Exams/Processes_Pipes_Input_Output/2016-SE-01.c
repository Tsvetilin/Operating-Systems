#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>

void close_safe(int fd);

void close_safe(int fd) {
    int errno_ = errno;
    close(fd);
    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Invalid arguments. Usage: %s <file_name>", argv[0]);
    }

    const char *file_name = argv[1];
    int a[2];

    if (pipe(a) == -1) {
        err(4, "Pipe failed");
    }

    const pid_t child1 = fork();
    if (child1 == -1) {
        err(3, "Fork failed");
    }

    if (child1 == 0) {
        close(a[0]);
        if (dup2(a[1], 1) == -1) {
            close_safe(a[1]);
            err(5, "Dup failed");
        }

        if (execlp("cat", "cat", file_name, (char *)NULL) == -1) {
            err(6, "Cat execlp failed");
        }
    }

    close(a[1]);
    int status;

    if (wait(&status) == -1) {
        close_safe(a[0]);
        err(8, "Could not wait for child process to finish");
    }

    if (!WIFEXITED(status)) {
        close_safe(a[0]);
        errx(9, "Child process did not terminate normally");
    }

    if (WEXITSTATUS(status) != 0) {
        close_safe(a[0]);
        errx(10, "Child process finished with exit code not 0");
    }

    if (dup2(a[0], 0) == -1) {
        err(5, "Dup failed");
    }

    if (execlp("sort", "sort", (char *)NULL) == -1) {
        err(7, "Sort failed");
    }

    exit(0);
}

//Напишете програма на C, която по подадено име на (текстови) файл като параметър,
//извежда съдържанието на файла сортирано, чрез употреба на външните програми cat и sort през
//pipe().