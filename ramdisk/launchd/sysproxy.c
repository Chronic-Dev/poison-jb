#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

/* void exit(int status) */
void _exit(int status) {
	exit(status);
}

/* pid_t fork(void) */
pid_t _fork(void) {
	return fork();
}

/* ssize_t read(int fd, void *buf, size_t count) */
ssize_t _read(int fd, void *buf, size_t count) {
	return read(fd, buf, count);
}

/* ssize_t write(int fd, const void *buf, size_t count) */
ssize_t _write(int fd, const void *buf, size_t count) {
	return write(fd, buf, count);
}

/* int open(const char *pathname, int flags, mode_t mode) */
int _open(const char *pathname, int flags, mode_t mode) {
	return open(pathname, flags, mode);
}

/* int close(int fd) */
int _close(int fd) {
	return close(fd);
}

/* int unlink(const char *pathname) */
int _unlink(const char *pathname) {
	return unlink(pathname);
}

/* int stat(const char *path, struct stat *buf) */ 
int _stat(const char *path, struct stat *buf) {
	return stat(path, buf);
}

