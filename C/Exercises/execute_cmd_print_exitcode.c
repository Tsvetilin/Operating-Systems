#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include "string.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        errx(2, "Args Count");
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        err(1, "Err");
    }

    int status;
    if (pid > 0)
    {
        wait(&status);

        if (WIFEXITED(status))
        {
            if (write(1, argv[1], strlen(argv[1])) < 0)
            {
                err(1, "Err");
            }
        }
    }
    else
    {
        if (execlp(argv[1], argv[1], (char *)NULL) < 0)
        {
            err(1, "Err");
        }
    }
}