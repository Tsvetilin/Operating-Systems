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