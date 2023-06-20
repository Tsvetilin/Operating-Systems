#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <err.h>

/*
        cut -d: -f7 /etc/passwd | sort | uniq -c | sort -n
*/

int main(void) {
    int a[2];

    if(pipe(a) == -1) {
        err(1, "pipe failed");
    }

    pid_t pid;
    if((pid = fork()) == -1) {
        err(2, "first fork failed");
    }

    if(pid == 0) {
        close(a[0]);
        //close(1);
        //dup(a[1]);
        if(dup2(a[1], 1) == -1) {
            err(3, "cut dup failed");
        }
        if(execlp("cut", "cut", "-d:", "-f7", "/etc/passwd", (char *) NULL) == -1) {
            err(4, "cut exec failed");
        }
    } else {
        close(a[1]);
    }

    int b[2];
    if(pipe(b) == -1) {
        err(4, "Second pipe failed");
    }

    if((pid = fork()) == -1) {
        err(5, "sort fork failed");
    }

    if(pid == 0) {
        close(b[0]);
        if(dup2(a[0], 0) == -1) {
            err(6, "error dup read end sort");
        }
        if(dup2(b[1], 1) == -1) {
            err(6, "error dup write end sort");
        }
        execlp("sort", "sort", (char *) NULL);
    } else {
        close(b[1]);
    }

    int c[2];

    if(pipe(c) == -1){
        err(6, "error pipe for uniq");
    }

    if((pid = fork()) == -1) {
        err(7, "error forking uniq");
    }

    if(pid == 0) {
        close(c[0]);

        if(dup2(b[0], 0) == -1) {
            err(9, "error dup read end uniq");
        }
        if(dup2(c[1], 1) == -1) {
            err(10, "error dup read end uniq");
        }

        if(execlp("uniq", "uniq", "-c", (char *)NULL) == -1) {
            err(11, "error exec uniq");
        }
    } else {
        close(c[1]);
    }

    close(a[0]);
    close(b[0]);

    while(wait(NULL) > 0);

    if(dup2(c[0], 0) == -1){
        err(11, "error dup read end sort -n");
    }

    if(execlp("sort", "sort", "-n", (char *)NULL) == -1){
        err(6, "error exec sort -n");
    }

    return 0;
}


#v2

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

int ps[3][2];

void close_all(void);
void dup2_safe(int fd1, int fd2);
void pipe_safe(int fd_num);
pid_t fork_safe(void);

pid_t fork_safe(void) {
    pid_t child = fork();
    if (child == -1) {
        close_all();
        err(2, "Could not fork");
    }

    return child;
}

void pipe_safe(int fd_num) {
    if (pipe(ps[fd_num]) == -1) {
        close_all();
        err(1, "Could not pipe");
    }
}

void close_all(void) {
    int errno_ = errno;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            if (ps[i][j] >= 0) {
                close(ps[i][j]);
            }
        }
    }

    errno = errno_;
}

void dup2_safe(int fd1, int fd2) {
    if (dup2(fd1, fd2) == -1) {
        close_all();
        err(4, "Could not dup2");
    }
}

//cut -d ':' -f7 /etc/passwd | sort | uniq -c | sort -n

int main(void) {
    pipe_safe(0);
    pid_t child = fork_safe();

    if (child == 0) {
        close(ps[0][0]);
        dup2_safe(ps[0][1], 1);
        if (execlp("cut", "cut", "-d", ":", "-f", "7", "/etc/passwd", (char*)NULL) == -1) {
            close_all();
            err(3, "Could not execlp cut");
        }
    }

    close(ps[0][1]);
    pipe_safe(1);

    child = fork_safe();

    if (child == 0) {
        close(ps[1][0]);
        dup2_safe(ps[0][0], 0);
        dup2_safe(ps[1][1], 1);
        if (execlp("sort", "sort", (char*)NULL) == -1) {
            close_all();
            err(3, "Could not execlp sort");
        }
    }

    close(ps[1][1]);
    pipe_safe(2);

    child = fork_safe();

    if (child == 0) {
        close(ps[2][0]);
        dup2_safe(ps[2][1], 1);
        dup2_safe(ps[1][0], 0);
        if (execlp("uniq", "uniq", "-c", (char*)NULL) == -1) {
            close_all();
            err(3, "Could not execlp uniq");
        }
    }

    close(ps[2][1]);
    dup2_safe(ps[2][0], 0);
    close_all();

    if (execlp("sort", "sort", "-n", (char*)NULL) == -1) {
        err(3, "Could not sort");
    }
}
//Напишете програма на C, която използвайки външни shell команди през pipe() да
//        извежда статистика за броя на използване на различните shell-ове от потребителите, дефинирани
//        в системата. Изходът да бъде сортиран във възходящ ред според брой използвания на shell-овете.
//Примерно извикване и изход:
//$ ./main
//1 /bin/sync
//3 /bin/bash
//7 /bin/false
//17 /usr/sbin/nologin