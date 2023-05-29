#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>

const char L[3][4] = { "tic ", "tac ", "toe\n" };

int main(int argc, char** argv) {

        if(argc != 3) {
                errx(1, "Arg count");
        }

        int pds[8][2];
        int NC = atoi(argv[1]);
        int WC = atoi(argv[2]);

        for(int i = 0; i < NC + 1; i++) {
                if(pipe(pds[i]) == -1) {
                        err(2, "creating pipe");
                }
        }

        int id = NC;
        for(int i = 0; i < NC; i++) {
                pid_t pid = fork();

                if(pid == 0) {
                        id = i;
                        break;
                }
        }

        if(id == NC) {
                for(int i = 1; i < NC; i++) {
                        close(pds[i][0]);
                        close(pds[i][1]);
                }
                close(pds[0][0]);
                close(pds[NC][1]);
        } else {
                for(int i = 0; i < NC+1; i++) {
                        if(i == id) {
                                close(pds[i][1]);
                                close(pds[i+1][0]);
                                i++;
                        } else {
                                close(pds[i][0]);
                                close(pds[i][1]);
                        }
                }
        }

        int count;
        if(id == NC) {
                count = 1;
                if(write(1, L[0], 4) == -1) {
                        err(2, "writing to stdout");
                }
                if(write(pds[0][1], &count, sizeof(count)) == -1) {
                        err(2, "writing to pipe");
                }
        }

        while(read(pds[id][0], &count, sizeof(count))) {
                if(count == WC) {
                        break;
                }
                if(write(1, L[count % 3], 4) == -1) {
                        err(3, "writing to pipe");
                }
                count++;
                if(id == NC) {
                        if(write(pds[0][1], &count, sizeof(count)) == -1) {
                                err(3, "writing to pipe");
                        }
                } else {
                        if(write(pds[id+1][1], &count, sizeof(count)) == -1) {
                                err(3, "writing to pipe");
                        }
                }
        }

        if(id == NC) {
                close(pds[0][1]);
                close(pds[id][0]);
        } else {
                close(pds[id+1][1]);
                close(pds[id][0]);
        }

        return 0;
}