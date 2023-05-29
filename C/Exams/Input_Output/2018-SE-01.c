#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

const int BUFF_SIZE = 4096;
bool BUFF[256];
char SUB[256];

void write_safe(char c);
void delete_chars(const char* str, const char* set, int str_length);
void mark_visited_chars(const char* set);
void substitute_chars(const char* str, const char* set, int str_length);
void mark_substitute_chars(const char* set1, const char* set2);
void substitute_sets(const char* str, const char* set1, const char* set2, int str_length);

void write_safe(char c) {
    if (write(1, &c, sizeof(c)) != sizeof(c)) {
        err(3, "Could not write to stdout");
    }
}

void delete_chars(const char* str, const char* set, int str_length) {
    mark_visited_chars(set);
    for (int i = 0; i < str_length; i++) {
        if (BUFF[(int)str[i]] == 0) {
            write_safe(str[i]);
        }
    }
}

void substitute_chars(const char* str, const char* set, int str_length) {
    mark_visited_chars(set);
    bool WRITTEN[256];
    for (int i = 0; i < str_length; i++) {
        if (BUFF[(int)str[i]] == 0) {
            write_safe(str[i]);
        } else if (BUFF[(int)str[i]] == 1 && WRITTEN[(int)str[i]] == 0) {
            WRITTEN[(int)str[i]] = 1;
            write_safe(str[i]);
        }
    }
}

void substitute_sets(const char* str, const char* set1, const char* set2, int str_length) {
    mark_visited_chars(set1);
    mark_substitute_chars(set1, set2);
    for (int i = 0; i < str_length; i++) {
        if (BUFF[(int)str[i]] == 0) {
            write_safe(str[i]);
        } else if (BUFF[(int)str[i]] == 1) {
            write_safe(SUB[(int)str[i]]);
        }
    }
}

void mark_visited_chars(const char* set) {
    int set_length = strlen(set);
    for (int i = 0; i < set_length; i++) {
        BUFF[(int)set[i]] = 1;
    }
}


void mark_substitute_chars(const char* set1, const char* set2) {
    int set_length = strlen(set1);
    for (int i = 0; i < set_length; i++) {
        SUB[(int)set1[i]] = set2[i];
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s [OPTION] SET1 [SET2]", argv[0]);
    }

    char str[BUFF_SIZE];
    if (read(0, &str, sizeof(str)) == -1) {
        err(4, "Could not read from stdin");
    }

    int str_length = strlen(str);
    char set1[BUFF_SIZE], set2[BUFF_SIZE];
    if (strcmp(argv[1],"-d") == 0) {
        strcpy(set1, argv[2]);
        delete_chars(str, set1, str_length);
    } else if  (strcmp(argv[1],"-s") == 0) {
        strcpy(set1, argv[2]);
        substitute_chars(str, set1, str_length);
    } else {
        if (strlen(argv[1]) != strlen(argv[2])) {
            errx(2, "Lenght of SET1 is not equal to length of SET2");
        }

        strcpy(set1, argv[1]);
        strcpy(set2, argv[2]);
        substitute_sets(str, set1, set2, str_length);
    }

    exit(0);
}

//Напишете програма на C, която да работи подобно на командата tr, реализирайки
//само следната функционалност:
//• програмата чете от стандартния вход и пише на стандартния изход
//33
//• общ вид на изпълнение: ./main [OPTION] SET1 [SET2]
//• OPTION би могъл да бъде или -d, или -s, или да липсва
//• SET1 и SET2 са низове, в които знаците нямат специално значение, т.е. всеки знак “означава”
//съответния знак. SET2, ако е необходим, трябва да е същата дължина като SET1
//• ако е подаден като първи параметър -d, програмата трие от входа всяко срещане на знак µ
//        от SET1; SET2 не е необходим
//• ако е подаден като първи параметър -s, програмата заменя от входа всяка поредица от повторения знак µ от SET1 с
//еднократен знак µ; SET2 не е необходим
//• в останалите случаи програмата заменя от входа всеки знак µ от SET1 със съответстващият
//му позиционно знак ν от SET2.
//Примерно извикване:
//$ echo asdf | ./main -d ’d123’ | ./main ’sm’ ’fa’ | ./main -s ’f’
//af
