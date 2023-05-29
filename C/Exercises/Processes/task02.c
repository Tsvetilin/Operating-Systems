#include <stdlib.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main (int argc, char** argv) {
    if (argc != 2) {
        errx(1, "err");
    }

    if (execlp("/bin/ls", "ls", argv[1], 0) == -1) {
        err(1, "err execling");
    } 
    
}

//T2 - Да се напише програма на C, която изпълнява команда ls с точно един аргумент.
