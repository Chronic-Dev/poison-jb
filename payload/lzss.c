/*
 *
 *  greenpois0n - kernel.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#define N         4096  /* size of ring buffer - must be power of 2 */
#define F         18    /* upper limit for match_length */
#define THRESHOLD 2     /* encode string into position and length
                           if match_length is greater than this */
#define NIL       N     /* index for root of binary search trees */

int lzss_decompress(unsigned char *dst, unsigned int dstlen, unsigned char *src, unsigned int srclen) {
    /* ring buffer of size N, with extra F-1 bytes to aid string comparison */
    unsigned char text_buf[N + F - 1];
    unsigned char *dststart = dst;
	unsigned char *dstend = dst + dstlen;
    unsigned char *srcend = src + srclen;
    int  i, j, k, r, c;
    unsigned int flags;
    
    dst = dststart;
    srcend = src + srclen;
    for (i = 0; i < N - F; i++)
        text_buf[i] = ' ';
    r = N - F;
    flags = 0;
    for ( ; ; ) {
        if (((flags >>= 1) & 0x100) == 0) {
            if (src < srcend) c = *src++; else break;
            flags = c | 0xFF00;  /* uses higher byte cleverly */
        }   /* to count eight */
        if (flags & 1) {
            if (src < srcend) c = *src++; else break;
            *dst++ = c;
			if (dst >= dstend) {
				goto finish;
			}
            text_buf[r++] = c;
            r &= (N - 1);
        } else {
            if (src < srcend) i = *src++; else break;
            if (src < srcend) j = *src++; else break;
            i |= ((j & 0xF0) << 4);
            j  =  (j & 0x0F) + THRESHOLD;
            for (k = 0; k <= j; k++) {
                c = text_buf[(i + k) & (N - 1)];
                *dst++ = c;
				if (dst >= dstend) {
					goto finish;
				}
                text_buf[r++] = c;
                r &= (N - 1);
            }
        }
    }

finish:
    return dst - dststart;
}
