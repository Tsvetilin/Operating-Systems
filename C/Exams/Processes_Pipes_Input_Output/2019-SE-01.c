#include <err.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <time.h>

int time_cond(time_t, time_t, int);
int time_cond(time_t t_start, time_t t_end, int seconds) {
        return seconds > (t_end - t_start);
}

int main(int argc, char** argv) {

        if(argc < 3) {
                errx(2, "arg count. Usage: ./main <seconds> <Q> args ...");
        }

        int seconds = atoi(argv[1]);
        char* Q = argv[2];
        char** args = argv + 2;

        int condition = 0;

        int fd = open("run.log", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

        if(fd == -1) {
                err(2, "opening run.log");
        }

        while(1) {

                pid_t pid = fork();
                if(pid == -1) {
                        err(3, "forking");
                }

                if(pid == 0) {
                        if(execvp(Q, args) == -1) {
                                err(4, "execing");
                        }
                } else {
                        int status;
                        time_t t_start = time(NULL);
                        if(wait(&status) == -1) {
                                err(5, "waiting");
                        }
                        time_t t_end = time(NULL);

                        int exit_code;
                        if(WIFEXITED(status)) {
                                exit_code = WEXITSTATUS(status);
                        } else {
                                exit_code = 129;
                        }

                        dprintf(fd, "%ld %ld %d\n",
                                        t_start,
                                        t_end,
                                        exit_code);


                        if(exit_code != 0 && time_cond(t_start, t_end, seconds)) {
                                if(condition == 1) {
                                        break;
                                } else {
                                        condition = 1;
                                }
                        } else {
                                condition = 0;
                        }

                }
        }

        close(fd);

        return 0;
}

//v2

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

typedef struct triplet {
    time_t start_time;
    time_t end_time;
    int exit_code;
} triplet;

void validate_interval(const char* str);
void write_safe(int fd, const triplet tr);
bool check_condition(const triplet first, const triplet second, int ttl);
triplet execute_command(int fd, const char* command, char** args);

bool check_condition(const triplet first, const triplet second, int ttl) {
    return ((first.end_time - first.start_time) < ttl && (second.end_time - second.start_time) < ttl) ||
           (first.exit_code != 0 && second.exit_code != 0);
}

void write_safe(int fd, const triplet tr) {
    if (write(fd, &tr, sizeof(triplet)) != sizeof(triplet)) {
        err(129, "Could not write to file");
    }
}

triplet execute_command(int fd, const char* command, char** args) {
    const pid_t child = fork();
    if (child == -1) {
        err(129, "Could not fork");
    }

    triplet tr;
    if (child == 0) {
        if (execvp(command, args) == -1) {
            err(129, "Could not exec %s", command);
        }
    }

    tr.start_time = time(NULL);
    int status;
    if (wait(&status) == -1) {
        err(129, "Could not wait for child process");
    }
    tr.end_time = time(NULL);
    if (WIFEXITED(status)) {
        tr.exit_code = WEXITSTATUS(status);
    }

    write_safe(fd, tr);
    return tr;
}

void validate_interval(const char* str) {
    if (strlen(str) > 1 || str[0] > '9' || str[0] < '0') {
        errx(129, "Invalid TTL. Should be digit");
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        errx(129, "Invalid arguments. Usage: %s <TTL> <Q> [args]", argv[0]);
    }

    validate_interval(argv[1]);
    uint8_t ttl = atoi(argv[1]);
    const char* command = argv[2];
    const char* output_file = "run.log";
    char* args[argc-3];
    for (int i = 3; i < argc; i++) {
        args[i-3] = argv[i];
    }

    int fd;
    if ((fd = open(output_file, O_RDWR | O_TRUNC | O_CREAT, S_IWUSR | S_IRUSR)) == -1) {
        err(129, "Could not open file %s", output_file);
    }

    triplet old, new;
    old = execute_command(fd, command, args);
    while(true) {
        new = execute_command(fd, command, args);

        if (check_condition(old, new, ttl)) {
            break;
        }
        old = new;
    }

    close(fd);
}
