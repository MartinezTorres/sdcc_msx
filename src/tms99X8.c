#include <tms99X8.h>

////////////////////////////////////////////////////////////////////////
// Memory Copy Functions

#ifdef MSX

void TMS99X8_memcpy_slow(uint16_t dst, const uint8_t *src, uint16_t size) {

	register uint16_t sz = size;
	register const uint8_t *p = src;
	VDP1 = dst & 0xFF; 
	VDP1 = 0x40 | (dst>>8);
    
	while (sz--) VDP0 = *p++;
}

void TMS99X8_memset(uint16_t dst, uint8_t value, uint16_t size) {

	register uint16_t sz = size;
	register const uint8_t val = value;
	
	VDP1 = dst & 0xFF; 
	VDP1 = 0x40 | (dst>>8);
	
	while (sz--) VDP0 = val;
}

void TMS99X8_memcpy8(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm
    ld c, #_VDP0
    ld b, #8
TMS99X8_memcpy8_l:
    outi
    jp nz, TMS99X8_memcpy8_l
    __endasm;
}
void TMS99X8_memcpy16(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm
    ld c, #_VDP0
    ld b, #16
TMS99X8_memcpy16_l:
    outi
    jp nz, TMS99X8_memcpy16_l
    __endasm;
}    
void TMS99X8_memcpy32(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm
    ld c, #_VDP0
    ld b, #32
TMS99X8_memcpy32_l:
    outi
    jp nz, TMS99X8_memcpy32_l
    __endasm;
}
void TMS99X8_memcpy64(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm
    ld c, #_VDP0
    ld b, #64
TMS99X8_memcpy64_l:
    outi
    jp nz, TMS99X8_memcpy64_l
    __endasm;
}
void TMS99X8_memcpy128(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm
    ld c, #_VDP0
    ld b, #128
TMS99X8_memcpy128_l:
    outi
    jp nz, TMS99X8_memcpy128_l
    __endasm;
}

void TMS99X8_fastcpy8(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm__("ld c, #_VDP0");
    REPEAT8(__asm__("outi"));
}
void TMS99X8_fastcpy16(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm__("ld c, #_VDP0");
    REPEAT16(__asm__("outi"));
}
void TMS99X8_fastcpy32(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm__("ld c, #_VDP0");
    REPEAT32(__asm__("outi"));
}
void TMS99X8_fastcpy64(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm__("ld c, #_VDP0");
    REPEAT64(__asm__("outi"));
}
void TMS99X8_fastcpy128(const uint8_t *src) __z88dk_fastcall {
    UNUSED(src);
    __asm__("ld c, #_VDP0");
    REPEAT64(__asm__("outi"));
    REPEAT64(__asm__("outi"));
}

#else
#include <string.h>

TMS99X8_Register TMS99X8;
uint8_t TMS99X8VRAM[0x4000];
uint16_t TMS99X8VRAM_PTR;

void TMS99X8_memcpy_slow(uint16_t dst, const uint8_t *src, uint16_t size) {
	memcpy(&TMS99X8VRAM[dst], src, size);
    TMS99X8VRAM_PTR = dst + size;
}

void TMS99X8_memset(uint16_t dst, uint8_t value, uint16_t size) {
	memset(&TMS99X8VRAM[dst], value, size);
    TMS99X8VRAM_PTR = dst + size;
}

void TMS99X8_memcpy8(const uint8_t *src) __z88dk_fastcall   { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,   8); TMS99X8VRAM_PTR+=  8;}
void TMS99X8_memcpy16(const uint8_t *src) __z88dk_fastcall  { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,  16); TMS99X8VRAM_PTR+= 16;}
void TMS99X8_memcpy32(const uint8_t *src) __z88dk_fastcall  { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,  32); TMS99X8VRAM_PTR+= 32;}
void TMS99X8_memcpy64(const uint8_t *src) __z88dk_fastcall  { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,  64); TMS99X8VRAM_PTR+= 64;}
void TMS99X8_memcpy128(const uint8_t *src) __z88dk_fastcall { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src, 128); TMS99X8VRAM_PTR+=128;}

void TMS99X8_fastcpy8(const uint8_t *src) __z88dk_fastcall   { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,   8); TMS99X8VRAM_PTR+=  8;}
void TMS99X8_fastcpy16(const uint8_t *src) __z88dk_fastcall  { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,  16); TMS99X8VRAM_PTR+= 16;}
void TMS99X8_fastcpy32(const uint8_t *src) __z88dk_fastcall  { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,  32); TMS99X8VRAM_PTR+= 32;}
void TMS99X8_fastcpy64(const uint8_t *src) __z88dk_fastcall  { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src,  64); TMS99X8VRAM_PTR+= 64;}
void TMS99X8_fastcpy128(const uint8_t *src) __z88dk_fastcall { memcpy(&TMS99X8VRAM[TMS99X8VRAM_PTR], src, 128); TMS99X8VRAM_PTR+=128;}
#endif


