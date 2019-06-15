#include <bit_manip.h>

uint8_t reverse8(uint8_t i) {
   
   register uint8_t b = i;
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

uint16_t reverse16(uint16_t i) {
   
   register uint16_t b = i;
   b = (b & 0xFF00) >> 8 | (b & 0x00FF) << 8;
   b = (b & 0xF0F0) >> 4 | (b & 0x0F0F) << 4;
   b = (b & 0xCCCC) >> 2 | (b & 0x3333) << 2;
   b = (b & 0xAAAA) >> 1 | (b & 0x5555) << 1;
   return b;
}

uint16_t *load16(U16x16 s, const U16x16 cs) {
	
	register uint8_t *s_ = (uint8_t *)s;
	register const uint8_t *cs_ = (const uint8_t *)cs;
	uint8_t i;
	for (i=0; i<32; i++) 
		*s_++ = *cs_++;
		
	return s;
}

uint16_t *hflip16(U16x16 s) {
	
	uint8_t i=0;
	for (i=0; i<16; i++)
		s[i] = reverse16(s[i]);
		
	return s;
}

uint16_t *rotate16(U16x16 s) {

	uint16_t tmp[16];
	register uint16_t i,j;
	register uint16_t *src, *tgt;

	
	src = &s[15];
	for (i=0; i<16; i++) {
		register uint16_t sv = *src--;
		tgt = &tmp[15];
		for (j=0; j<16; j++) {
			register uint16_t t = *(uint16_t *)tgt;
			t += t + (sv&1);
			sv = sv>>1;
			*(uint16_t *)tgt = t;
			tgt--;
		}
	}

	src = tmp;
	tgt = s;
	for (i=0; i<16; i++) 
		*tgt++ = *src++;
	
	return s;
}
