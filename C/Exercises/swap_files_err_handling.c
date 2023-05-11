#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

typedef struct
{
    int fds[5];
} ctx_t;

void init_ctx(ctx_t *ctx)
{
    for (int i = 0; i < 5; i++)
    {
        ctx->fds[i] = -1;
    }
}

void cleanup(ctx_t *ctx)
{
    int old_errno = errno;
    for (int i = 0; i < 5; i++)
    {
        if (ctx->fds[i] >= 0)
        {
            close(ctx->fds[i]);
        }
    }
    errno = old_errno;
}

void add_fd_to_ctx(ctx_t *ctx, int fd)
{
    for (int i = 0; i < 5; i++)
    {
        if (ctx->fds[i] < 0)
        {
            ctx->fds[i] = fd;
            return;
        }
    }
    cleanup(ctx);
    errx(1, "too many open files");
}

int open_safe(ctx_t *ctx, const char *filename, int flags, mode_t mode)
{
    int fd = open(filename, flags, mode);
    if (fd < 0)
    {
        cleanup(ctx);
        err(1, "could not open %s", filename);
    }
    add_fd_to_ctx(ctx, fd);
    return fd;
}

off_t lseek_safe(ctx_t *ctx, int fd, off_t offset, int whence)
{
    off_t result = lseek(fd, offset, whence);
    if (result < 0)
    {
        cleanup(ctx);
        err(1, "could not seek");
    }
    return result;
}

int ftruncate_safe(ctx_t *ctx, int fd, off_t length)
{
    int result = ftruncate(fd, length);
    if (result < 0)
    {
        cleanup(ctx);
        err(1, "could not truncate");
    }
    return result;
}

void copy_data(ctx_t *ctx, int fd_from, int fd_to)
{
    char c[4096];
    ssize_t read_size;
    ssize_t write_size;

    while ((read_size = read(fd_from, c, sizeof(c))) > 0)
    {
        write_size = write(fd_to, &c, read_size);
        if (write_size < 0)
        {
            cleanup(ctx);
            err(1, "Error while writing");
        }
        if (write_size != read_size)
        {
            cleanup(ctx);
            errx(1, "Did not write everything at once");
        }
    }
    if (read_size != 0)
    {
        cleanup(ctx);
        err(2, "Error while reading");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        errx(1, "err");
    }

    ctx_t ctx;
    init_ctx(&ctx);

    int fd1 = open_safe(&ctx, argv[1], O_RDWR, 0666);
    int fd2 = open_safe(&ctx, argv[2], O_RDWR, 0666);
    int fd3 = open_safe(&ctx, "my_temp_file", O_CREAT | O_RDWR | O_TRUNC, 0666);

    copy_data(&ctx, fd1, fd3);

    lseek_safe(&ctx, fd1, 0, SEEK_SET);
    ftruncate_safe(&ctx, fd1, 0);

    copy_data(&ctx, fd2, fd1);

    lseek_safe(&ctx, fd2, 0, SEEK_SET);
    lseek_safe(&ctx, fd3, 0, SEEK_SET);
    ftruncate_safe(&ctx, fd2, 0);

    copy_data(&ctx, fd3, fd2);

    cleanup(&ctx);

    exit(0);
}