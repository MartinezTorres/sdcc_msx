#include <tms99X8.h>

////////////////////////////////////////////////////////////////////////
// Mode2 Interface

void TMS99X8_clear() {
	
	TMS99X8_setFlags(TMS99X8_M2 | TMS99X8_GINT | TMS99X8_MEM416K);
	TMS99X8_memset(0,0,16*1024);
	TMS99X8_setFlags(TMS99X8_M2 | TMS99X8_ENABLE | TMS99X8_GINT | TMS99X8_MEM416K);
	
	TMS99X8_memset(MODE2_ADDRESS_PN0, 0, sizeof(T_PN));
	TMS99X8_memset(MODE2_ADDRESS_PN1, 0, sizeof(T_PN));
	TMS99X8_memset(MODE2_ADDRESS_SA0, 208, sizeof(T_PN));
	TMS99X8_memset(MODE2_ADDRESS_SA1, 208, sizeof(T_PN));	
}

void TMS99X8_activateMode2 (EM2_RowPageFlags rowPages) {
	
	uint8_t pgPageMask = 0;
	switch ((uint8_t)rowPages) {
		case MODE2_ROWS_0__7:
			pgPageMask = 0;
			break;
		case MODE2_ROWS_0__7 + MODE2_ROWS_8_15:
			pgPageMask = 1;
			break;
		case MODE2_ROWS_0__7 + MODE2_ROWS16_23:
			pgPageMask = 2;
			break;
		case MODE2_ROWS_0__7 + MODE2_ROWS_8_15 + MODE2_ROWS16_23:
			pgPageMask = 3;
			break;
		default:
			break;
	}
			

	TMS99X8.flags = TMS99X8_M2 | TMS99X8_GINT | TMS99X8_MEM416K;

	TMS99X8.ct6  = (MODE2_ADDRESS_CT  >>  6) | ( (pgPageMask<<5) + 0x1F); // 0b01111111 
	TMS99X8.pg11 = (((int16_t)MODE2_ADDRESS_PG)  >> 11) | pgPageMask; //0b00000011;
	
	//printf("PageFlags: %d\n", TMS99X8.pg11); fflush(stdout);
	
	TMS99X8.pn10 =  MODE2_ADDRESS_PN0 >> 10;
	TMS99X8.sa7  =  MODE2_ADDRESS_SA0 >>  7;
	TMS99X8.sg11 =  MODE2_ADDRESS_SG  >> 11;
	
	TMS99X8.backdrop  = BBlack;
	TMS99X8.textcolor = BWhite;
	
	TMS99X8_syncAllRegisters();
			
	TMS99X8_clear();
}

void TMS99X8_activateBuffer(EM2_Buffer buffer) {
	switch (buffer) {
	default:
	case MODE2_BUFFER_0:
		TMS99X8.pn10 = MODE2_ADDRESS_PN0 >> 10; 
		TMS99X8.sa7  = MODE2_ADDRESS_SA0 >>  7; 
		break;
	case MODE2_BUFFER_1:
		TMS99X8.pn10 = MODE2_ADDRESS_PN1 >> 10; 
		TMS99X8.sa7  = MODE2_ADDRESS_SA1 >>  7; 
		break;
	}
	TMS99X8_syncRegister(2);
	TMS99X8_syncRegister(5);
}


////////////////////////////////////////////////////////////////////////
// High Level Interface


T_SA SA0, SA1;

void TMS99X8_setFlags(TMS99X8_TFlags flags) {
	
	
	TMS99X8.flags = flags; 
	TMS99X8_syncRegister(0);
	TMS99X8_syncRegister(1);
}

#ifdef MSX
void TMS99X8_memcpy(uint16_t dst, const uint8_t *src, uint16_t size) {

	register uint16_t sz = size;
	register const uint8_t *p = src;
	
	VDP1 = dst & 0xFF; 
	NOP();
	VDP1 = 0x40 | (dst>>8);
	NOP();	
	
	while (sz--) VDP0 = *p++;
}

void TMS99X8_memset(uint16_t dst, uint8_t value, uint16_t size) {

	register uint16_t sz = size;
	register const uint8_t val = value;
	
	VDP1 = dst & 0xFF; 
	NOP();
	VDP1 = 0x40 | (dst>>8);
	NOP();	
	
	while (sz--) VDP0 = val;
}
#else
#include <string.h>

TMS99X8_Register TMS99X8;
uint8_t TMS99X8VRAM[0x4000];

void TMS99X8_memcpy(uint16_t dst, const uint8_t *src, uint16_t size) {
	memcpy(&TMS99X8VRAM[dst], src, size);
}

void TMS99X8_memset(uint16_t dst, uint8_t value, uint16_t size) {
	memset(&TMS99X8VRAM[dst], value, size);
}
#endif


void TMS99X8_writeSprite8 (uint8_t pos, const U8x8   s) {
	
	TMS99X8_memcpy(MODE2_ADDRESS_SG+(((uint16_t)pos)<<3), (const uint8_t *)s, 8);
}

void TMS99X8_writeSprite16(uint8_t pos, const U16x16 s) {

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

	TMS99X8_memcpy(MODE2_ADDRESS_SG+(((uint16_t)pos)<<3), (const uint8_t *)s, 32);
}

void TMS99X8_writeSpriteAttributes(EM2_Buffer buffer, const T_SA sa) {
	
	TMS99X8_memcpy(buffer?MODE2_ADDRESS_SA1:MODE2_ADDRESS_SA0, (const uint8_t *)sa, sizeof(T_SA));
}

////////////////////////////////////////////////////////////////////////
// Low Level Interface

void TMS99X8_setRegister(uint8_t reg, uint8_t val) {
	
	TMS99X8.reg[reg] = val; 
	TMS99X8_syncRegister(reg);
}

