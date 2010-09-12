#include <stdio.h>
#include "common.h"
#include "partial.h"

char endianness = IS_LITTLE_ENDIAN;

int download_file_from_zip(const char* url, const char* path, const char* output) {
	FILE* fd;
	CDFile* file;
	ZipInfo* info;
	unsigned int size;
	unsigned char* data;

	info = PartialZipInit(url);
	if (!info) {
		printf("Cannot find %s\n", url);
		return -1;
	}

	file = PartialZipFindFile(info, path);
	if (!file) {
		printf("Cannot find %s in %s\n", path, url);
		return -1;
	}

	data = PartialZipGetFile(info, file);
	if(!data) {
		printf("Cannot get %s from %s\n", path, url);
		return -1;
	}
	size = file->size;

	fd = fopen(output, "wb");
	if(!fd) {
		printf("Cannot open file %s for output\n", output);
		return -1;
	}

	if(fwrite(data, 1, size, fd) != size) {
		printf("Unable to write entire file to output\n");
		return -1;
	}

	PartialZipRelease(info);
	free(data);
	return 0;
}
