#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

void close_safe(int fd);
void write_safe(int fd, const char* str, int str_size, const char* file_name);

void close_safe(int fd) {
    int errno_ = errno;
    close(fd);
    errno = errno_;
}

void write_safe(int fd, const char* str, int str_size, const char* file_name) {
    if (write(fd, str, str_size) != str_size) {
        close_safe(fd);
        err(3, "Error while writing to file %s", file_name);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Expected 2 arguments. Usage: %s <file_name>", argv[0]);
    }

    const char* file_name = argv[1];
    int fd;
    if (( fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        err(2, "Error while opening file %s", file_name);
    }

    const char *str1 = "fo", *str2 = "bar\n", *str3 = "o\n";
    write_safe(fd, str1, strlen(str1), file_name);

    pid_t child = fork();
    if (child == -1) {
        err(4, "Error while fork parent process");
    }

    if (child == 0) {
        write_safe(fd, str2, strlen(str2), file_name);
        exit(0);
    }

    int status;
    if(wait(&status) == -1) {
        close_safe(fd);
        err(5, "Could not wait for child process to finish");
    }

    if(!WIFEXITED(status)) {
        close_safe(fd);
        err(6, "Child process did not terminate normally");
    }

    if(WEXITSTATUS(status) != 0) {
        close_safe(fd);
        err(7, "Child process exited with status != 0");
    }

    write_safe(fd, str3, strlen(str3), file_name);
    close_safe(fd);
    exit(0);
}
