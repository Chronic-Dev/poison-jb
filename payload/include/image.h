/*
 *
 *  greenpois0n - payload/include/image.h
 *  (c) 2010 Chronic-Dev Team
 *
 */

#ifndef IMAGE_H
#define IMAGE_H

#include "common.h"
#include "device.h"
#include "offsets.h"
#include "commands.h"

#ifdef TARGET_IMAGE_LIST
#	define SELF_IMAGE_LIST ((void*)(TARGET_BASEADDR + TARGET_IMAGE_LIST))
#endif

#ifndef SELF_IMAGE_LIST
#	define SELF_IMAGE_LIST 0
#	error "SELF_IMAGE_LIST not defined"
#endif

#define IMAGE_IMG3       0x496D6733
#define IMAGE_8900       0x30303938

#define IMAGE_IBOOT      0x69626F74
#define IMAGE_LLB        0x696C6C62

#define IMAGE_DATA       0x44415441
#define IMAGE_KBAG       0x4B424147
#define IMAGE_COMP       0x636F6D70
#define IMAGE_LZSS       0x6C7A7373

typedef struct Image8900Header {
    unsigned int signature;
    unsigned char version[3];
    unsigned char format;
    unsigned int unk1;
    unsigned int dataSize;
    unsigned int footerSignatureOffset;
    unsigned int footerCertOffset;
    unsigned int footerCertSize;
    unsigned char salt[0x20];
    unsigned short unk2;
    unsigned short epoch;
    unsigned char headerSignature[0x10];
    unsigned char padding[0x7B0];
} Image8900Header;

typedef struct ImageHeader {
	unsigned int signature;
	unsigned int fullSize;
	unsigned int dataSize;
	unsigned int shshOffset;
	unsigned int imageType;
} ImageHeader;

typedef struct ImageTagHeader {
	unsigned int signature;
	unsigned int fullSize;
	unsigned int dataSize;
} ImageTagHeader;

typedef struct ImageKbag {
	ImageTagHeader header;
	unsigned int state;
	unsigned int type;
	unsigned char iv[16];
	unsigned char key[32];
} ImageKbag;

typedef struct ImageComp {
    unsigned int signature;
    unsigned int compressionType;
    unsigned int checksum;
    unsigned int compressedSize;
    unsigned int uncompressedSize;
    unsigned char padding[0x16C];
} ImageComp;

typedef struct ImageDescriptor ImageDescriptor;
typedef struct ImageInfo {
	int dataSize;
	int imageSize;
	int imageIdentifier;
	int imageType;
	int unk2;
	ImageDescriptor* current;
} ImageInfo;

struct ImageDescriptor {
	LinkedList list;
	struct BdevDescriptor* device;
	unsigned int startAddress;
	int unk1;
	ImageInfo info;
};

extern LinkedList* gImageList;

int image_init();
int image_cmd(int argc, CmdArg* argv);
void image_display_list();
void image_decrypt(void* image);
ImageDescriptor* image_find(unsigned int signature);
void* image_find_tag(void* image, unsigned int tag, unsigned int size);
int image_load(unsigned int signature, void* dataout, unsigned int maxsize);

#endif
