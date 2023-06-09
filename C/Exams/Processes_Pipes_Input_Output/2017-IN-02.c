#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <string.h>
#include <sys/wait.h>

const int BUFF_SIZE = 4096;

void echo(void);
void execute(const char* command);
void validate_argument(const char* arg, const char* message);

void validate_argument(const char* arg, const char* message) {
    if (strlen(arg) > 4) {
        errx(5, "Invalid %s length. max length allowed - 4", message);
    }
}

void echo(void) {
    char c;
    int bytes_read;
    while ((bytes_read = read(0, &c, sizeof(c))) == sizeof(c)) {
        if (write(1, &c, sizeof(c)) != sizeof(c)) {
            err(3, "Could not write to stdout.");
        }
    }

    if (bytes_read == -1) {
        err(2, "Could not read from stdin");
    }
}

void execute(const char* command) {
    char buff[BUFF_SIZE], c;
    int bytes_read, index = 0;
    while ((bytes_read = read(0, &c, sizeof(c))) == sizeof(c)) {
        if (c == ' ' || c == '\n' || c == '\t') {
            buff[index] = '\0';
            index = 0;
            if (strlen(buff) > 0) {
                validate_argument(buff, "string");
                const pid_t child = fork();
                if (child == -1) {
                    err(4, "Could not fork.");
                }

                if (child == 0) {
                    if (execlp(command, buff, buff, (char *)NULL) == -1) {
                        err(5, "Could not execute command %s %s.", command, buff);
                    }
                }

                int status;
                if (wait(&status) == -1) {
                    err(6, "Could not wait for child process.");
                }

                if (!WIFEXITED(status)) {
                    errx(7, "Child process did not terminate normally");
                }

                if (WEXITSTATUS(status) != 0) {
                    errx(8, "Child process finished with exit status not 0");
                }
            }
        } else {
            buff[index] = c;
            index++;
        }
    }

    if (bytes_read == -1) {
        err(2, "Could not read from stdin.");
    }
}

int main(int argc, char** argv) {
    if (argc > 2) {
        errx(1, "Invalid arguments. Usage %s [<command>]", argv[0]);
    }

    const char* command = argv[1];
    if (argc == 2) {
        validate_argument(command, "command");
        execute(command);
    } else {
        echo();
    }
}


//Напишете програма на C, която приема незадължителен параметър – име на команда. Ако не е зададена команда като параметър, да се ползва командата echo. Максималната
//        допустима дължина на командата е 4 знака.
//Програмата чете низове (с максимална дължина 4 знака) от стандартния си вход, разделени с
//интервали (0x20) или знак за нов ред (0x0A). Ако някой низ е с дължина по-голяма от 4 знака, то
//        програмата да терминира със съобщение за грешка.
//Подадените на стандартния вход низове програмата трябва да третира като множество от параметри за дефинираната команда. Програмата ви трябва да изпълни командата колкото пъти е
//        необходимо с максимум два низа като параметри, като изчаква изпълнението да приключи, преди
//да започне ново изпълнение.
//Примерни вход, извиквания и изходи:
//$ cat f1
//        a1
//$ cat f2
//        a2
//$ cat f3
//        a3
//$ echo -e "f1\nf2 f3" | ./main cat
//a1
//a2
//a3
//$ echo -e "f1\nf2 f3" | ./main
//f1 f2
//f3