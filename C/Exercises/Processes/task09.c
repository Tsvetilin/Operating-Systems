#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

void err_handler(int, const char*, int, ssize_t);

void err_handler(int exit_code, const char* msg, int errnum, ssize_t fd) {
    if(fd != -1) close(fd);
    errno = errnum;
    err(exit_code, "error while %s", msg);
}

int main(void) {
    ssize_t fd;
    const char* file_name = "foo.txt";
    if ((fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU)) == -1) {
        err(2, "Error, while opening file foo.txt");
    }
    close(fd);

    pid_t child1_pid = fork();
    if(child1_pid == -1) {
        err_handler(2, "fork in first child", errno, -1);
    }

    if(child1_pid == 0) {
        ssize_t fd1 = open(file_name, O_WRONLY | O_APPEND);
        const char* content = "foo";
        if (fd1 == -1) {
            err_handler(3, "opening foo.txt in first child", errno, fd1);
        }
        if ((write(fd1, content, strlen(content))) == -1) {
            err_handler(4, "writing in file foo.txt", errno, fd1);
        }
        close(fd1);
        exit(0);
    }

    if(wait(NULL) == -1) {
        err_handler(5, "waiting first child", errno, -1);
    } //here we guarantee sequential performance

    pid_t child_pid2 = fork();
    if(child_pid2 == -1) {
        err_handler(6, "fork second child", errno, -1);
    }

    if(child_pid2 == 0) {
        ssize_t fd2 = open(file_name, O_WRONLY | O_APPEND);
        const char* content = "bar\n";
        if(fd2 == -1) {
            err_handler(7, "opening foo.txt in second child", errno, fd2);
        }
        if ((write(fd2, content, strlen(content))) == -1) {
            err_handler(4, "writing in file foo.txt", errno, fd2);
        }

        close(fd2);
        exit(0);
    }

    if(wait(NULL) == -1) {
        err(8, "could not wait all children processes");
    }

}

//Да се напише програма на C, която която създава файл в текущата директория и генерира два процесa,
//които записват низовете foo и bar в създадения файл.
//Програмата не гарантира последователното записване на низове.
//Променете програмата така, че да записва низовете последователно, като първия е foo.