//Author: Todor Kermedchiev
#include <err.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define uint128_t __uint128_t

typedef struct __attribute__((packed)) {
    uint64_t magic;
    uint32_t cfsb;
    uint32_t cfsu;
    uint32_t ofsb;
    uint32_t ofsu;
    uint32_t unused;
    uint32_t checksum;
    uint128_t sectionKey;
    uint32_t sections[4];
} header;

typedef struct {
    int64_t offset;
    uint64_t len;
    uint128_t dataKey;
} section;

void unlinkSafe(const char *pathname);
off_t lseekSafe(int fd, off_t offset, int whence, const char *message);
ssize_t readSafe(int fd, void *buf, ssize_t nbyte);
ssize_t writeSafe(int fd, void *buf, ssize_t bytes);
void parseHeader(header* h, int fd);
void checkFileSizeIsCorrect(header* h, int fd);
void parseSection(section* s, int fd);
void decryptSection(section* s, uint128_t key);
void readUnit(uint128_t* unit, int fd);
void writeUnit(uint128_t* unit, int fd);
void decryptUnit(uint128_t* unit, uint128_t key);
uint32_t findChecksum(int fd);

void unlinkSafe(const char *pathname) {
    if (unlink(pathname) < 0) {
        err(4, "Could not unlink (delete) file %s\n", pathname);
    }
}

off_t lseekSafe(int fd, off_t offset, int whence, const char *message) {
    off_t position = lseek(fd, offset, whence);
    if (position < 0) {
        err(1, message);
    }
    return position;
}

ssize_t readSafe(int fd, void *buf, ssize_t nbyte) {
    ssize_t bytesRead = read(fd, buf, nbyte);
    if (bytesRead < 0) {
        err(2, "Could not read\n");
    }

    return bytesRead;
}

ssize_t writeSafe(int fd, void *buf, ssize_t bytes) {
    ssize_t bytesWritten = write(fd, buf, bytes);
    if (bytesWritten < 0) {
        err(4, "Could not write\n");
    }
    if (bytesWritten != bytes) {
        err(5, "Could not write all data\n");
    }

    return bytesWritten;
}

void parseHeader(header* h, int fd) {
    int bytesRead = read(fd, h, sizeof(header)); // sizeof(header), защото sizeof(h) ще върне размера на пойнтера
    if (bytesRead == -1) {
        err(3, "Could not read. Could not parse header\n");
    }
    if (bytesRead != sizeof(header)) {
        err(5, "Could not read the whole header\n");
    }
}

void checkFileSizeIsCorrect(header* h, int fd) {
    struct stat info;
    if (fstat(fd, &info) < 0) {
        err(4, "Could not stat\n");
    }
    uint32_t fileSize = info.st_size;

    if (fileSize % 16 != 0 || fileSize != h->cfsb) {
        errx(5, "Incorrect file size\n");
    }
}

void parseSection(section* s, int fd) {
    int bytesRead = read(fd, s, sizeof(section));
    if (bytesRead < 0) {
        err(3, "Could not read. Could not parse section\n");
    }
    if (bytesRead != sizeof(section)) {
        errx(5, "Could not read the whole section\n");
    }
}

void decryptSection(section* s, uint128_t key) {
    uint128_t* first = (uint128_t*) s;
    uint128_t* second = first + 1;

    *first = *first ^ key;
    *second = *second ^ key;
}

void readUnit(uint128_t* unit, int fd) {
    int bytesRead = read(fd, unit, sizeof(uint128_t));
    if (bytesRead < 0) {
        err(3, "Could not read unit\n");
    }
    if (bytesRead != sizeof(uint128_t)) {
        errx(5, "Could not read the whole unit\n");
    }
}

void writeUnit(uint128_t* unit, int fd) {
    int bytesWritten = write(fd, unit, sizeof(uint128_t));
    if (bytesWritten < 0) {
        err(6, "Could not write unit\n");
    }
    if (bytesWritten != sizeof(uint128_t)) {
        errx(7, "Could not write the whole unit\n");
    }
}

void decryptUnit(uint128_t* unit, uint128_t key) {
    *unit = *unit ^ key;
}

