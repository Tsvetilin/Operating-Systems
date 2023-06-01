#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

const int MAX_LINE_LENGTH = 4096;

void write_safe(char c);
void print_characters(const char* str, const char* option);
void check_char_options(const char* options);
void print_fields(const char* str, char delimeter, const char* option);

void write_safe(char c) {
    if (write(1, &c, sizeof(c)) != sizeof(c)) {
        err(4, "Could not write to stdin.");
    }
}

void print_characters(const char* str, const char* option) {
    int str_len = strlen(str) - 1; //exluce \n
    int option_len = strlen(option);

    if (option_len == 1) {
        int index = (int)(option[0] - '1');
        if (index < str_len) {
            write_safe(str[index]);
        }
    } else if (option_len == 3) {
        int start = (int)(option[0] - '1');
        if (start < str_len) {
            int end = (int)(option[2] - '1');
            if (end >= str_len) {
                end = str_len - 1;
            }
            for (int i = start; i <= end; i ++) {
                write_safe(str[i]);
            }
        }
    } else {
        errx(1, "Invalid option");
    }


    write_safe('\n');
}

void print_fields(const char* str, char delimeter, const char* option) {
    int str_len = strlen(str) - 1, index = 0;
    int option_len = strlen(option);

    if (option_len == 1) {
        int pos = (int)(option[0] - '0');
        while(index < str_len) {
            if (str[index] == delimeter) {
                pos--;
                if (pos == 1) {
                    index++;
                    while(index < str_len && str[index] != delimeter) {
                        write_safe(str[index]);
                        index++;
                    }
                }
            }
            index++;
        }
    } else if (option_len == 3) {
        int pos = (int)(option[0] - '0');
        while(index < str_len) {
            if (str[index] == delimeter) {
                pos--;
                if (pos == 1) {
                    index++;
                    int end = (int)(option[2] - '0');
                    while(index < str_len && pos < end) {
                        if(str[index] == delimeter) {
                            pos++;
                            if (pos == end) {
                                break;
                            }
                        }

                        write_safe(str[index]);
                        index++;
                    }
                }
            }
            index++;
        }
    } else {
        errx(1, "Invalid option");
    }

    write_safe('\n');
}

void check_char_options(const char* options) {
    int opt_len = strlen(options);
    if (opt_len != 1 && opt_len != 3) {
        errx(8, "Invalid option");
    }

    size_t start = options[0], end;

    if (options[0] == '0') {
        errx(5, "byte/characters are numbered from 1");
    }
    if (opt_len == 3 && (start > (end = options[2]))) {
        errx(4, "Invalid decreasing range");
    }
}

int main(int argc, char** argv) {
    if (argc < 4 || argc > 5) {
        errx(1, "Invalid arguments.");
    }

    char delimeter;
    char *options;
    char option;

    if (strcmp(argv[1], "-c") == 0) {
        options = argv[2];
        check_char_options(argv[2]);
        option = 'c';
    } else if (strcmp(argv[1], "-d") == 0) {
        delimeter = argv[2][0];
        if (strcmp(argv[3], "-f") != 0) {
            errx(6, "Invalid option");
        }
        option = 'd';
        options = argv[4];
    } else {
        errx(2, "Invalid option.");
    }

    char buff[4096];
    int bytes_read;
    while ((bytes_read = read(0, buff, sizeof(buff))) > 0) {
        buff[bytes_read] = '\0';
        if (option == 'c') {
            print_characters(buff, options);
        } else if (option == 'd') {
            print_fields(buff, delimeter, options);
        }
    }

    if (bytes_read == -1) {
        err(9, "Could not read from stdin");
    }

    exit(0);
}

//Напишете програма на C, която да работи подобно на командата cut, реализирайки
//само следната функционалност:
//• програмата трябва да чете текст от стандартния вход и да извежда избраната част от всеки
//        ред на стандартния изход;
//• ако първият параметър на програмата е низът -c, тогава вторият параметър е или едноцифрено число (от 1 до 9),
//или две едноцифрени числа N и M (N ≤ M), разделени с тире (напр. 3-5).
//В този случай програмата трябва да изведе избраният/избраните символи от реда: или само
//символа на указаната позиция, или няколко последователни символи на посочените позиции.
//• ако първият параметър на програмата е низът -d, тогава вторият параметър е низ, от който е
//        важен само първият символ; той се използва като разделител между полета на реда. Третият
//        параметър трябва да бъде низът -f, а четвъртият - или едноцифрено число (от 1 до 9),
//или две едноцифрени числа N и M (N ≤ M), разделени с тире (напр. 3-5). В този случай
//        програмата трябва да разглежда реда като съставен от няколко полета (може би празни),
//разделени от указания символ (първият символ от низа, следващ парметъра -d), и да изведе
//        или само указаното поле, или няколко последователни полета на указаните позиции, разделени
//        от същия разделител.
//• ако някой ред няма достатъчно символи/полета, за него програмата трябва да изведе каквото
//(докъдето) е възможно (или дори празен ред).
