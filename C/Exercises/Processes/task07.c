#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char **argv){
    if(argc != 4) {
        errx(1, "Invalid number of arguments. Usage: %s <cmd1> <cmd2> <cmd3>", argv[0]);
    }

    const char *cmd1 = argv[1], *cmd2 = argv[2], *cmd3 = argv[3];

    pid_t cmd1_pid = fork();
    if(cmd1_pid == -1) {
        err(2, "could not fork for %s", cmd1);
    }

    if(cmd1_pid == 0) {
        if (execlp(cmd1, cmd1, (char *) NULL) == -1)}
            err(3, "could not execlp %s", cmd1);
       }
    }

    int stat_cmd1;
    if(wait(&stat_cmd1) == -1) {
        err(4, "could not wait for %s", cmd1);
    }

    if(!WIFEXITED(stat_cmd1)) {
        errx(5, "%s did not terminated normally", cmd1);
    }

    dprintf(1, "%s, pid: %d, exit code: %d\n", cmd1, cmd1_pid, WEXITSTATUS(stat_cmd1));

    pid_t cmd2_pid = fork();
    if(cmd2_pid == -1) {
        err(6, "could not fork for %s", cmd2);
    }

    if(cmd2_pid == 0) {
        if(execlp(cmd2, cmd2, (char*)NULL) == -1) {
            err(7, "could not execlp %s", cmd2);
        }
    }

    int stat_cmd2;
    if(wait(&stat_cmd2) == -1)
        err(8, "could not wait for %s", cmd2);

    if(!WIFEXITED(stat_cmd2))
        errx(9, "%s did not terminated normally", cmd2);

    dprintf(1, "%s, pid: %d, exit code: %d\n", cmd2, cmd2_pid, WEXITSTATUS(stat_cmd2));

    pid_t cmd3_pid = fork();
    if(cmd3_pid == -1) {
        err(10, "could not fork for %s", cmd3);
    }

    if(cmd3_pid == 0) {
        if(execlp(cmd3, cmd3, (char*)NULL) == -1) {
            err(11, "could not execlp %s", cmd3);
        }
    }

    int stat_cmd3;
    if(wait(&stat_cmd3) == -1) {
        err(12, "could not wait for %s", cmd3);
    }
    if(!WIFEXITED(stat_cmd3)) {
        errx(13, "%s did not terminated normally", cmd3);
    }

    dprintf(1, "%s, pid: %d, exit code: %d\n", cmd3, cmd3_pid, WEXITSTATUS(stat_cmd3));

    exit(0);
}

//Да се напише програма на С, която получава като параметри три команди (без параметри), изпълнява ги последователно,
//като изчаква края на всяка и извежда на стандартния изход номера на завършилия процес, както и неговия код на завършване.