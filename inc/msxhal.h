#pragma once

// Removes warning for unused expressions
#define UNUSED(expr) do { (void)(expr); } while (0)

#define APPEND_INNER(a,b) a##b
#define APPEND(a,b) APPEND_INNER(a,b)

////////////////////////////////////////////////////////////////////////
// INLINE and FASTCALL
#ifdef MSX

	#define INLINE inline

#elif LINUX

	#define INLINE static inline
    #define __z88dk_fastcall

#else
	#error "Architecture Not Supported"
#endif


////////////////////////////////////////////////////////////////////////
// TYPES

#include <stdint.h>

typedef enum {
	false = 0,
	true = 1 
} bool;

#define nullptr ((void *)0)

typedef struct {
	int16_t x,y,dx,dy;
} TRect16;

typedef struct {
    union { uint8_t x,j; };
    union { uint8_t y,i; };
} uint8_tp;

typedef struct {
    union { int16_t x,j; };
    union { int16_t y,i; };
} int16_tp;

typedef struct {
    union { uint16_t x,j; };
    union { uint16_t y,i; };
} uint16_tp;

typedef uint16_t U16x16[16];
typedef uint8_t  U8x8  [8];

////////////////////////////////////////////////////////////////////////
// COMMON HELPER FUNCTIONS

#define cropped(a,b,c) (a<(b)?(b):(a>(c)?(c):a))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

//#define swap(a,b,c) do { a k_=b; b=c; c=k_; } while(false)

#define REPEAT0(a)  do { } while (false)
#define REPEAT1(a)  do { a; } while (false)
#define REPEAT2(a)  do { REPEAT1(a);  REPEAT1(a);  } while (false)
#define REPEAT3(a)  do { REPEAT1(a);  REPEAT2(a);  } while (false)
#define REPEAT4(a)  do { REPEAT2(a);  REPEAT2(a);  } while (false)
#define REPEAT8(a)  do { REPEAT4(a);  REPEAT4(a);  } while (false)
#define REPEAT16(a) do { REPEAT8(a);  REPEAT8(a);  } while (false)
#define REPEAT32(a) do { REPEAT16(a); REPEAT16(a); } while (false)
#define REPEAT64(a) do { REPEAT32(a); REPEAT32(a); } while (false)
#define REPEAT(a,N) do { \
	if (N&(1<<0)) REPEAT1(a);  \
	if (N&(1<<1)) REPEAT2(a);  \
	if (N&(1<<2)) REPEAT4(a);  \
	if (N&(1<<3)) REPEAT8(a);  \
	if (N&(1<<4)) REPEAT16(a); \
	if (N&(1<<5)) REPEAT32(a); \
	if (N&(1<<6)) REPEAT64(a); \
} while (false)

#define UNROLL1(nn,t,a)  do { if (nn<t) { uint8_t n = nn; a; UNUSED(n); } } while (false)
#define UNROLL2(nn,t,a)  do { if (nn<t) { UNROLL1(nn,t,a);  UNROLL1(nn+1,t,a);   } } while (false)
#define UNROLL4(nn,t,a)  do { if (nn<t) { UNROLL2(nn,t,a);  UNROLL2(nn+2,t,a);   } } while (false)
#define UNROLL8(nn,t,a)  do { if (nn<t) { UNROLL4(nn,t,a);  UNROLL4(nn+4,t,a);   } } while (false)
#define UNROLL16(nn,t,a) do { if (nn<t) { UNROLL8(nn,t,a);  UNROLL8(nn+8,t,a);   } } while (false)
#define UNROLL32(nn,t,a) do { if (nn<t) { UNROLL16(nn,t,a); UNROLL16(nn+16,t,a); } } while (false)
#define UNROLL64(nn,t,a) do { if (nn<t) { UNROLL32(nn,t,a); UNROLL32(nn+32,t,a); } } while (false)

#define UNROLL(t,a)      do { UNROLL64(0,t,a); UNROLL64(64,t,a); } while (false)

////////////////////////////////////////////////////////////////////////
// COMMON ASM INTEGRATIONS

