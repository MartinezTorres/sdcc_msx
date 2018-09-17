#include <msx1hal.h>

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

void TMS9918_setRegister(uint8_t reg, uint8_t val) {
	
	TMS9918Status.reg[reg] = val; 
	TMS9918_writeRegister(reg);
}

void TMS9918_setFlags(uint16_t flags) {
	
	TMS9918Status.flags = flags; 
	TMS9918_writeRegister(0);
	TMS9918_writeRegister(1);
}

void TMS9918_setMode2(uint8_t use3pages) {

	TMS9918Status.flags = TMS9918_M2 | TMS9918_BLANK | TMS9918_GINT | TMS9918_MEM416K;

	if (use3pages) {
		TMS9918Status.ct6  = (ADDRESS_CT  >>  6) | 0b01111111;
		TMS9918Status.pg11 = (((int16_t)ADDRESS_PG)  >> 11) | 0b00000011;
	} else {
		TMS9918Status.ct6  = (ADDRESS_CT  >>  6) | 0b00011111;
		TMS9918Status.pg11 = (((int16_t)ADDRESS_PG)  >> 11) | 0b00000000;
	}
	
	TMS9918Status.pn10 =  ADDRESS_PN0 >> 10;
	TMS9918Status.sa7  =  ADDRESS_SA0 >>  7;
	TMS9918Status.sg11 =  ADDRESS_SG  >> 11;
	
	TMS9918Status.backdrop  = BBlack;
	TMS9918Status.textcolor = BWhite;
	
	{
		uint8_t i=0;
		for (i=0; i<8; ++i)
			TMS9918_writeRegister(i);
	}
	
	TMS9918_memset(ADDRESS_PN0, 0, sizeof(T_PN));
}

void TMS9918_activatePage0() { 
	
	TMS9918Status.pn10 = ADDRESS_PN0 >> 10; TMS9918_writeRegister(2);
	TMS9918Status.sa7  = ADDRESS_SA0 >>  7; TMS9918_writeRegister(5);
}

void TMS9918_activatePage1() {
	
	TMS9918Status.pn10 = ADDRESS_PN1 >> 10; TMS9918_writeRegister(2);
	TMS9918Status.sa7  = ADDRESS_SA1 >>  7; TMS9918_writeRegister(5);
}



// 16 bit pattern manipulation functions

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

uint16_t *shuffle16(U16x16 s) {

	uint8_t tmp[32];
	register uint8_t i;
	register uint8_t *src, *tgt;

	tgt = ((uint8_t *)tmp)+0;
	src = ((uint8_t *)s)+1;
	for (i=0; i<16; i++) {
		*tgt = *src;
		tgt++; src+=2;
	}

	src = ((uint8_t *)s)+0;
	for (i=0; i<16; i++) {
		*tgt = *src;
		tgt++; src+=2;
	}

	tgt = ((uint8_t *)s)+0;
	src = ((uint8_t *)tmp)+0;
	for (i=0; i<32; i++) 
		*tgt++ = *src++;

	return s;
}

uint16_t *load16(U16x16 s, const U16x16 cs) {
	
	register uint8_t *s_ = (uint8_t *)s;
	register const uint8_t *cs_ = (const uint8_t *)cs;
	uint8_t i;
	for (i=0; i<32; i++) 
		*s_++ = *cs_++;
		
	return s;
}

void writeSprite16(uint8_t pos, U16x16 s) {
	shuffle16(s);
	TMS9918_memcpy(ADDRESS_SG+(((uint16_t)pos)<<3), (const uint8_t *)s, 32);
}

// Tile and ASCII rutines
static uint8_t ascii2tiles[256]; // Tiles 0x10 to 0x1F reserved for the bloodmeter.

// Precondition, freeTiles must have at least 1 spot!
uint8_t addASCIITile(uint8_t freeTiles[256], uint8_t ascii, const U8x8 shape, const U8x8 color) {
	
	uint8_t freeTile = 0;
	while (freeTiles[freeTile]==0) freeTile++;
	
	TMS9918_memcpy(ADDRESS_PG + (((uint16_t)freeTile)<<3), shape, 8);
	TMS9918_memcpy(ADDRESS_CT + (((uint16_t)freeTile)<<3), color, 8);

	freeTiles[freeTile] = 0;
	ascii2tiles[ascii] = freeTile;
	return freeTile;
}

// Text rutines
void uint16ToString(char *msg, uint16_t v, uint8_t enableZeroPad) {
	
	register char *p = msg;
	char c;
	c = 0; while (v>10000) { c++; v-=10000; } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>1000 ) { c++; v-=1000;  } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>100  ) { c++; v-=100;   } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>10   ) { c++; v-=10;    } if (c || enableZeroPad) *p++ = '0'+c;
	c = 0; while (v>1    ) { c++; v-=1;     } if (c) *p++ = '0'+c;
	*p++=0;
}

void printStr(uint16_t baseAddress, uint8_t x, uint8_t y, const char *msg) {

	uint8_t tiles[32];
	register uint8_t l = 0;
	{
		register const char *src = msg;
		register uint8_t *dst = &tiles[0];
		while (TRUE) {
			register uint8_t v = *src++;
			if (v==0) break;
			l++;
			*dst++ = ascii2tiles[v];
		}
	}
	TMS9918_memcpy(baseAddress + x+(y<<5),tiles,l);
}

