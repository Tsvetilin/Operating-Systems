#include <stdio.h>
#include <err.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    size_t successful_counter = 0, failed_counter = 0;

    for (int i = 1; i < argc; i++) {
        const char* cmd = argv[i];
        const pid_t child_pid = fork();
        if (child_pid == -1) {
            err(1, "Error while fork process %s", cmd);
        }
        if (child_pid == 0) {
            if (execlp(cmd, cmd, (char *)NULL) == -1) {
                err(2, "Error executing cmd %s", cmd);
            }
        }

        int status;
        if (wait(&status) == -1) {
            err(3, "could not wait for current child");
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                successful_counter++;
            } else {
                failed_counter++;
            }
        } else {
            failed_counter++;
        }
    }

    dprintf(1, "successful: %d\nunsuccessful: %d\n", successful_counter, failed_counter);
    exit(0);
}

//Да се напише програма на C, която изпълнява последователно подадените ѝ като параметри команди,
//като реализира следната функционалност постъпково:
//main cmd1 ... cmdN Изпълнява всяка от командите в отделен дъщерен процес.
//... при което се запазва броя на изпълнените команди, които са дали грешка и броя на завършилите успешно.
