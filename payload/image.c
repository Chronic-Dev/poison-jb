/*
 *
 *  greenpois0n - payload/src/image.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#include <stdio.h>

#include "aes.h"
#include "bdev.h"
#include "lock.h"
#include "task.h"
#include "image.h"
#include "common.h"

LinkedList* gImageList = SELF_IMAGE_LIST;

int image_init() {
	cmd_add("image", &image_cmd, "display and operate on for firmware images");
	return 0;
}

int image_cmd(int argc, CmdArg* argv) {
	unsigned int image = 0;
	void* address = NULL;
	char* action = NULL;
	if(argc < 2) {
		puts("usage: task <list/load> [options]\n");
		puts("  list                   \t\tdisplay list of active tasks\n");
		puts("  load <image> <address> \t\tdisplay list of active tasks\n");
		return 0;
	}

	action = argv[1].string;
	if(!strcmp(action, "list")) {
		image_display_list();
	}

	if(argc == 4) {
		if(!strcmp(action, "load")) {
			image = argv[2].uinteger;
			address = (void*) argv[3].uinteger;
			return image_load(image, address, 0x100000);
		}
	}

	return 0;
}

void image_display_list() {
	char type[] = "type";
	LinkedList* list = gImageList->next;

	enter_critical_section();
	printf("Images:\n");
	while(list != gImageList) {
		ImageDescriptor* image = (ImageDescriptor*)list;
		type[0] = (image->info.imageIdentifier & 0xFF000000) >> 24;
		type[1] = (image->info.imageIdentifier & 0xFF0000) >> 16;
		type[2] = (image->info.imageIdentifier & 0xFF00) >> 8;
		type[3] = (image->info.imageIdentifier & 0xFF);
		type[4] = '\0';
		printf("  %p: bdev: %p type: %s offset: 0x%05x len: 0x%x\n", image,
				image->device, type, image->startAddress, image->info.imageSize);

		list = image->list.next;
	}
	printf("\n");
	exit_critical_section();
}

void* image_find_tag(void* image, unsigned int tag, unsigned int size) {
	unsigned int i = 0;
	unsigned int* current = image;
	current = image;
	for (i = 0; i < size; i++) {
		if (*current == tag) {
			return current;
		}
		if(i % 0x1000) task_yield();
		current++;
	}
	return 0;
}

void image_decrypt(void* image) {
	void* data = NULL;

	ImageHeader* header = (ImageHeader*) image;
	data = image_find_tag(image, IMAGE_DATA, header->fullSize);

	if (data == 0) {
		puts("Unable to find DATA tag\n");
		return;
	}

	ImageKbag* kbag = (ImageKbag*) image_find_tag(image, IMAGE_KBAG, header->fullSize);
	if (kbag == 0) {
		puts("Unable to find KBAG tag\n");
		return;
	}

	ImageTagHeader* data_header = (ImageTagHeader*) data;
    data = (void*) data_header + sizeof(ImageTagHeader);

    /* Decrypt kbag */
    // FIXME: derive AES size from kbag length
	aes_crypto_cmd(kAesDecrypt, kbag->iv, kbag->iv, 0x30, kAesTypeGid, 0, 0);

    /* Decrypt data */
    //FIXME: derive AES type from kbag type
	aes_crypto_cmd(kAesDecrypt, data, data, (data_header->dataSize - (data_header->dataSize % 16)), kAesType256, kbag->key, kbag->iv);
}

ImageDescriptor* image_find(unsigned int signature) {
	LinkedList* list = gImageList->next;
	while(list != gImageList) {
		ImageDescriptor* image = (ImageDescriptor*) list;
		if(image->info.imageIdentifier == signature) {
			return image;
		}
		list = image->list.next;
	}
	return NULL;
}

int image_load(unsigned int signature, void* dataout, unsigned int maxsize) {
	ImageDescriptor* image = image_find(signature);
	if(image == NULL) {
		puts("unable to find requested image\n");
		return -1;
	}

	image->device->read(image->device, dataout,
			(void*) image->startAddress, 0, image->info.imageSize);

	image_decrypt(dataout);
}
