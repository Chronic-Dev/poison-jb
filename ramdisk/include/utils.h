#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syslog.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "reboot.h"
#include "hfs_mount.h"

#define cp _cp
#define puts _puts
#define mountHFS _mountHFS

void _puts(char *msg);
int _cp(char *src, char *dest);
int _mountHFS(char *device, char *mountdir, int options);
