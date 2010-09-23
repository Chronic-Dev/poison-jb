#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syslog.h>
#include <sys/stat.h>
#include "hfs_mount.h"
#include "reboot.h"
#include <sys/resource.h>

void _puts(char *msg);
int mountHFS(char *device, char *mountdir, int options);

int copy(char *src, char *dest);
