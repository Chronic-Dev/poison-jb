/*
 *
 *  greenpois0n - image.c
 *  (c) 2009 Chronic-Dev Team
 *
 */

#include <string.h>

#include "font.h"
#include "device.h"
#include "common.h"
#include "framebuffer.h"

static Font* gFbFont;
static unsigned int gFbX;
static unsigned int gFbY;
static unsigned int gFbTWidth;
static unsigned int gFbTHeight;
static unsigned int gFbWidth;
static unsigned int gFbHeight;

Bool gFbHasInit = FALSE;
Bool gFbDisplayText = FALSE;

static unsigned int gFbBackgroundColor;
static unsigned int gFbForegroundColor;

inline int font_get_pixel(Font* font, int ch, int x, int y) {
	register int index = ((font->width * font->height) * ch) + (font->width * y) + x;
	return (font->data[index / 8] >> (index % 8)) & 0x1;
}

volatile unsigned int* fb_get_pixel(register unsigned int x, register unsigned int y) {
	return (((unsigned int*)FRAMEBUFFER) + (y * gFbWidth) + x);
}

static void fb_scrollup() {
	register volatile unsigned int* newFirstLine = fb_get_pixel(0, gFbFont->height);
	register volatile unsigned int* oldFirstLine = fb_get_pixel(0, 0);
	register volatile unsigned int* end = oldFirstLine + (gFbWidth * gFbHeight);
	while(newFirstLine < end) {
		*(oldFirstLine++) = *(newFirstLine++);
	}
	while(oldFirstLine < end) {
		*(oldFirstLine++) = gFbBackgroundColor;
	}
	gFbY--;
}

void fb_setup() {
	gFbFont = (Font*) font_data;
	gFbBackgroundColor = COLOR_BLACK;
	gFbForegroundColor = COLOR_WHITE;
#ifdef S5L8930X
	gFbWidth = 640;
	gFbHeight = 960;
#else
	gFbWidth = 320;
	gFbHeight = 480;
#endif
	gFbTWidth = gFbWidth / gFbFont->width;
	gFbTHeight = gFbHeight / gFbFont->height;
}

int fb_init() {
	if(gFbHasInit) return 0;
	fb_setup();
	fb_clear();
    fb_set_loc(0,0);
	fb_display_text(TRUE);

	fb_print("=====================================================");
#ifdef S5L8930X
	fb_print("=====================================================");
	fb_print("                          ");
#endif

	fb_print("                     greenpois0n                     ");

#ifdef S5L8930X
	fb_print("                                                     ");
#endif

	fb_print("              http://www.greenpois0n.com             ");

#ifdef S5L8930X
	fb_print("                           ");
	fb_print("=====================================================");
#endif
	fb_print("=====================================================");

	gFbHasInit = TRUE;
	return 0;
}

void fb_clear() {
    unsigned int *p = 0;
	for(p = (unsigned int*)FRAMEBUFFER; p < (unsigned int*)(FRAMEBUFFER + (gFbWidth * gFbHeight * 4)); p++) {
        *p = gFbBackgroundColor;
    }
}

unsigned int fb_get_x() {
	return gFbX;
}

unsigned int fb_get_y() {
	return gFbY;
}

unsigned int fb_get_width() {
	return gFbTWidth;
}

unsigned int fb_get_height() {
	return gFbTHeight;
}

void fb_set_loc(unsigned int x, unsigned int y) {
	gFbX = x;
	gFbY = y;
}

void fb_set_colors(unsigned int fore, unsigned int back) {
	gFbForegroundColor = fore;
	gFbBackgroundColor = back;
}

void fb_display_text(Bool option) {
	gFbDisplayText = option;
}

void fb_putc(int c) {
	if(c == '\r') {
		gFbX = 0;

	} else if(c == '\n') {
		gFbX = 0;
		gFbY++;

	} else {
		register unsigned int sx;
		register unsigned int sy;

		for(sy = 0; sy < gFbFont->height; sy++) {
			for(sx = 0; sx < gFbFont->width; sx++) {
				if(font_get_pixel(gFbFont, c, sx, sy)) {
					*(fb_get_pixel(sx + (gFbFont->width * gFbX), sy + (gFbFont->height * gFbY))) = gFbForegroundColor;
				} else {
					*(fb_get_pixel(sx + (gFbFont->width * gFbX), sy + (gFbFont->height * gFbY))) = gFbBackgroundColor;
				}
			}
		}

		gFbX++;
	}

	if(gFbX == gFbTWidth) {
		gFbX = 0;
		gFbY++;
	}

	if(gFbY == gFbTHeight) {
		fb_scrollup();
	}
}

void fb_print(const char* str) {
	if(!gFbDisplayText)
		return;

	unsigned int len = strlen(str);
	int i;
	for(i = 0; i < len; i++) {
		fb_putc(str[i]);
	}
}

void fb_print_force(const char* str) {
	size_t len = strlen(str);
	int i;
	for(i = 0; i < len; i++) {
		fb_putc(str[i]);
	}
}

void fb_draw_image(unsigned int* image, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	register unsigned int sx;
	register unsigned int sy;
	for(sy = 0; sy < height; sy++) {
		for(sx = 0; sx < width; sx++) {
			*(fb_get_pixel(sx + x, sy + y)) = image[(sy * width) + sx];
		}
	}
}

void fb_capture_image(unsigned int* image, unsigned int x, unsigned int y, unsigned int width, unsigned int height) {
	register unsigned int sx;
	register unsigned int sy;
	for(sy = 0; sy < height; sy++) {
		for(sx = 0; sx < width; sx++) {
			image[(sy * width) + sx] = *(fb_get_pixel(sx + x, sy + y));
		}
	}
}