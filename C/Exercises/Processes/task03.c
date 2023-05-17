#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    if (execl("/bin/sleep", "sleep", "60", 0) == -1) {
        err(1, "err execling");
    } else {
        dprintf(1, "foobar\n");
    }
}


//T3 - Да се напише програма на C, която изпълнява команда sleep (за 60 секунди).

