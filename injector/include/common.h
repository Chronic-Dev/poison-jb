#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#define info(...) printf(__VA_ARGS__)
#define error(...) fprintf(stderr, __VA_ARGS__)
#define debug(...) if(greenpois0n_debug) fprintf(stderr, __VA_ARGS__)

#define CPID_UNKNOWN        -1
#define CPID_IPHONE2G     8900
#define CPID_IPOD1G       8900
#define CPID_IPHONE3G     8900
#define CPID_IPOD2G       8720
#define CPID_IPHONE3GS    8920
#define CPID_IPOD3G       8922
#define CPID_IPAD1G       8930
#define CPID_IPHONE4      8930
#define CPID_IPOD4G       8930

#define BDID_UNKNOWN        -1
#define BDID_IPHONE2G        0
#define BDID_IPOD1G          2
#define BDID_IPHONE3G        4
#define BDID_IPOD2G          0
#define BDID_IPHONE3GS       0
#define BDID_IPOD3G          2
#define BDID_IPAD1G          2
#define BDID_IPHONE4         0
#define BDID_IPOD4G          8

#define DEVICE_UNKNOWN      -1
#define DEVICE_IPHONE2G      0
#define DEVICE_IPOD1G        1
#define DEVICE_IPHONE3G      2
#define DEVICE_IPOD2G        3
#define DEVICE_IPHONE3GS     4
#define DEVICE_IPOD3G        5
#define DEVICE_IPAD1G        6
#define DEVICE_IPHONE4       7
#define DEVICE_IPOD4G        8


#define MODE_UNKNOWN        -1
#define MODE_DFU             0
#define MODE_RECOVERY        1
#define MODE_RESTORE         2
#define MODE_NORMAL          3

#define FLAG_QUIT            1
#define FLAG_DEBUG           2
#define FLAG_ERASE           4
#define FLAG_CUSTOM          8
#define FLAG_EXCLUDE        16

struct greenpois0n_mode_t {
	int index;
	const char* string;
};

struct greenpois0n_device_t {
	int index;
	const char* product;
	const char* model;
	uint32_t board_id;
	uint32_t chip_id;
	const char* url;
};

static struct greenpois0n_device_t greenpois0n_devices[] = {
	{  0, "iPhone1,1", "m68ap",  0,  8900,
	NULL },
	{  1, "iPod1,1",   "n45ap",  2,  8900,
	NULL },
	{  2, "iPhone1,2", "n82ap",  4,  8900,
	NULL },
	{  3, "iPod2,1",   "n72ap",  0,  8720,
	NULL },
	{  4, "iPhone2,1", "n88ap",  0,  8920,
	NULL },
	{  5, "iPod3,1",   "n18ap",  2,  8922,
	NULL },
	{  6, "iPad1,1",   "k48ap",  2,  8930,
	"http://appldnld.apple.com/iPad/061-8801.20100811.CvfR5/iPad1,1_3.2.2_7B500_Restore.ipsw" },
	{  7, "iPhone3,1", "n90ap",  0,  8930,
	"http://appldnld.apple.com/iPhone4/061-7939.20100908.Lcyg3/iPhone3,1_4.1_8B117_Restore.ipsw" },
	{  8, "iPod4,1",   "n81ap",  8,  8930,
	"http://appldnld.apple.com/iPhone4/061-8490.20100901.hyjtR/iPod4,1_4.1_8B117_Restore.ipsw" },
	{ -1,  NULL,        NULL,   -1,    -1,
	NULL }
};

extern int greenpois0n_debug;

void print_progress_bar(double progress);
int read_file(const char* filename, void** data, int* size);
int write_file(const char* filename, const void* data, int size);

#endif
