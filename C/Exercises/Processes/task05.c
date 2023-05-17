#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int n = 100;
    int status;

    pid_t a = fork();
    if (a == -1) {
        err(1, "BOO");
    }
    if (a > 0) {
        wait(&status);
        for (int i = 0; i < n; i++) {
            write(1, "F", 1);
        }
    }else {
        for (int i = 0; i < n; i++) {
            write(1, "S", 1);
        }
    }

    write(1,"\n",1);

    exit(0);
}


//T5 - Да се напише програма на C, която е аналогична на горния пример, но принуждава бащата да изчака сина си да завърши.