#ifdef MSX

	#define NOP(a)  do { __asm nop  __endasm;  } while (false)
	#define DI(a)   do { __asm di   __endasm;  } while (false) // Dangerous, please use wait_frame and yield instead.
	#define EI(a)   do { __asm ei   __endasm;  } while (false) // Dangerous, please use wait_frame and yield instead.
	#define HALT(a) do { __asm halt __endasm;  } while (false) // Dangerous, please use wait_frame and yield instead.

	INLINE void wait_frame(void) { 
		__asm 
			ei
			nop
			halt
			di 
		__endasm;
	}
	INLINE void yield(void) {
		__asm 
			ei
			nop
			di 
		__endasm;
	}
		
//	inline int printf(const char *f, ...) { (f); return 0;}
	#define printf(...)
	#define fflush(a) do {} while(false)
	
	#define memset(d,c,n) __builtin_memset(d,c,n);
	#define memcpy(d,s,n) __builtin_memcpy(d,s,n);
	#define strcpy(d,s) __builtin_strcpy(d,s);

#elif LINUX

	void yield();
	void wait_frame();

	#define NOP(a)  do { } while (false)
	#define DI(a)
	#define EI(a)
	#define HALT(a) wait_frame()

	#include <stdio.h>
	#include <string.h>

#else
	#error "Architecture Not Supported"
#endif

////////////////////////////////////////////////////////////////////////
// MAPPER FUNCTIONS
// THERE ARE 4 PAGES, A, B, C, and D. Each page can hold any segment (from 0 to 255) of the mapped ROM
// 
// MAPPER can be selected between ASCII8_MAPPER and KONAMI5_MAPPER and NULL_MAPPER

typedef enum { PAGE_A=0, PAGE_B=1, PAGE_C=2, PAGE_D=3 } Page;


#ifdef MAPPER
    #error "Mapper should be defined internally"
#endif

#ifdef KONAMI5
    #define MAPPER MAPPER_8K
    #define MAPPER_FAST MAPPER_8K_FAST
    #define PAGE_A_ADDRESS 0x5000
    #define PAGE_B_ADDRESS 0x7000
    #define PAGE_C_ADDRESS 0x9000
    #define PAGE_D_ADDRESS 0xB000
#endif

#ifdef ASCII8
    #ifdef MAPPER
        #error "Duplicated mapper definition"
    #endif
    #define MAPPER MAPPER_8K
    #define MAPPER_FAST MAPPER_8K_FAST
    #define PAGE_A_ADDRESS 0x6000
    #define PAGE_B_ADDRESS 0x6800
    #define PAGE_C_ADDRESS 0x7000
    #define PAGE_D_ADDRESS 0x7800
#endif

#ifdef LINUX
    #undef MAPPER
#endif

#ifndef MAPPER
    #define MAPPER(segment, page) do {} while(false)
    #define MAPPER_FAST(segment, page) do {} while(false)
	#define MODULE_SEGMENT(module, page) 0
    #define CURRENT_SEGMENT(page) 0
    #define msxhal_call_b(segment, a) do { a(); } while(false)
    #define msxhal_call_c(segment, a) do { a(); } while(false)
    #define msxhal_call_d(segment, a) do { a(); } while(false)
    
#else
	#define MODULE_SEGMENT_APPEND(module, page) ((const uint8_t)&MAPPER_MODULE_ ## module ## _ ## page)
	#define MODULE_SEGMENT(module, page) MODULE_SEGMENT_APPEND( module, page)
    #define CURRENT_SEGMENT(page) mapper_current_segments[page] 
    typedef void (*call_function)();
	void msxhal_call_b(uint8_t segment, call_function f);
	void msxhal_call_c(uint8_t segment, call_function f);
	void msxhal_call_d(uint8_t segment, call_function f);

    extern uint8_t mapper_current_segments[4];

    INLINE void MAPPER_8K(uint8_t segment, Page page) { 
        if (page == PAGE_A) *(uint8_t *)PAGE_A_ADDRESS = mapper_current_segments[PAGE_A] = segment;
        if (page == PAGE_B) *(uint8_t *)PAGE_B_ADDRESS = mapper_current_segments[PAGE_B] = segment;
        if (page == PAGE_C) *(uint8_t *)PAGE_C_ADDRESS = mapper_current_segments[PAGE_C] = segment;
        if (page == PAGE_D) *(uint8_t *)PAGE_D_ADDRESS = mapper_current_segments[PAGE_D] = segment;
    }

    INLINE void MAPPER_8K_FAST(uint8_t segment, Page page) { 
        if (page == PAGE_A) *(uint8_t *)PAGE_A_ADDRESS = segment;
        if (page == PAGE_B) *(uint8_t *)PAGE_B_ADDRESS = segment;
        if (page == PAGE_C) *(uint8_t *)PAGE_C_ADDRESS = segment;
        if (page == PAGE_D) *(uint8_t *)PAGE_D_ADDRESS = segment;
    }
