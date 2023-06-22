
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

int pipes_count = 2;
int pds[2][2];

void check_args(const char* argv1, const char* argv2, const char* program);
void write_safe(int fd, const char* str);
void pipe_safe(int pipe_ind);
void wait_child(void);
void handle_errx(int exit_code, const char* message);
void handle_err(int exit_code, const char* message);
void close_all(void);
pid_t fork_safe(void);
void create_pipes(void);
void read_pipe(int fd);

void read_pipe(int fd) {
    char c;
    if (read(fd, &c, sizeof(c)) == -1) {
        handle_err(9, "Could not read from pipe");
    }
}

void create_pipes(void) {
    for (int i = 0; i < pipes_count; i++) {
        pipe_safe(i);
    }
}

void close_all(void) {
    int errno_ = errno;
    for (int i = 0; i < pipes_count; i++) {
        for (int j = 0; j < 2; j++) {
            if (pds[i][j] >= 0) {
                close(pds[i][j]);
            }
        }
    }

    errno = errno_;
}

void handle_errx(int exit_code, const char* message) {
    close_all();
    errx(exit_code, message);
}
void handle_err(int exit_code, const char* message) {
    close_all();
    err(exit_code, message);
}

void wait_child(void) {
    int status;
    if (wait(&status) == -1) {
        handle_err(6, "Could not wait for child process to finish");
    }

    if (!WIFEXITED(status)) {
        handle_errx(7, "Child process did not terminate normally");
    }

    if (WEXITSTATUS(status) != 0) {
        handle_errx(8, "Child process finished with exit code not 0");
    }
}

void pipe_safe(int pipe_ind) {
    if (pipe(pds[pipe_ind]) == -1) {
        handle_err(5, "Could not pipe");
    }
}

pid_t fork_safe(void) {
    const pid_t child = fork();
    if (child == -1) {
        handle_err(4, "Could not fork\n");
    }

    return child;
}

void write_safe(int fd, const char* str) {
    int str_length = strlen(str);
    if (write(fd, str, str_length) != str_length) {
        handle_err(3, "Could not write to stdout");
    }
}

void check_args(const char* argv1, const char* argv2, const char* program) {
    if (strlen(argv1) != 1 || strlen(argv2) != 1) {
        errx(2, "Invalid arguments. Usage %s N D", program);
    }

    char num1 = argv1[0], num2 = argv2[0];

    if (!(num1 <= '9' && num1 >= '0') || !(num2 <= '9' && num2 >= '0')) {
        errx(2, "Invalid arguments. Usage %s N D", program);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage %s N D", argv[0]);
    }

    check_args(argv[1], argv[2], argv[0]);
    int N = (int)argv[1][0] - '0', D = (int)argv[2][0] - '0';
    create_pipes();
    const pid_t child = fork_safe();

    if (child == 0) {
        close(pds[1][0]);
        close(pds[0][1]);
    } else {
        close(pds[0][0]);
        close(pds[1][1]);
    }

    for (int i = 1; i <= N; i++) {
        if (child == 0) {
            read_pipe(pds[0][0]);
            write_safe(1, "DONG\n");
            write_safe(pds[1][1], "0");
            if (i == N) {
                exit(0);
            }
        } else {
            write_safe(1, "DING\n");
            write_safe(pds[0][1], "1");
            read_pipe(pds[1][0]);
            sleep(D);
        }
    }

    wait_child();
    close_all();
    exit(0);
}

//v2 does not match the given algorithm!!!

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <stdio.h>

void check_args(const char* argv1, const char* argv2, const char* program);
void write_safe(const char* str);
void write_safe(const char* str) {
    int str_length = strlen(str);
    if (write(1, str, str_length) != str_length) {
        err(2, "Could not write to stdout");
    }
}

void check_args(const char* argv1, const char* argv2, const char* program) {
    if (strlen(argv1) != 1 || strlen(argv2) != 1) {
        errx(1, "Invalid arguments. Usage %s N D", program);
    }

    char num1 = argv1[0], num2 = argv2[0];

    if (!(num1 <= '9' && num1 >= '0') || !(num2 <= '9' && num2 >= '0')) {
        errx(1, "Invalid arguments. Usage %s N D", program);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage %s N D", argv[0]);
    }

    check_args(argv[1], argv[2], argv[0]);

    int N = (int)argv[1][0] - '0', D = (int)argv[2][0] - '0';

    for (int i = 0; i < N; i++) {
        write_safe("DING\n");

        const pid_t child = fork();
        if (child == -1) {
            err(3, "Could not fork\n");
        }

        if (child == 0) {
            write_safe("DONG\n");
            exit(0);
        }

        int status;
        if (wait(&status) == -1) {
            err(4, "Could not wait for child process\n");
        }

        if (!WIFEXITED(status)) {
            errx(5, "Child process did not terminate normally");
        }

        if (WEXITSTATUS(status) != 0) {
            errx(6, "Child process finished with exit code not 0");
        }

        sleep(D);
    }

    exit(0);
}


//Ваши колеги - асистенти по ОС имат нужда от демострационна програма на C, която
//да служи като пример за конкурентност и синхронизация на процеси. Напишете такава програма,
//приемаща два задължителни позиционни параметъра – едноцифрени числа. Примерно извикване:
//./main N D
//        Общ алгоритъм на програмата:
//• началният (родителски) процес създава процес-наследник
//• N на брой пъти се изпълнява:
//– родителският процес извежда на stdout низа “DING ”
//– процесът-наследник извежда на stdout низа “DONG”
//– родителският процес изчаква D секунди
//        Гарантирайте, че:
//• процесът-наследник винаги извежда “DONG” след като родителския процес е извел “DING ”
//• родителският процес винаги започва изчакването след като процеса-наследник е извел “DONG”
//Забележка: За изчакването погледнете sleep(3).