#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <fcntl.h>
#include <stdbool.h>

bool check_byte(unsigned char b);

bool check_byte(unsigned char b) {
    return b ==0x00 || b == 0xFF || b == 0x55 || b == 0x7D;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Invalid arguments. Usage: %s <input_file> <output_file>", argv[0]);
    }

    int pd[2];
    if (pipe(pd) == -1) {
        err(3, "Could not pipe");
    }

    const char *source_file = argv[1], *output_file = argv[2];
    const pid_t child = fork();
    if (child == -1) {
        err(2, "Could not fork");
    }

    if (child == 0) {
        close(pd[0]);
        dup2(pd[1], 1);
        if (execlp("cat", "cat", source_file, (char*)NULL) == -1) {
            err(4, "Could not execlp cat");
        }

        exit(0);
    }

    close(pd[1]);


    unsigned char escape_char = 0x7D;
    unsigned char key = 0x20;
    unsigned char current_byte;
    int bytes_read;
    int fd;

    if ((fd = open(output_file, O_RDWR | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR)) == -1) {
        err(5, "Could not open file %s", output_file);
    }

    while ((bytes_read = read(pd[0], &current_byte, sizeof(current_byte))) > 0) {
        if (current_byte == escape_char) {
            if ((bytes_read = read(pd[0], &current_byte, sizeof(current_byte))) > 0) {
                unsigned char original_byte = (current_byte ^ key);
                if (!check_byte(original_byte)) {
                    errx(8, "Invalid file content");
                }
                if (write(fd, &original_byte, sizeof(original_byte)) != sizeof(original_byte)) {
                    err(6, "Could not write to file");
                }
            }
        }
    }

    if (bytes_read == -1) {
        err(7, "Could not read");
    }

    close(fd);
    exit(0);
}


//При изграждане на система за пренасяне на сериен асинхронен сигнал върху радиопреносна мрежа се оказало, че големи поредици от битове само нули или само единици смущават
//        сигнала, поради нестабилно ниво. Инженерите решили проблема, като:
//• в моментите, в които няма сигнал от серийният порт, вкарвали изкуствено байт 0x55 в потока;
//• реалните байтове 0x00, 0xFF, 0x55 и 0x7D се кодирали посредством XOR-ване (побитова обработка с изключващо-или) с 0x20, като полученият байт се изпращал през потока, предхождан
//        от 0x7D, който играе ролята на escape character.
//Разполагате със запис от такъв поток. Напишете програма на C, която приема два параметъра -
//имена на файлове. Примерно извикване:
//$ ./main input.lfld output.bin
//        Програмата трябва да обработва записа и да генерира output.bin, който да съдържа оригиналните данни. Четенето на входните данни трябва да става посредством изпълнение на външна shell
//команда.
