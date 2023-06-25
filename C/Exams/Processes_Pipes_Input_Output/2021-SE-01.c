#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

struct Spaces {
	uint8_t s1, s2, s3;
};

struct Spaces s;

int createLogFile(void);
int createLogFile(void) {
	int fd;
	if((fd = open("file.log", O_WRONLY | O_APPEND | O_CREAT, 0644)) == -1)
		err(1, "ERROR: opening log file");

	return fd;
}

int appendToFile(int fd, const char* app, int error, const char* message);
int appendToFile(int fd, const char* app, int error, const char* message) {
	int bytes = 0;
	int len = strlen(app);
	if((bytes = write(fd, app, len)) != len)
		err(error, message);

	return bytes;
}

int readNextPid(int fd, char buf[30]);
int readNextPid(int fd, char buf[30]) {
	int bytes = 0;
	int totalBytes = 0;

	//if((bytes = read(fd, &s, sizeof(s))) != sizeof(s))
	//	err(4, "ERROR: reading 3 spaces from the pipe for the next pid");

	//if(bytes == 0)
	//	return 0;

	totalBytes += bytes;

	char ch = '\0';
	int index = 0;
	while((bytes = read(fd, &ch, sizeof(ch))) > 0) {
		if(bytes == 0)
			return 0;

		totalBytes += bytes;

		if(ch == ' ')
			continue;
		if(ch == '\n')
			break;

		buf[index++] = ch;
	}
	if(bytes < 0)
		err(5, "ERROR: reading pid from stdin");
	if(bytes == 0)
		return 0;

	buf[index++] = '\0';

	return totalBytes;
}

int ps(const char* username);
int ps(const char* username) {
	int pd[2];
	if(pipe(pd) == -1)
		err(6, "ERROR: running pipe for ps command");

	pid_t pid = fork();

	if(pid < 0)
		err(7, "ERROR: running fork for ps command");

	if(pid == 0) {
		close(pd[0]);

		dup2(pd[1], 1);
		close(pd[1]);

		if(execlp("ps", "ps", "-u", username, "-o", "pid=", (char*)NULL) == -1)
			err(8, "ERROR: running exec command in child process");
	}

	close(pd[1]);
	return pd[0];
}

void killProcess(char pidToKill[30]);
void killProcess(char pidToKill[30]) {
	pid_t pid = fork();

	if(pid < 0)
		err(9, "ERROR: running fork to kill a process");

	if(pid == 0) {
		if(execlp("kill", "kill", "-KILL", pidToKill, (char*)NULL) == -1)
			err(10, "ERROR: killing process with pid: %s", pidToKill);
	}
}

void lockUser(const char* username);
void lockUser(const char* username) {
	pid_t pid = fork();

	if(pid < 0)
		err(9, "ERROR: running fork to kill a process");	

	if(pid == 0) {
		if(execlp("passwd", "passwd", "-l", username, (char*)NULL) == -1)
			err(11, "ERROR: locking user with username: %s", username);
	}
}

int main(int argc, char** argv) {
	struct timeval tv;
	if(gettimeofday(&tv, NULL) == -1)
		err(13, "ERROR: running gettimeofday");
	
	struct tm* tm = localtime(&tv.tv_sec);
	if(tm == NULL)
		err(14, "ERROR: runninc localtime in main()");

	char buf[30];
	size_t size = strftime(buf, 30, "%F %T.",  tm);
	
	if(size <= 0)
		err(28, "ERROR: formating year / time for appending");

	uid_t uid = getuid();
	struct passwd* pwd = getpwuid(uid);
	if(pwd == NULL)
		err(15, "ERROR: extracting passwd struct");


	printf("UID: %d\nUSERNAME: %s\n", uid, pwd->pw_name);
	

	int logFd = createLogFile();
	int bytes = 0;
	bytes = appendToFile(logFd, buf, 16, "ERROR: appending date time to log");
	bytes = write(logFd, &tv.tv_usec, sizeof(tv.tv_usec));
	bytes = appendToFile(logFd, " \0", 18, "ERROR: appending space to log");
	bytes = appendToFile(logFd, pwd->pw_name, 17, "ERROR: appending username to log");
	bytes = appendToFile(logFd, " \0", 19, "ERROR: appending space to log");

	for(int i = 0; i < argc; ++i) {
		bytes = appendToFile(logFd, argv[i], 20, "ERROR: appending args to log");
		bytes = appendToFile(logFd, " \0", 21, "ERROR: appending space to log");
	}
	bytes = appendToFile(logFd, "\n\0", 22, "ERROR: appending new line to log");

	//lockUser(pwd->pw_name);
	int p = ps(pwd->pw_name);

	while((bytes = readNextPid(p, buf)) > 0) {
		if(bytes <= 0)
			err(23, "ERROR: reading next Pid from pipe. bytes read must be > 0");

		printf("PID: %s\n", buf);
		//killProcess(buf);
	}

	close(p);
	close(logFd);
	return 0;
}
