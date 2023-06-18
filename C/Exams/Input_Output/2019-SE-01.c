#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>

const int MAX_SIZE = 16384;
const int MAX_USERS = 2048;

typedef struct __attribute__((packed)) {
    uint32_t uid;
    uint16_t _;
    uint16_t __;
    uint32_t start_session;
    uint32_t end_session;
} user_data;

typedef struct __attribute__((packed)) {
    uint32_t uid;
    uint32_t session_duration;
} user_session;

void close_safe(int fd);
double calculate_dispersion(user_session* sessions, uint32_t nums_count, double average);

double calculate_dispersion(user_session* sessions, uint32_t nums_count, double average) {
    double sum = 0;
    for (uint32_t i = 0; i < nums_count; i++) {
        double diff = sessions[i].session_duration > average ? sessions[i].session_duration - average : average - sessions[i].session_duration;
        sum += (diff * diff);
    }

    return (sum / nums_count);
}

void close_safe(int fd) {
    int errno_ = errno;
    close(fd);
    errno = errno_;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        errx(1, "Invalid arguments. Usage: %s <file_name>", argv[0]);
    }

    const char* file_name = argv[0];

    struct stat st;
    if (stat(file_name, &st) == -1) {
        err(4, "Could not stat file %s", file_name);
    }

    if (st.st_size % sizeof(user_data) != 0) {
        errx(5, "Invalid file content");
    }

    user_session sessions[MAX_SIZE];
    user_data ud;
    int fd;
    if ((fd = open(file_name, O_RDONLY)) == -1) {
        err(2, "Could not open file %s", file_name);
    }

    int bytes_count, counter = 0;
    uint32_t duration, sum = 0;;
    while((bytes_count = read(fd, &ud, sizeof(ud))) > 0) {
        if (ud.end_session < ud.start_session) {
            close_safe(fd);
            errx(7, "Invalid session interval");
        }
        duration = ud.end_session - ud.start_session;
        sum += duration;
        sessions[counter] = (user_session){ud.uid, duration};
        counter++;
    }

    close_safe(fd);
    if (bytes_count == -1) {
        err(3, "Could not read from file %s", file_name);
    }

    double average = sum / counter;
    double dispersion = calculate_dispersion(sessions, counter, average);
    uint32_t us[MAX_USERS];
    for (int i = 0; i < MAX_USERS; i++) {
        us[i] = 0;
    }

    for (int i = 0; i < counter; i++) {
        uint32_t sq = (sessions[i].session_duration * sessions[i].session_duration);
        if (sq > dispersion) {
            if (us[sessions[i].uid] < sq) {
                us[sessions[i].uid] = sq;
            }
        }
    }

    for (uint32_t i = 0; i < MAX_USERS; i++) {
        if (us[i] > 0) {
            dprintf(1, "%d %d\n", i, us[i]);
        }
    }
}