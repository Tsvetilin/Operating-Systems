#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <sys/wait.h>

const int BUFF_SIZE = 4096;

void write_safe(const char* str);

void write_safe(const char* str) {
    if (write(1, str, strlen(str)) != (int)strlen(str)) {
        err(1, "Failed writing");
    }
}

int main(void) {
    int bytes_read;
    char buff[BUFF_SIZE];
    const char *cmd = "Enter command:";
    write_safe(cmd);
    while ((bytes_read = read(1, buff, sizeof(buff))) > 0) {
        buff[bytes_read - 1] = '\0';
        const pid_t child = fork();
        if (child == -1) {
            err(3, "Failed fork");
        }

        if (strcmp(buff, "exit") == 0) {
            exit(0);
        }

        if (child == 0) {
            char command[BUFF_SIZE];
            strcpy(command, "/bin/");
            strcat(command, buff);
            if (execl(command, buff, (char *)NULL) == -1) {
                err(6, "Could not exec %s", buff);
            }
        }

        int status;
        if (wait(&status) == -1) {
            err(4, "Could not wait for child process to finish");
        }

        if (!WIFEXITED(status)) {
            errx(5, "Child process did not terminate normally");
        }

        if (WEXITSTATUS(status) != 0) {
            errx(6, "Child process finished with exit code not 0");
        }

        write_safe(cmd);
    }

    if (bytes_read == -1) {
        err(2, "Failed reading");
    }
}

//Напишете програма на C, която реализира simple command prompt. Тя изпълнява в
//        цикъл следната поредица действия:
//1. Извежда промпт на стандартния изход.
//2. Прочита име на команда.
//3. Изпълнява без параметри прочетената команда.
//Командите се търсят в директорията /bin. За край на програмата се смята въвеждането на exit.