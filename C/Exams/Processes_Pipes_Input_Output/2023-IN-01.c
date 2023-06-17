
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>

void validate_number(long number, int start, int end, const char* num_type);
void write_safe(const char* text);

void write_safe(const char* text) {
    if (write(0, text, strlen(text)) != (ssize_t)strlen(text)) {
        errx(3, "Could not write to stdout");
    }
}

void validate_number(long number, int start, int end, const char* num_type) {
    if (number < start || number > end) {
        errx(2, "Invalid %s. Number should be between %d-%d", num_type, start, end);
    }
}

const char words[3][5] = {"tic", "tac", "toe\n"};

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <NC> <WC>", argv[0]);
    }

    char* endptr;
    long NC, WC;
    NC = strtol(argv[1], &endptr, 10);
    validate_number(NC, 1, 7, "NC");
    WC = strtol(argv[2], &endptr, 10);
    validate_number(WC, 1, 35, "WC");
    uint8_t counter = 0;

    while (counter < WC) {
        write_safe(words[counter % 3]);
        counter++;

        for (uint8_t i = 0; i < NC && counter < WC; i++) {
            const pid_t child = fork();
            if (child == -1) {
                err(4, "Could not fork");
            }

            if (child == 0) {
                write_safe(words[counter % 3]);
                exit(0);
            }

            int status;
            if (wait(&status) == -1) {
                err(5, "Could not wait for child process to finish");
            }

            if (!WIFEXITED(status)) {
                errx(6, "Child process did not terminate normally");
            }

            if (WEXITSTATUS(status) != 0) {
                errx(7, "Child process finished with exit code not 0");
            }

            counter++;
        }
    }
}
