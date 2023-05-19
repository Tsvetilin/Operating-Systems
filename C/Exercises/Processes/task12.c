#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

void execute_command(const char* cmd);

void execute_command(const char* cmd) {
    const pid_t child_pid = fork();
    if (child_pid == -1) {
        err(3, "Err while fork process");
    }

    if (child_pid == 0) {
        if (execlp(cmd, cmd, (char *)NULL) == -1) {
            err(4, "Err while execlp cmd %s", cmd);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 4) {
        errx(1, "Expected 4 args");
    }

    const char* cmd1 = argv[1];
    const char* cmd2 = argv[2];
    const char* file_name = argv[3];
    int fd;

    if ((fd = open(file_name, O_RDWR | O_CREAT, S_IRWXU)) == -1) {
        err(2, "Error while opening file %s", file_name);
    }

    execute_command(cmd1);

    int status;
    if (wait(&status) == -1) {
        err(5, "Could not wait child process");
    }

    if(WIFEXITED(status)) {
        if (WEXITSTATUS(status) == 0) {
            if (write(fd, cmd1, strlen(cmd1)) == -1) {
                const int errno_ = errno;
                close(fd);
                errno = errno_;
                err(6, "could not write to file %s", file_name);
            }
            write(fd, "\n", 1);
        } else {
            warnx("%s exit status not 0", cmd1);
        }
    } else {
        warnx("%s did not terminate normally", cmd1);
    }

    execute_command(cmd2);

    int status2;
    if (wait(&status2) == -1) {
        err(7, "Could not wait child process");
    }
    exit(0);
}

//Да се напише програма на C, която получава като параметри от команден ред две команди (без параметри) и име на файл
//в текущата директория. Ако файлът не съществува, го създава. Програмата изпълнява командите последователно,
//по реда на подаването им. Ако първата команда завърши успешно, програмата добавя нейното име към съдържанието на файла,
//подаден като команден параметър. Ако командата завърши неуспешно, програмата уведомява потребителя чрез подходящо съобщение.