uint32_t findChecksum(int fd) {
    uint32_t checksum = 0;
    uint32_t current;
    lseekSafe(fd, 0, SEEK_SET, "Could not lseek to the begining of the tmp file\n");

    while (readSafe(fd, &current, sizeof(current)) > 0) {
        checksum = checksum ^ current;
    }

    return checksum;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        errx(1, "Expected 2 arguments\n");
    }

    int fd_src = open(argv[1], O_RDONLY);
    if (fd_src == -1) {
        err(2, "Err opening");
    }

    int fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_dest == -1) {
        err(3, "Err opening");
    }

    char tmpName[255] = "tmp_";
    strcat(tmpName, argv[2]);
    int fd_tmp = open(tmpName, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd_tmp < 0) {
        err(3, "Err opening file %s for writing\n", tmpName);
    }

    header h;
    parseHeader(&h, fd_src);

    checkFileSizeIsCorrect(&h, fd_src);

    for (int i = 0; i < 4; ++i) {
        lseekSafe(fd_src, h.sections[i] * 16, SEEK_SET, "Could not lseek to section\n");

        section s;
        parseSection(&s, fd_src);
        decryptSection(&s, h.sectionKey);

        if (h.sections[i] == 0) {
            continue; // по условие е така
        }

        lseekSafe(fd_src, (h.sections[i] + s.offset) * 16, SEEK_SET, "Could not lseek to data\n");

        for (uint64_t j = 0; j < s.len; ++j) {
            uint128_t unit;
            readUnit(&unit, fd_src);
            decryptUnit(&unit, s.dataKey);
            writeUnit(&unit, fd_tmp);
        }
    }

    if (findChecksum(fd_tmp) != h.checksum) {
        close(fd_src);
        close(fd_dest);
        close(fd_tmp);
        unlinkSafe(argv[2]);
        unlinkSafe(tmpName);
        errx(10, "The checksum is not correct\n");
    }

    // removing padding
    lseekSafe(fd_tmp, 0, SEEK_SET, "Could not lseek to the begining of the tmp file\n");
    char c;
    for (uint32_t i = 0; readSafe(fd_tmp, &c, sizeof(c)) > 0 && i < h.ofsb; ++i) {
        writeSafe(fd_dest, &c, sizeof(c));
    }

    close(fd_src);
    close(fd_dest);
    close(fd_tmp);
    unlinkSafe(tmpName);

    return 0;
}

//Вашите колеги от съседната лаборатория са попаднали на вогонски∗ крипто-вирус
//        и вашата задача е да напишете програма, която възстановява файлове, криптирани от вируса.
//Примерно извикване: ./main input.encrypted output.decrypted
//        Вирусът не бил особенно добре проектиран и след анализ се оказало, че подменя оригиналните
//файлове с криптирани със следните особенности:
//• вогоните явно предпочитат да работят с данни от по 128 бита, като всеки такъв елемент ще
//        наричаме unit (U). Както входния (оригинален) файл, така и изходния (криптиран) файл се
//третират като състоящи се от unit-и. Криптираният файл винаги е точен брой unit-и (номерирани спрямо N0), като при нужда към оригиналният файл преди криптиране се добавят
//байтове 0x00 (padding), за да стане точен брой unit-и.
//• unit-ите в криптирания файл могат да се ползват за съхранение на следните типове данни (и
//∗Adams, Douglas. The Ultimate Hitchhiker’s Guide to the Galaxy: Five Novels in One Outrageous Volume. Del Rey,
//2010.
//техните големини): header (4U), section (2U), cryptdata (1U), unused data (1U).
//• типовете данни section и cryptdata са обработени с побитово изключващо или (XOR) със 128
//битов ключ (ключът е голям 1U), като за section ключът се ползва два пъти – за първият и
//        за вторият unit по отделно.
//• криптираният файл винаги започва с header (4U) и има следната структура:
//– uint64_t, magic – магическа стойност 0x0000534f44614c47, която дефинира, че файлът
//        е криптиран спрямо тази спецификация
//– uint32_t, cfsb – размер на криптирания файл в байтове
//– uint32_t, cfsu – размер на криптирания файл в брой unit-и
//– uint32_t, ofsb – размер на оригиналния файл в байтове
//– uint32_t, ofsu – размер на оригиналния файл в брой unit-и
//– uint32_t, unused1 – не се използва в тази версия на вируса
//– uint32_t, cksum – checksum-а на оригиналния файл, получена по следния алгоритъм:
//∗ оригиналният файл се третира като състоящ се от uint32_t елементи
//∗ при нужда от padding на последния елемент се ползват байтове 0x00
//∗ всички елементи се XOR-ват един с друг и полученият резултат е cksum
//– 128 бита†
//, sectionkey – ключ, с който са кодирани section-ите
//– четири “слота”, описващи начална позиция в U на section
//∗ uint32_t, s0
//∗ uint32_t, s1
//∗ uint32_t, s2
//∗ uint32_t, s3
//∗ тъй като unit-и с номера в интервала [0, 3] се ползват за самия header, в горните
//слотове 0 означава, че този слот не се използва (няма такъв section)
//∗ unit-ите в изходния (декриптиран) файл са по реда на секциите
//• section (2U) е кодиран със sectionkey от header-а и след разкодиране има следната структура:
//– int64_t, relative_offset – дефинира от кое U започват unit-ите с данни за тази секция.
//Числото е относително спрямо първият unit на section, например, ако section е записан в
//U 4/5 и стойността на offset е 2, то първите данни на секцията ще са в U 6.
//– uint64_t, len – дефинира брой unit-и в този section
//– 128 бита, datakey – ключ, с който са кодирани unit-ите с данни в този section
//– unit-ите в изходния (декриптиран) файл са по реда на unit-ите в секцията
//• cryptdata (1U) – unit в който има записани данни, кодирани с datakey на секцията, към която
//        е този unit
//• unused data (1U) – unit, който не се ползва. Колегите ви предполагат, че това е един от
//        начините чрез които вогоните са вкарвали “шум” в криптирания файл, но без да правят декриптирането невъзможно.