#endif
    
#define USING_MODULE(module, page) \
    extern const uint8_t MAPPER_MODULE_ ## module ## _PAGE_A, \
        MAPPER_MODULE_ ## module ## _PAGE_B, \
        MAPPER_MODULE_ ## module ## _PAGE_C, \
        MAPPER_MODULE_ ## module ## _PAGE_D
        
INLINE uint8_t mapper_load_segment(uint8_t segment, Page page) { uint8_t old = CURRENT_SEGMENT(page); UNUSED(segment); UNUSED(page); MAPPER(segment, page); return old; }
#define mapper_load_module(module, page) mapper_load_segment(MODULE_SEGMENT(module, page), page)

INLINE void mapper_load_segment_fast(uint8_t segment, Page page) { UNUSED(segment); UNUSED(page); MAPPER_FAST(segment, page); }
#define mapper_load_module_fast(module, page) mapper_load_segment_fast(MODULE_SEGMENT(module, page), page)

#define CALL_PAGE(module, page, a) { do { uint8_t o = mapper_load_module(module, page); a;  mapper_load_segment(o, page); } while(false); }

#define CALL_B(module, a) msxhal_call_b(MODULE_SEGMENT(module, PAGE_B), a)
#define CALL_C(module, b) msxhal_call_c(MODULE_SEGMENT(module, PAGE_C), b)
#define CALL_D(module, c) msxhal_call_d(MODULE_SEGMENT(module, PAGE_D), c)

#define IN_MODULE(module, page, a) { do { uint8_t o = mapper_load_module(module, page); a;  mapper_load_segment(o, page); } while(false); }
#define IN_SEGMENT(segment, page, a) { do { uint8_t o = mapper_load_segment(segment, page); a;  mapper_load_segment(o, page); } while(false); }

/*////////////////////////////////////////////////////////////////////////
// MAPPER CALLING FUNCTIONS IN NORMAL PROGRAMMING MODE
// 
// PAGE_A IS FIXED, PAGE_B to PAGE_D are DYNAMIC.
// calls FROM PAGE_A to PAGE_A are free.
// calls FROM PAGE_M to PAGE_A are free.
// calls FROM PAGE_A to PAGE_M must load the segment first.
// calls FROM PAGE_M to PAGE_M must use a trampoline in PAGE_A. mapper_call supports basic functionality.
    
uint16_t mapper_call_a(uint8_t segment, uint16_t (*f)(uint16_t), uint16_t value);
uint16_t mapper_call_b(uint8_t segment, uint16_t (*f)(uint16_t), uint16_t value);
uint16_t mapper_call_c(uint8_t segment, uint16_t (*f)(uint16_t), uint16_t value);
uint16_t mapper_call_d(uint8_t segment, uint16_t (*f)(uint16_t), uint16_t value);

INLINE uint16_t mapper_call(Page page, uint8_t segment, uint16_t (*f)(uint16_t), uint16_t value) {
    switch (page) {
        case PAGE_A: return mapper_call_a(segment, f, value);
        case PAGE_B: return mapper_call_b(segment, f, value);
        case PAGE_C: return mapper_call_c(segment, f, value);
        case PAGE_D: return mapper_call_d(segment, f, value);
    }
}

#ifndef MAPPER
    #define banked_call_segment(segment, page,  function, ...) function(__VA_ARGS__)
    #define banked_call_module(module, page, function, ...) function(__VA_ARGS__)
#else

    extern void *banked_call_trampoline_PAGE_A;
    extern void *banked_call_trampoline_PAGE_B;
    extern void *banked_call_trampoline_PAGE_C;
    extern void *banked_call_trampoline_PAGE_D;

    #define declare_banked_call(page, ret, name, calls)
        ret name ## _ ## page ## _trampoline calls

    #define banked_call_segment(segment, page, function, name, ...) \
    ( banked_call_prelude(segment, page, function), name ## _ ## page ## _trampoline(__VA_ARGS__) )

    #define banked_call_segment(segment, page, function, type, ...) \
    (*(type)&banked_call_trampoline_ ## page)(segment, (void *)function, __VA_ARGS__)
      
    #define banked_call_module(module, page, function, type, ...) banked_call_segment( MODULE_SEGMENT(module, page), page, function, type, __VA_ARGS__)
    
#endif
*/

