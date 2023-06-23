#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    if (execlp("sleep", "sleep", "60", (char*)NULL) == -1) {
        err(1, "err execling");
    } 
}

//T3 - Да се напише програма на C, която изпълнява команда sleep (за 60 секунди).
