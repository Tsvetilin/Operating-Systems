#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void execute_command(const char* cmd);

void execute_command(const char* cmd) {
    const pid_t child_pid = fork();
    if (child_pid == -1) {
        err(2, "Could not fork process");
    }

    if (child_pid == 0) {
        if (execlp(cmd, cmd, (char *)NULL) == -1) {
            err(3, "Could not execlp %s", cmd);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 args");
    }

    char *cmd1 = argv[1], *cmd2 = argv[2];

    execute_command(cmd1);
    execute_command(cmd2);

    int status1, status2;

    const pid_t first = wait(&status1);
    if (first == -1) {
        err(6, "Could not wait for any process");
    }

    if(WIFEXITED(status1) && !WEXITSTATUS(status1)) {
        dprintf(1, "%d\n", first);
    } else {
        const pid_t second = wait(&status2);
        if (second == -1) {
            err(6, "Could not wait for second process");
        }
        if(WIFEXITED(status2) && !WEXITSTATUS(status2)) {
            dprintf(1, "%d\n", second);
        } else {
            dprintf(1, "%d\n", -1);
        }
    }
}


//Да се напише програма на C, която получава като командни параметри две команди (без параметри). Изпълнява ги
//едновременно и извежда на стандартния изход номера на процеса на първата завършила успешно.
//Ако нито една не завърши успешно извежда -1.