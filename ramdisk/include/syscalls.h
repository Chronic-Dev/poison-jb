#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <sys/stat.h>

#define STDOUT          0x0000
#define STDIN           0x0001
#define STDERR          0x0002

#define O_RDONLY        0x0000
#define O_WRONLY        0x0001
#define O_RDWR          0x0002
#define O_CREAT         0x0200
#define O_TRUNC         0x0400
#define O_EXCL          0x0800

void exit(int status);
int fork(void);
int read(int fd, void *buf, unsigned int count);
int write(int fd, const void *buf, unsigned int count);
int open(const char *pathname, int flags, ...);
int close(int fd);
int unlink(const char *pathname);
int stat(const char *path, struct stat *buf);

int mount(const char *filesystemtype, const char *target, int mountflags, void *source);
int unmount(const char *dir, int flags);

#endif