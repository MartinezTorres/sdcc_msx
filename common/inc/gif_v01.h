#pragma once

#include <tms99X8.h>

extern volatile uint8_t gif_v01_frames_left;
extern volatile uint8_t gif_v01_bufferId;

void gif_v01_initVideo(void);
void gif_v01_deinitVideo(void);

INLINE void gif_v01_initSendFrame(void) {}

INLINE void gif_v01_post_sendPN(void) {}

INLINE void gif_v01_endSendFrame(void) {}

void gif_v01_pre_sendPN(void);
void gif_v01_mid_sendPN(void);

#ifdef MSX

#define gif_v01_copy_vpn_x8(address, data, sz) \
do { \
	DI(); \
	__asm__("_MODE2_ADDRESS_CT = 0x2000"); \
	__asm__("_MODE2_ADDRESS_PG = 0x0000"); \
	__asm__("in a,(#0x99)"); \
	__asm__("ld a,#((_" #address ") & 0xFF)"); \
	__asm__("out (#0x99),a"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld d,#(" #sz ")"); \
	__asm__("nop"); \
	__asm__("nop"); \
	__asm__("ld a,#( 0x40 + ((_" #address ")/256))"); \
	__asm__("out (#0x99),a"); \
	__asm__("call _gif_v01_copy_vpn_x8_asm"); \
	EI(); \
} while(false)



INLINE void gif_v01_addPN(uint16_t target, const uint8_t *data, uint16_t sz) {

	uint8_t pn[256];
	DI();
	{
		uint16_t i = 0, j=(gif_v01_bufferId?0:128);
		for (i=0; i<sz; i++)
			pn[i] = data[i] + j;

		TMS99X8_memcpy( (gif_v01_bufferId?MODE2_ADDRESS_PN0:MODE2_ADDRESS_PN1)+target, pn, sz);
	}
	EI();
}

//INLINE void gif_v01_addPN_section0(const uint8_t *data, uint8_t start, uint8_t sz) { gif_v01_addPN((256*  0)+start,data,sz); }
//INLINE void gif_v01_addPN_section0_256(const uint8_t *data)                        { gif_v01_addPN((256*  0),data,256); }
//INLINE void gif_v01_addPN_section1(const uint8_t *data, uint8_t start, uint8_t sz) { gif_v01_addPN((256*  1)+start,data,sz); }
//INLINE void gif_v01_addPN_section1_256(const uint8_t *data)                        { gif_v01_addPN((256*  1),data,256); }
//INLINE void gif_v01_addPN_section2(const uint8_t *data, uint8_t start, uint8_t sz) { gif_v01_addPN((256*  2)+start,data,sz); }
//INLINE void gif_v01_addPN_section2_256(const uint8_t *data)                        { gif_v01_addPN((256*  2),data,256); }

#define gif_v01_addPN_section0(data, start, sz) \
do { \
	__asm__("ld d,#(0x00)"); \
	__asm__("ld e,#(" #start ")"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld b,#(" #sz ")"); \
	__asm__("call _gif_v01_copy_pn_asm"); \
} while(false)

#define gif_v01_addPN_section0_256(data) \
do { \
	__asm__("ld d,#(0x00)"); \
	__asm__("ld e,#(0x00)"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld b,#(0x00)"); \
	__asm__("call _gif_v01_copy_pn_asm"); \
} while(false)

#define gif_v01_addPN_section1(data, start, sz) \
do { \
	__asm__("ld d,#(0x01)"); \
	__asm__("ld e,#(" #start ")"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld b,#(" #sz ")"); \
	__asm__("call _gif_v01_copy_pn_asm"); \
} while(false)

#define gif_v01_addPN_section1_256(data) \
do { \
	__asm__("ld d,#(0x01)"); \
	__asm__("ld e,#(0x00)"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld b,#(0x00)"); \
	__asm__("call _gif_v01_copy_pn_asm"); \
} while(false)


#define gif_v01_addPN_section2(data, start, sz) \
do { \
	__asm__("ld d,#(0x02)"); \
	__asm__("ld e,#(" #start ")"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld b,#(" #sz ")"); \
	__asm__("call _gif_v01_copy_pn_asm"); \
} while(false)

#define gif_v01_addPN_section2_256(data) \
do { \
	__asm__("ld d,#(0x02)"); \
	__asm__("ld e,#(0x00)"); \
	__asm__("ld hl,#(_" #data ")"); \
	__asm__("ld b,#(0x00)"); \
	__asm__("call _gif_v01_copy_pn_asm"); \
} while(false)

#else



INLINE void gif_v01_copy_vpn_x8(uint16_t address, const uint8_t *data, uint8_t sz) { DI(); TMS99X8_memcpy(address,data,sz<<3); EI(); }



INLINE void gif_v01_addPN(uint16_t target, const uint8_t *data, uint16_t sz) {

	uint8_t pn[256];
	DI();
	{
		uint16_t i = 0;
		uint8_t j=(gif_v01_bufferId?0:128);
		for (i=0; i<sz; i++)
			pn[i] = data[i] + j;

		TMS99X8_memcpy( (gif_v01_bufferId?MODE2_ADDRESS_PN0:MODE2_ADDRESS_PN1)+target, pn, sz);
	}
	EI();
}

INLINE void gif_v01_addPN_section0(const uint8_t *data, uint8_t start, uint8_t sz) { gif_v01_addPN((256*  0)+start,data,sz); }
INLINE void gif_v01_addPN_section0_256(const uint8_t *data)                        { gif_v01_addPN((256*  0),data,256); }
INLINE void gif_v01_addPN_section1(const uint8_t *data, uint8_t start, uint8_t sz) { gif_v01_addPN((256*  1)+start,data,sz); }
INLINE void gif_v01_addPN_section1_256(const uint8_t *data)                        { gif_v01_addPN((256*  1),data,256); }
INLINE void gif_v01_addPN_section2(const uint8_t *data, uint8_t start, uint8_t sz) { gif_v01_addPN((256*  2)+start,data,sz); }
INLINE void gif_v01_addPN_section2_256(const uint8_t *data)                        { gif_v01_addPN((256*  2),data,256); }


#endif