////////////////////////////////////////////////////////////////////////
// IO FUNCTIONS
enum    { J_RIGHT=0x80,J_DOWN=0x40,J_UP=0x20,J_LEFT=0x10,J_DEL=0x08,J_INS=0x04,J_HOME=0x02,J_SPACE=0x01 };
extern volatile bool enable_keyboard_routine;
	
uint8_t msxhal_joystick_read(uint8_t joystickId);
uint8_t msxhal_getch(); //Non blocking red character from buffer.

////////////////////////////////////////////////////////////////////////
// MSX INFO FUNCTIONS

enum    { 
	MSX_CHARACTER_SET_JAPANESE=0x00,
	MSX_CHARACTER_SET_ASCII=0x01,
	MSX_CHARACTER_SET_KOREAN=0x02,

	MSX_DATE_FORMAT_YMD=0x00,
	MSX_DATE_FORMAT_MDY=0x10,
	MSX_DATE_FORMAT_DMY=0x20,

	MSX_FREQUENCY_60HZ=0x00,
	MSX_FREQUENCY_50HZ=0x80,

	MSX_KEYBOARD_TYPE_JAPANESE=0x00,
	MSX_KEYBOARD_TYPE_INTERNATIONAL=0x01,
	MSX_KEYBOARD_TYPE_FRENCH=0x02,
	MSX_KEYBOARD_TYPE_UK=0x03,
	MSX_KEYBOARD_TYPE_GERMAN=0x04,

	MSX_BASIC_VERSION_JAPANESE=0x00,
	MSX_BASIC_VERSION_INTERNATIONAL=0x10,
	
	MSX_VERSION_MSX1=0x00,
	MSX_VERSION_MSX2=0x01,
	MSX_VERSION_MSX2P=0x02,
	MSX_VERSION_MSXTR=0x03

};


#ifdef MSX

	#define BIOS_IDBYT1 (*(const uint8_t*)0x002B)
	#define BIOS_IDBYT2 (*(const uint8_t*)0x002C)
	#define BIOS_ROMID  (*(const uint8_t*)0x002D)

#elif LINUX

	#define BIOS_IDBYT1 (MSX_CHARACTER_SET_ASCII+MSX_DATE_FORMAT_DMY+MSX_FREQUENCY_60HZ)
	#define BIOS_IDBYT2 (MSX_KEYBOARD_TYPE_INTERNATIONAL+MSX_BASIC_VERSION_INTERNATIONAL)
	#define BIOS_ROMID  (MSX_VERSION_MSX1)

#endif


INLINE uint8_t msxhal_get_character_set() { return BIOS_IDBYT1 & 0x0F; }
INLINE uint8_t msxhal_get_date_format()   { return BIOS_IDBYT1 & 0x70; }
INLINE uint8_t msxhal_get_interrupt_frequency() { return BIOS_IDBYT1 & 0x80; }
INLINE uint8_t msxhal_get_keyboard_type() { return BIOS_IDBYT2 & 0x0F; }
INLINE uint8_t msxhal_get_basic_version() { return BIOS_IDBYT2 & 0xF0; }
INLINE uint8_t msxhal_get_msx_version()   { return BIOS_ROMID; }

////////////////////////////////////////////////////////////////////////
// TURBO FUNCTIONS

#ifdef MSX

INLINE void msxhal_request60Hz() {
    __asm
    ld   hl,#0xFFE8
    ld   a,(hl)
    and   #0xFD
    ld   (hl),a
    out  (_VDP1),a
    ld   a,#0x89
    out  (_VDP1),a
    __endasm; 
}

INLINE void msxhal_enableR800() {
    __asm
    ld   A,(#0x0180) ; CHGCPU
    cp   #0xC3
    ld   a,#0x81              ; can be ld a,81h for R800 ROM if you wish
    call z,#0x0180
    __endasm; 
}
#else
INLINE void msxhal_request60Hz() {}
INLINE void msxhal_enableR800() {}
#endif


////////////////////////////////////////////////////////////////////////
// ISR FUNCTIONS

typedef void (*isr_function)();
isr_function msxhal_install_isr(isr_function);
isr_function msxhal_install_segment_isr(uint8_t segment, isr_function);

////////////////////////////////////////////////////////////////////////
// INIT FUNCTIONS
void msxhal_init();

void exit(int);

