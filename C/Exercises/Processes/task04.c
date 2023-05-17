#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>

int main(void) {
    int n = 100;

    pid_t a = fork();
    if (a == -1) {
        err(1, "BOO");
    }
    if (a > 0) {
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

//T4 - Да се напише програма на C, която създава процес дете и демонстрира принцина на конкурентност при процесите.

