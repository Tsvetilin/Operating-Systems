#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void execute_child(const char* cmd);

void execute_child(const char* cmd) {
    const pid_t child_pid = fork();
    if (child_pid == -1) {
        err(2, "Error while fork process");
    }

    if (child_pid == 0) {
        if(execlp(cmd, cmd, (char*)NULL) == -1 ) {
            err(3, "Error executing cmd %s", cmd);
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 3 arguments");
    }

    const char* cmd1 = argv[1];
    const char* cmd2 = argv[2];

    execute_child(cmd1);

    int status;
    if(wait(&status) == -1) {
        err(4, "Could not wait child1");
    }

    if(!WIFEXITED(status)) {
        errx(42, "%s did not terminated normally", cmd1);
    }

    if(WEXITSTATUS(status)) {
        errx(42, "%s exist status not 0", cmd1);
    }

    execute_child(cmd2);

    wait(NULL);
    exit(0);
}

//Да се напише програма на C, която получава като параметри от команден ред две команди (без параметри).
//Изпълнява първата. Ако тя е завършила успешно изпълнява втората. Ако не, завършва с код 42.