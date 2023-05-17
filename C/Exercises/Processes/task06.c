#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Invalid number of arguments. Usage: %s <cmd>", argv[0]);
    }

    const char* cmd = argv[1];

    pid_t child_pid = fork();
    if(child_pid == -1) {
        err(2, "could not fork");
    }
    if (child_pid == 0) {
        if (execlp(cmd, "cmd", (char*)NULL) == -1) {
            err(3, "could not execlp %s", cmd);
        }
    }

    int child_exit_status;
    if(wait(&child_exit_status) == -1) {
        err(4, "could not wait for child");
    }

    if (!(WIFEXITED(child_exit_status))) {
        errx(5, "child did not terminate normally");
    }

    if(WEXITSTATUS(child_exit_status)) {
        errx(6, "child exit code not 0");
    }

    dprintf(1, "command %s was successfully executed %d\n", cmd, WEXITSTATUS(child_exit_status));

    exit(0);
}

//Да се напише програма на С, която получава като параметър команда (без параметри) и при успешното ѝ изпълнение,
//извежда на стандартния изход името на командата.