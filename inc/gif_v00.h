#pragma once

#include <tms99X8.h>

void gif_v00_initVideo();
void gif_v00_deinitVideo();

void gif_v00_sendPatternNames(const uint8_t *address);


#ifdef MSX

// Invariant During Pattern Transfer:
// HL: source address
// c contains 0x98 (VDP port);
// de contains 8
#define gif_v00_sendPatternsInit() { DI(); __asm__("ld c,#0x98"); __asm__("ld de,#0x08"); }
#define gif_v00_sendPatternsEnd() { EI(); }
#define gif_v00_load_page_d(segment) { __asm__("push hl"); __asm__("call _K5_PAGE_D_" #segment); __asm__("ld	a,l"); __asm__("ld	(#0xB000),a"); __asm__("pop hl"); }

#define gif_v00_setSourceAddress(symbol, offset) { __asm__("ld hl,#(_" #symbol "+" #offset ")"); }
#define gif_v00_setTargetAddress(address) { EI(); DI(); __asm__("ld a,#(" #address "& 0xFF)");   __asm__("out (#0x99),a"); NOP(); __asm__("ld a,#( 0x40 + (" #address "/256))"); __asm__("out (#0x99),a"); NOP();}
#define gif_v00_copy1() { __asm__("call #_gif_v00_copy1_asm"); }
#define gif_v00_copy2() { __asm__("call #_gif_v00_copy2_asm"); }
#define gif_v00_copy3() { __asm__("call #_gif_v00_copy3_asm"); }
#define gif_v00_copy4() { __asm__("call #_gif_v00_copy4_asm"); }
#define gif_v00_copy5() { __asm__("call #_gif_v00_copy5_asm"); }
#define gif_v00_copy6() { __asm__("call #_gif_v00_copy6_asm"); }
#define gif_v00_copy7() { __asm__("call #_gif_v00_copy7_asm"); }
#define gif_v00_copy8() { __asm__("call #_gif_v00_copy8_asm"); }
#define gif_v00_copy9() { __asm__("call #_gif_v00_copy9_asm"); }
#define gif_v00_copy10() { __asm__("call #_gif_v00_copy10_asm"); }
#define gif_v00_copy11() { __asm__("call #_gif_v00_copy11_asm"); }
#define gif_v00_copy12() { __asm__("call #_gif_v00_copy12_asm"); }
#define gif_v00_copy13() { __asm__("call #_gif_v00_copy13_asm"); }
#define gif_v00_copy14() { __asm__("call #_gif_v00_copy14_asm"); }
#define gif_v00_copy15() { __asm__("call #_gif_v00_copy15_asm"); }
#define gif_v00_copy16() { __asm__("call #_gif_v00_copy16_asm"); }

#define gif_v00_copy_inc2() { __asm__("call #_gif_v00_copy_inc2_asm"); }
#define gif_v00_copy_inc3() { __asm__("call #_gif_v00_copy_inc3_asm"); }
#define gif_v00_copy_inc4() { __asm__("call #_gif_v00_copy_inc4_asm"); }
#define gif_v00_copy_inc5() { __asm__("call #_gif_v00_copy_inc5_asm"); }
#define gif_v00_copy_inc6() { __asm__("call #_gif_v00_copy_inc6_asm"); }
#define gif_v00_copy_inc7() { __asm__("call #_gif_v00_copy_inc7_asm"); }
#define gif_v00_copy_inc8() { __asm__("call #_gif_v00_copy_inc8_asm"); }
#define gif_v00_copy_inc9() { __asm__("call #_gif_v00_copy_inc9_asm"); }
#define gif_v00_copy_inc10() { __asm__("call #_gif_v00_copy_inc10_asm"); }
#define gif_v00_copy_inc11() { __asm__("call #_gif_v00_copy_inc11_asm"); }
#define gif_v00_copy_inc12() { __asm__("call #_gif_v00_copy_inc12_asm"); }
#define gif_v00_copy_inc13() { __asm__("call #_gif_v00_copy_inc13_asm"); }
#define gif_v00_copy_inc14() { __asm__("call #_gif_v00_copy_inc14_asm"); }
#define gif_v00_copy_inc15() { __asm__("call #_gif_v00_copy_inc15_asm"); }
#define gif_v00_copy_inc16() { __asm__("call #_gif_v00_copy_inc16_asm"); }

#else

extern const uint8_t *gif_v00_source;
extern uint16_t gif_v00_target;

#define gif_v00_sendPatternsInit() do {} while(false)
#define gif_v00_load_page_d(segment) do {} while(false)
#define gif_v00_sendPatternsEnd() do {} while(false)

static inline void gif_v00_setTargetAddress(uint16_t address) { gif_v00_target = address; }
static inline void gif_v00_setSourceAddress(const uint8_t *address, uint16_t offset) { gif_v00_source = address + offset; }

static inline void gif_v00_copy1() { TMS99X8_memcpy(gif_v00_target,gif_v00_source,8); gif_v00_source += 8; gif_v00_target+=8;}
static inline void gif_v00_copy2() { gif_v00_copy1(); gif_v00_copy1(); }
static inline void gif_v00_copy3() { gif_v00_copy2(); gif_v00_copy1(); }
static inline void gif_v00_copy4() { gif_v00_copy3(); gif_v00_copy1(); }
static inline void gif_v00_copy5() { gif_v00_copy4(); gif_v00_copy1(); }
static inline void gif_v00_copy6() { gif_v00_copy5(); gif_v00_copy1(); }
static inline void gif_v00_copy7() { gif_v00_copy6(); gif_v00_copy1(); }
static inline void gif_v00_copy8() { gif_v00_copy7(); gif_v00_copy1(); }
static inline void gif_v00_copy9() { gif_v00_copy8(); gif_v00_copy1(); }
static inline void gif_v00_copy10() { gif_v00_copy9(); gif_v00_copy1(); }
static inline void gif_v00_copy11() { gif_v00_copy10(); gif_v00_copy1(); }
static inline void gif_v00_copy12() { gif_v00_copy11(); gif_v00_copy1(); }
static inline void gif_v00_copy13() { gif_v00_copy12(); gif_v00_copy1(); }
static inline void gif_v00_copy14() { gif_v00_copy13(); gif_v00_copy1(); }
static inline void gif_v00_copy15() { gif_v00_copy14(); gif_v00_copy1(); }
static inline void gif_v00_copy16() { gif_v00_copy15(); gif_v00_copy1(); }


static inline void gif_v00_copy_inc2() { gif_v00_copy1(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc3() { gif_v00_copy_inc2(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc4() { gif_v00_copy_inc3(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc5() { gif_v00_copy_inc4(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc6() { gif_v00_copy_inc5(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc7() { gif_v00_copy_inc6(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc8() { gif_v00_copy_inc7(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc9() { gif_v00_copy_inc8(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc10() { gif_v00_copy_inc9(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc11() { gif_v00_copy_inc10(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc12() { gif_v00_copy_inc11(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc13() { gif_v00_copy_inc12(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc14() { gif_v00_copy_inc13(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc15() { gif_v00_copy_inc14(); gif_v00_source += 8; gif_v00_copy1(); }
static inline void gif_v00_copy_inc16() { gif_v00_copy_inc15(); gif_v00_source += 8; gif_v00_copy1(); }
#endif
