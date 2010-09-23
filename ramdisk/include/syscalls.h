#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>

#define STDOUT 0
#define STDIN  1
#define STDERR 2

//#define puts _puts
//#define open _open
//#define close _close
//#define read _read
//#define write _write

void _exit(int status);
pid_t _fork(void);
ssize_t _read(int fd, void *buf, size_t count);
ssize_t _write(int fd, const void *buf, size_t count);
int _open(const char *pathname, int flags, mode_t mode);
int _close(int fd);
int _unlink(const char *pathname);
int _stat(const char *path, struct stat *buf);

int _mount(const char *filesystemtype, const char *target, int mountflags, void *source);
int _unmount(const char *dir, int flags);

#endif