////////////////////////////////////////////////////////////////////////
// Mode2 Interface

void TMS99X8_clear() {
	
	TMS99X8_memset(0,0,16*1024);

	if (msxhal_get_msx_version() != MSX_VERSION_MSX1) {
		TMS99X8_setPtrExt(0);
	}
	
	TMS99X8_swapBuffersP = TMS99X8_activateBuffer0;
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
			

	TMS99X8.flags = TMS99X8_ENABLE | TMS99X8_GINT | TMS99X8_MEM416K;

	if (msxhal_get_msx_version() == MSX_VERSION_MSX1) {

		TMS99X8.M3 = 1;
		TMS99X8.sa7  =  MODE2_ADDRESS_SA0 >>  7;
	} else {

		TMS99X8.M4 = 1;
		TMS99X8.sa7  =  (MODE4_ADDRESS_SA0 >>  7) | 0x7;
	}

	TMS99X8.ct6  = (MODE2_ADDRESS_CT  >>  6) | ( (pgPageMask<<5) + 0x1F); // 0b01111111 
	TMS99X8.pg11 = (((int16_t)MODE2_ADDRESS_PG)  >> 11) | pgPageMask; //0b00000011;
	
	//printf("PageFlags: %d\n", TMS99X8.pg11); fflush(stdout);
	
	TMS99X8.pn10 =  MODE2_ADDRESS_PN0 >> 10;
	TMS99X8.sg11 =  MODE2_ADDRESS_SG  >> 11;
	
	TMS99X8.backdrop  = BBlack;
	TMS99X8.textcolor = BWhite;
	
	TMS99X8_syncAllRegisters();
	
	TMS99X8_clear();
}

void (*TMS99X8_swapBuffersP)();

void TMS99X8_activateBuffer0() {
	
	if (msxhal_get_msx_version() == MSX_VERSION_MSX1) {
		TMS99X8.sa7  = MODE2_ADDRESS_SA0 >>  7; 
	} else {
		TMS99X8.sa7  = (MODE4_ADDRESS_SA0 >>  7) | 0x7;
	}
	
	TMS99X8.pn10 = MODE2_ADDRESS_PN0 >> 10; 
	TMS99X8_syncRegister(2);
	TMS99X8_syncRegister(5);
	TMS99X8_swapBuffersP = TMS99X8_activateBuffer1;
}

void TMS99X8_activateBuffer1() {

	if (msxhal_get_msx_version() == MSX_VERSION_MSX1) {
		TMS99X8.sa7  = MODE2_ADDRESS_SA1 >>  7; 
	} else {
		TMS99X8.sa7  = (MODE4_ADDRESS_SA1 >>  7) | 0x7;
	}

	TMS99X8.pn10 = MODE2_ADDRESS_PN1 >> 10; 
	TMS99X8_syncRegister(2);
	TMS99X8_syncRegister(5);
	TMS99X8_swapBuffersP = TMS99X8_activateBuffer0;
}

void TMS99X8_writeSpriteAttributes(EM2_Buffer buffer, const T_SA sa) { 
	
//	VDP1 = 8; VDP1 = 0x87;
	
	if (msxhal_get_msx_version() == MSX_VERSION_MSX1) {
		if (buffer) 
			TMS99X8_memcpy(MODE2_ADDRESS_SA1, (const uint8_t *)sa, sizeof(T_SA)); 
		else 
			TMS99X8_memcpy(MODE2_ADDRESS_SA0, (const uint8_t *)sa, sizeof(T_SA)); 
	} else {
		if (buffer) {
			TMS99X8_setPtrExt(MODE4_ADDRESS_SA1-0x200);
		} else {
			TMS99X8_setPtrExt(MODE4_ADDRESS_SA0-0x200);
		}
		const uint8_t *p = (const uint8_t *)sa;
		for (uint8_t i=0; i<32; i++) {
			p++; p++; p++;
			uint8_t c = *p++;
			REPEAT16({
				TMS99X8_write(c);
				NOP();
			});
		}
		TMS99X8_memcpy128((const uint8_t *)sa);
		TMS99X8_setPtrExt(0);
	}

//	VDP1 = 0; VDP1 = 0x87;
}




////////////////////////////////////////////////////////////////////////
// High Level Interface

void TMS99X8_setFlags(TMS99X8_TFlags flags) {
	
	
	TMS99X8.flags = flags; 
	TMS99X8_syncRegister(0);
	TMS99X8_syncRegister(1);
}

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


////////////////////////////////////////////////////////////////////////
// Low Level Interface

void TMS99X8_setRegister(uint8_t reg, uint8_t val) {
	
	TMS99X8.reg[reg] = val; 
	TMS99X8_syncRegister(reg);
}

