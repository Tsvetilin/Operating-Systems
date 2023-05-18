
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Expected 2 args");
    }

    const char* file_name = argv[1];
    const char* content = "foobar";
    int fd;

    if(( fd = open(file_name, O_RDWR | O_TRUNC | O_CREAT, S_IRWXU)) == -1) {
        err(2, "Error opening file %s", file_name);
    }

    pid_t child_pid = fork();

    if (child_pid == -1) {
        err(3, "Error, could not fork process.");
    }

    if (child_pid == 0) {
        if (write(fd, content, strlen(content)) == -1) {
            const int errno_ = errno;
            close(fd);
            errno = errno_;
            err(4, "Error writing in file %s", file_name);
        }

        close(fd);
        exit(0);

    }

    int status;

    if (wait(&status) == -1) {
        const int errno_ = errno;
        close(fd);
        errno = errno_;
        err(5, "could not wait child process");
    }

    if(!WIFEXITED(status)) {
        const int errno_ = errno;
        close(fd);
        errno = errno_;
        err(6, "child process did not terminate normally");
    }

    if (WEXITSTATUS(status) != 0) {
        const int errno_ = errno;
        close(fd);
        errno = errno_;
        err(7, "Child exit code not 0");
    }

    fd = open(file_name, O_RDONLY);
    if (fd == -1) {
        const int errno_ = errno;
        close(fd);
        errno = errno_;
        err(8, "Error while opening file %s", file_name);
    }


    char buf[2];
    int bytes_read;
    lseek(fd, 0, SEEK_SET);
    while ((bytes_read = read(fd, buf, 2)) > 0) {
        write(1, buf, 2);
        write(1, " ", 1);
    }

    if (bytes_read == -1) {
        const int errno_ = errno;
        close(fd);
        errno = errno_;
        err(9, "Error while writing in parent process");
    }

    write(1, "\n", 1);
    close(fd);
    exit(0);
}
