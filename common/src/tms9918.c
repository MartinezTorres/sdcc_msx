#include <tms9918.h>

////////////////////////////////////////////////////////////////////////
// Mode2 Interface

void TMS9918_clear() {
	
	TMS9918_setFlags(TMS9918_M2 | TMS9918_GINT | TMS9918_MEM416K);
	TMS9918_memset(0,0,16*1024);
	TMS9918_setFlags(TMS9918_M2 | TMS9918_ENABLE | TMS9918_GINT | TMS9918_MEM416K);
}

void TMS9918_activateMode2 (EM2_PGpage_Flags pgPageFlags) {

	TMS9918Status.flags = TMS9918_M2 | TMS9918_GINT | TMS9918_MEM416K;

	TMS9918Status.ct6  = (MODE2_ADDRESS_CT  >>  6) | ( (pgPageFlags<<5) + 0x1F); // 0b01111111 
	TMS9918Status.pg11 = (((int16_t)MODE2_ADDRESS_PG)  >> 11) | pgPageFlags; //0b00000011;
	
	TMS9918Status.pn10 =  MODE2_ADDRESS_PN0 >> 10;
	TMS9918Status.sa7  =  MODE2_ADDRESS_SA0 >>  7;
	TMS9918Status.sg11 =  MODE2_ADDRESS_SG  >> 11;
	
	TMS9918Status.backdrop  = BBlack;
	TMS9918Status.textcolor = BWhite;
	
	{
		uint8_t i=0;
		for (i=0; i<8; ++i)
			TMS9918_syncRegister(i);
	}
		
	TMS9918_memset(MODE2_ADDRESS_PN0, 0, sizeof(T_PN));
	
	void TMS9918_clear();
}

void TMS9918_activateBuffer(EM2_Buffer buffer) {
	switch (buffer) {
	default:
	case MODE2_BUFFER_0:
		TMS9918Status.pn10 = MODE2_ADDRESS_PN0 >> 10; 
		TMS9918Status.sa7  = MODE2_ADDRESS_SA0 >>  7; 
		break;
	case MODE2_BUFFER_1:
		TMS9918Status.pn10 = MODE2_ADDRESS_PN1 >> 10; 
		TMS9918Status.sa7  = MODE2_ADDRESS_SA1 >>  7; 
		break;
	}
	TMS9918_syncRegister(2);
	TMS9918_syncRegister(5);
}


////////////////////////////////////////////////////////////////////////
// High Level Interface

void TMS9918_setFlags(TMS9918_TFlags flags) {
	
	
	TMS9918Status.flags = flags; 
	TMS9918_syncRegister(0);
	TMS9918_syncRegister(1);
}

#ifdef MSX
void TMS9918_memcpy(uint16_t dst, const uint8_t *src, uint16_t size) {

	register uint16_t sz = size;
	register const uint8_t *p = src;
	
	VDP1 = dst & 0xFF; 
	NOP();
	VDP1 = 0x40 | (dst>>8);
	NOP();	
	
	while (sz--) VDP0 = *p++;
}

void TMS9918_memset(uint16_t dst, uint8_t value, uint16_t size) {

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

TMS9918_Register TMS9918Status;
uint8_t TMS9918VRAM[0x4000];

void TMS9918_memcpy(uint16_t dst, const uint8_t *src, uint16_t size) {
	memcpy(&TMS9918VRAM[dst], src, size);
}

void TMS9918_memset(uint16_t dst, uint8_t value, uint16_t size) {
	memset(&TMS9918VRAM[dst], value, size);
}
#endif


void TMS9918_writeSprite8 (uint8_t pos, U8x8   s) {
	
	TMS9918_memcpy(MODE2_ADDRESS_SG+(((uint16_t)pos)<<3), (const uint8_t *)s, 8);
}

void TMS9918_writeSprite16(uint8_t pos, U16x16 s) {

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

	TMS9918_memcpy(MODE2_ADDRESS_SG+(((uint16_t)pos)<<3), (const uint8_t *)s, 32);
}


////////////////////////////////////////////////////////////////////////
// Low Level Interface

void TMS9918_setRegister(uint8_t reg, uint8_t val) {
	
	TMS9918Status.reg[reg] = val; 
	TMS9918_syncRegister(reg);
}

