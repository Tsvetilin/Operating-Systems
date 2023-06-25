#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/stat.h>
#include <stdint.h>

int fds[4];
int elSize[5] = {4, 1, 2, 8, 16};

struct Header {
	uint32_t unused;
	uint16_t len;
	uint16_t unused2;
	uint64_t unused3;
};

struct Header h;

struct Pair {
	uint16_t offset, len;
};

struct Pair p[4];

int openFile(char* file, int index);
int openFile(char* file, int index) {
	int fd;
	if((fd = open(file, O_RDONLY)) == -1)
		err(2, "ERROR: opening file for read: %s", file);

	if(read(fd, &h, sizeof(h)) != sizeof(h))
		err(5, "ERROR: reading header from file: %s", file);

	struct stat st;
	if(stat(file, &st) == -1)
		err(3, "ERROR: running stat on file: %s", file);
	
	if((st.st_size - 16) % elSize[index] != 0)
		errx(4, "ERROR: invalid file structure");

	if((st.st_size - 16) / elSize[index] != h.len)
		errx(6, "ERROR: invalid element count in file: %s", file);

	return fd;
}

int createFile(char* file);
int createFile(char* file) {
	int fd;
	if((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
		err(7, "ERROR: creating file for write: %s", file);

	if(write(fd, &h, sizeof(h)) != sizeof(h))
		err(8, "ERROR: writing initial header in file: %s", file);

	return fd;
}

void myLseek(int fd, int pos, int elementSize);
void myLseek(int fd, int pos, int elementSize) {
	if(lseek(fd, 16 + (pos*elementSize), SEEK_SET) == -1)
		err(9, "ERROR: running lseek() in file");
}

int myRead(int fd, uint64_t* buf, int size);
int myRead(int fd, uint64_t* buf, int size) {
	int bytes = 0;
	if((bytes = read(fd, buf, size)) == -1)
		err(11, "ERROR: reading from file. size: %d", size);
	
	return bytes;
}

int myWrite(int fd, uint64_t* buf, int size);
int myWrite(int fd, uint64_t* buf, int size) {
	int bytes = 0;
	if((bytes = write(fd, buf, size)) != size)
		err(12, "ERROR: writin %d bytes to file", size);
	
	return bytes;
}

int main(int argc, char** argv) {

	if(argc != 7)
		errx(1, "ERROR: params count. 6 expected");

	int fd1 = openFile(argv[1], 4);
	fds[0] = openFile(argv[2], 0); 
	fds[1] = openFile(argv[3], 1); 
	fds[2] = openFile(argv[4], 2); 
	fds[3] = openFile(argv[5], 3); 
	int fd2 = createFile(argv[6]); 

	struct Header newH = { 0, 0, 0, 0 };

	int bytes = 0;
	while((bytes = read(fd1, &p, elSize[4])) > 0) {
		for(int i = 0; i < 4; ++i) {
			myLseek(fds[i], p[i].offset, elSize[i]);

			uint64_t buf;
			for(uint16_t j = 0; j < p[i].len; ++j) {
				myRead(fds[i], &buf, elSize[i]);
				myWrite(fd2, &buf, elSize[i]);
			}

			newH.len += p[i].len;
		}	
	}
	if(bytes == -1)
		err(10, "ERROR: reading complects from file1");

	if(lseek(fd2, 0, SEEK_SET) == -1)
		err(12, "ERROR: reseting lseek on file6");

	if(write(fd2, &newH, sizeof(newH)) != sizeof(newH))
		err(13, "ERROR: adding final header to file6");

	close(fd1);
	close(fd2);
	for(int i = 0; i < 4; ++i)
		close(fds[i]);
	return 0;
}
