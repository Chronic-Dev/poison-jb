/*
 *
 *  greenpois0n - include/framebuffer.h
 *  (c) 2009 Chronic-Dev Team
 *
 */

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define COLOR_WHITE 0xffffff
#define COLOR_BLACK 0x0

typedef struct Font {
	unsigned int width;
	unsigned int height;
	unsigned char data[];
} Font;

extern Bool gFbHasInit;

int fb_init();
void fb_clear();
void fb_display_text(Bool option);
void fb_set_loc(unsigned int x, unsigned int y);
unsigned int fb_get_x();
unsigned int fb_get_y();
unsigned int fb_get_width();
unsigned int fb_get_height();
void fb_putc(int c);
void fb_print(const char* str);
void fb_print_force(const char* str);
void fb_draw_image(unsigned int* image, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void fb_capture_image(unsigned int* image, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
unsigned int* fb_load_image(const char* data, unsigned int len, unsigned int* width, unsigned int* height, unsigned int alpha);

#endif /* FRAMEBUFFER_H */
