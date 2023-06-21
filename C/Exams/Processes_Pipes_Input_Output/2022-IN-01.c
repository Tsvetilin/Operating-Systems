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

#v2

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

char temp[5] = "temp";

void check_args(const char* argv1, const char* argv2, const char* program);
void write_safe(const char* str);
void wait_signal(const char* fifo);
void signal_s(const char* fifo);
void mkfifo_safe(const char* fifo);
void write_pipe(int fd, char c);
void read_pipe(int fd);
void unlink_safe(const char* file_name);
void wait_child(void);
void handle_errx(int exit_code, const char* message);
void handle_err(int exit_code, const char* message);

void handle_errx(int exit_code, const char* message) {
    unlink_safe(temp);
    errx(exit_code, message);
}
void handle_err(int exit_code, const char* message) {
    unlink_safe(temp);
    err(exit_code, message);
}

void wait_child(void) {
    int status;
    if (wait(&status) == -1) {
        handle_err(12, "Could not wait for child process to finish");
    }

    if (!WIFEXITED(status)) {
        handle_errx(13, "Child process did not terminate normally");
    }

    if (WEXITSTATUS(status) != 0) {
        handle_errx(14, "Child process finished with exit code not 0");
    }
}

void unlink_safe(const char* file_name) {
    if (unlink(file_name) == -1) {
        err(11, "Could not remove file %s", file_name);
    }
}

void read_pipe(int fd) {
    char c;
    int bytes_read;
    if ((bytes_read = read(fd, &c, sizeof(c))) != sizeof(c)) {
        if (bytes_read == -1) {
            handle_err(9, "Could not read from pipe");
        } else {
            handle_errx(9, "Could not read from pipe");
        }
    }
}

void write_pipe(int fd, char c) {
    int bytes_written;
    if ((bytes_written = write(fd, &c, sizeof(c))) != sizeof(c)) {
        if (bytes_written == -1) {
            handle_err(10, "Could not read from pipe");
        } else {
            handle_errx(10, "Could not read from pipe");
        }
    }
}
void mkfifo_safe(const char* fifo) {
    if (mkfifo(fifo, 0666) == -1) {
        err(9, "Could not mkfifo");
    }
}
void signal_s(const char* fifo) {
    int fd = open(fifo, O_WRONLY);
    char c = '1';
    write_pipe(fd, c);

    close(fd);
}

void wait_signal(const char* fifo) {
    int fd = open(fifo, O_RDONLY);
    read_pipe(fd);
    close(fd);
}

pid_t fork_safe(void);

pid_t fork_safe(void) {
    const pid_t child = fork();
    if (child == -1) {
        handle_err(3, "Could not fork\n");
    }

    return child;
}

void write_safe(const char* str) {
    int str_length = strlen(str);
    if (write(1, str, str_length) != str_length) {
        handle_err(2, "Could not write to stdout");
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
    mkfifo_safe(temp);

    const pid_t child = fork_safe();
    for (int i = 1; i <= N; i++) {
        if (child == 0) {
            wait_signal(temp);
            write_safe("DONG\n");
            signal_s(temp);
            if (i == N) {
                exit(0);
            }
        } else {
            write_safe("DING\n");
            signal_s(temp);
            wait_signal(temp);
            sleep(D);
        }
    }

    wait_child();
    unlink_safe(temp);
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