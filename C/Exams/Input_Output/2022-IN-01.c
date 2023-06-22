#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct{
        uint16_t magic;
        uint16_t filetype;
        uint32_t count;
} header;

void lseek_safe(int fd, size_t pos);
void close_all(void);
int open_safe_create(const char* file);
int open_safe(const char* file, int oflag);
void read_safe(int fd, void* toWhere, ssize_t howMany ,const char* file_name);
void write_safe(int fd, void* what, ssize_t howMany,const char* file_name);

int fds[5] = {-1,-1,-1,-1,-1};

void read_safe(int fd, void* toWhere, ssize_t howMany ,const char* file_name)
{
        int bytes_read = read(fd, toWhere, howMany);

        if(bytes_read<0)
        {
                close_all();
                err(1, "Couldn't read from file %s", file_name);
        }

        if(bytes_read != howMany)
        {
                close_all();
                errx(2, "Couldn't read all data from file %s.",file_name);
        }
}


void write_safe(int fd, void* what, ssize_t howMany,const char* file_name)
{
        int bytes_write = write(fd, what, howMany);

        if(bytes_write < 0)
        {
                close_all();
                err(3, "Couldn't write to file %s", file_name);
        }

        if(bytes_write != howMany)
        {
                close_all();
                errx(4,"Couldn't write all data to file %s.", file_name);
        }
}

int open_safe(const char* file, int oflag)
{
        int fd = open(file, oflag);
        if(fd == -1)
        {
                close_all();
                errx(6, "Couldn't open file %s", file);
        }

        return fd;
}

int open_safe_create(const char* file)
{
        int fd = open(file, O_CREAT | O_WRONLY | O_RDONLY , S_IRUSR | S_IWUSR);
        if(fd == -1)
        {
                close_all();
                errx(6, "Couldn't open file %s", file);
        }

        return fd;
}

void close_all()
{
        int old_errno = errno;
        for(size_t i = 0; i < 5; i++)
        {
                if(fds[i]!=-1)
                {
                        close(fds[i]);
                }
        }

        errno=old_errno;
}

void lseek_safe(int fd, size_t pos)
{
        if(lseek(fd,pos,SEEK_SET)<0)
        {
                close_all();
                err(5,"Couldn't lseek");
        }
}



int main(int argc, char** argv)
{
        if(argc != 4)
        {
                errx(10,"Invalid args count!");
        }

        fds[0] = open_safe(argv[1], O_RDONLY);
        fds[1] = open_safe(argv[2], O_RDONLY);
        fds[2] = open_safe_create(argv[3]);

        header hList,hData,hOut;
        read_safe(fds[0],&hList, sizeof(hList), argv[1]);
        read_safe(fds[1], &hData, sizeof(hData),argv[2]);

        size_t headerLen = sizeof(header);
        const uint16_t magic_check = 0x5A4D;

        if(hList.magic != magic_check || hData.magic!=magic_check)
        {
                close_all();
                errx(11,"Files not in format for the Hoge system.");
        }

        if(hList.filetype != 1 || hData.filetype!=2 )
        {
                close_all();
                errx(12,"Invalid file types supplied.");
        }


        struct stat sList, sData;
        if(fstat(fds[0], &sList) == -1 || fstat(fds[1], &sData) ==-1 )
        {
                close_all();
                err(12, "Couldn't get stats for file.");
        }

        if( (sList.st_size-headerLen) / sizeof(uint16_t) != hList.count || (sData.st_size -headerLen) /sizeof(uint32_t) != hData.count)
        {
                close_all();
                errx(13,"File length not matching elements count.");
        }

        if(hList.count>hData.count)
        {
                close_all();
                errx(14,"No position in data matching a position in list for some postion in list.");
        }


        uint16_t maxValue = 0;
        uint16_t v;
        for(size_t i = 0; i < hList.count; i++){
                read_safe(fds[0],&v, sizeof(v),argv[1]);
                if(v>maxValue)
                {
                        maxValue = v;
                }
        }

        hOut.count = maxValue;
        hOut.magic = magic_check;
        hOut.filetype = 3;

        write_safe(fds[2], &hOut, sizeof(hOut), argv[3]);
        uint64_t dummy = 0;

        for(size_t i = 0; i < maxValue; i++)
        {
                write_safe(fds[2], &dummy, sizeof(dummy), argv[3]);
        }

        size_t position = 0;
        uint16_t value;
        uint32_t data;
        uint64_t toWrite;
        while(position < hList.count)
        {
                lseek_safe(fds[0], headerLen + position * sizeof(uint16_t));
                read_safe(fds[0], &value, sizeof(value), argv[1]);

                lseek_safe(fds[1],headerLen + position * sizeof(uint32_t));
                read_safe(fds[1], &data, sizeof(data), argv[2]);

                toWrite = data;
                lseek_safe(fds[2], headerLen + value * sizeof(uint64_t));
                write_safe(fds[2], &toWrite, sizeof(toWrite), argv[3]);

                position++;
        }

        close_all();
        return 0;
}

//Вашите колеги от съседната лаборатория работят със специализирана система Hoge,
//която съхранява данните си в binary файлове. За съжаление обаче им се налага да работят с две
//различни версии на системата (стара и нова) и разбира се, те ползват различни файлови формати.
//Вашата задача е да напишете програма на C (./main list.bin data.bin out.bin), която конвертира файлове с данни от стария (list.bin + data.bin) в новия (out.bin) формат.
//Всички файлове се състоят от две секции – 8 байтов хедър и данни. Структурата на хедъра е:
//• uint16_t, magic – магическа стойност 0x5A4D, която дефинира, че файлът е от системата Hoge
//и следва тази спецификация;
//• uint16_t, filetype – дефинира какъв тип файл е това, с допустими стойности 1 за list.bin,
//2 за data.bin и 3 за out.bin;
//• uint32_t, count – дефинира броя на елементите в секцията с данни на файла.
//Секцията за данни на всички файлове съдържа елементи – цели числа без знак, от съответните
//типове:
//• list.bin – uint16_t;
//• data.bin – uint32_t;
//• out.bin – uint64_t.
//Във файлове data.bin и out.bin елементи, чиято стойност е 0, са валидни елементи, но се игнорират от системата Hoge (тяхното наличие във файла не пречи на работата на системата).
//Всеки елемент в секцията за данни на list.bin има две характеристики – позиция и стойност,
//като позиция е отместването на съответния елемент, докато стойност е неговата стойност. Тези
//две числа семантично дефинират отмествания във файловете с данни:
//• позиция дефинира отместване в data.bin;
//• стойност дефинира отместване в out.bin.
//
//На базата на тези числови характеристики елементите на list.bin дефинират правила от вида:
//“елементът на отместване позиция в data.bin трябва да отиде на отместване стойност в out.bin”.
//Забележка: Всички отмествания са спрямо N0 в брой елементи.
//Програмата трябва да интепретира по ред дефинираните правила и на тяхна база да генерира
//изходния файл.
//Обърнете внимание на проверките, обработката за грешки и съобщенията към потребителя – искаме
//програмата да бъде удобен инструмент.
//Забележка: Не е гарантирано, че разполагате с достатъчно памет, в която да заредите всички
//елементи на който и да от файловете.