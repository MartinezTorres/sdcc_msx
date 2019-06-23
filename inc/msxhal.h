#pragma once

////////////////////////////////////////////////////////////////////////
// INLINE
#ifdef MSX

	#define INLINE inline

#elif LINUX

	#define INLINE static inline

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
	uint8_t x,y;
} uint8_tp;

typedef struct {
	int16_t x,y;
} int16_tp;

typedef uint16_t U16x16[16];
typedef uint8_t  U8x8  [8];

////////////////////////////////////////////////////////////////////////
// COMMON HELPER FUNCTIONS

#define cropped(a,b,c) (a<(b)?(b):(a>(c)?(c):a))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

//#define swap(a,b,c) do { a k_=b; b=c; c=k_; } while(false)

#define REPEAT0(a)  { }
#define REPEAT1(a)  { {a}; }
#define REPEAT2(a)  { REPEAT1(a);  REPEAT1(a);  }
#define REPEAT4(a)  { REPEAT2(a);  REPEAT2(a);  }
#define REPEAT8(a)  { REPEAT4(a);  REPEAT4(a);  }
#define REPEAT16(a) { REPEAT8(a);  REPEAT8(a);  }
#define REPEAT32(a) { REPEAT16(a); REPEAT16(a); }
#define REPEAT64(a) { REPEAT32(a); REPEAT32(a); }
#define REPEAT(a,N) { \
	if (N&(1<<0)) REPEAT1(a);  \
	if (N&(1<<1)) REPEAT2(a);  \
	if (N&(1<<2)) REPEAT4(a);  \
	if (N&(1<<3)) REPEAT8(a); \
	if (N&(1<<4)) REPEAT16(a); \
	if (N&(1<<5)) REPEAT32(a); \
	if (N&(1<<6)) REPEAT64(a); \
}

////////////////////////////////////////////////////////////////////////
// COMMON ASM INTEGRATIONS

#ifdef MSX

	#define NOP(a)  do { __asm nop  __endasm;  } while (false)
	//#define DI(a)   do { __asm di   __endasm;  } while (false)
	//#define EI(a)   do { __asm ei   __endasm;  } while (false)
	//#define HALT(a) do { __asm halt __endasm;  } while (false)

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

#elif LINUX

	void yield();
	void wait_frame();

	#define NOP(a)  do { } while (false)
	//#define HALT(a) do { wait_frame(); } while (false)

	#include <stdio.h>
	#include <string.h>

#else
	#error "Architecture Not Supported"
#endif

////////////////////////////////////////////////////////////////////////
// BLOCK FUNCTIONS


#ifdef MSX

	// NAMING IS CONFUSING.
	// THERE ARE 4 PAGES, A, B, C, and D. Each page can hold any segment (from 0 to 255) of the K5 mapped ROM

	#define USING_PAGE_A(module) extern const uint8_t K5_SEGMENT_TO_PAGE_A_ ## module
	#define USING_PAGE_B(module) extern const uint8_t K5_SEGMENT_TO_PAGE_B_ ## module
	#define USING_PAGE_C(module) extern const uint8_t K5_SEGMENT_TO_PAGE_C_ ## module
	#define USING_PAGE_D(module) extern const uint8_t K5_SEGMENT_TO_PAGE_D_ ## module
	#define SEGMENT_TO_PAGE_A(module) ((const uint8_t)&K5_SEGMENT_TO_PAGE_A_ ## module)
	#define SEGMENT_TO_PAGE_B(module) ((const uint8_t)&K5_SEGMENT_TO_PAGE_B_ ## module)
	#define SEGMENT_TO_PAGE_C(module) ((const uint8_t)&K5_SEGMENT_TO_PAGE_C_ ## module)
	#define SEGMENT_TO_PAGE_D(module) ((const uint8_t)&K5_SEGMENT_TO_PAGE_D_ ## module)

	extern volatile uint8_t current_segment_a;
	extern volatile uint8_t current_segment_b;
	extern volatile uint8_t current_segment_c;
	extern volatile uint8_t current_segment_d;
	
	inline uint8_t load_page_a(const uint8_t newSegment) { uint8_t oldSegment = current_segment_a;
		*(uint8_t *)0x5000 = current_segment_a = (uint8_t)newSegment; return oldSegment; }

	inline uint8_t load_page_b(const uint8_t newSegment) { uint8_t oldSegment = current_segment_b;
		*(uint8_t *)0x7000 = current_segment_b = (uint8_t)newSegment; return oldSegment; }

	inline uint8_t load_page_c(const uint8_t newSegment) { uint8_t oldSegment = current_segment_c;
		*(uint8_t *)0x9000 = current_segment_c = (uint8_t)newSegment; return oldSegment; }

	inline uint8_t load_page_d(const uint8_t newSegment) { uint8_t oldSegment = current_segment_d;
		*(uint8_t *)0xB000 = current_segment_d = (uint8_t)newSegment; return oldSegment; }

	inline void restore_page_a(uint8_t oldSegment) { *(uint8_t *)0x5000 = current_segment_a = oldSegment; }
	inline void restore_page_b(uint8_t oldSegment) { *(uint8_t *)0x7000 = current_segment_b = oldSegment; }
	inline void restore_page_c(uint8_t oldSegment) { *(uint8_t *)0x9000 = current_segment_c = oldSegment; }
	inline void restore_page_d(uint8_t oldSegment) { *(uint8_t *)0xB000 = current_segment_d = oldSegment; }

	inline void fast_load_page_a(uint8_t segment) { *(uint8_t *)0x5000 = current_segment_a = segment; }
	inline void fast_load_page_b(uint8_t segment) { *(uint8_t *)0x7000 = current_segment_b =segment; }
	inline void fast_load_page_c(uint8_t segment) { *(uint8_t *)0x9000 = current_segment_c =segment; }
	inline void fast_load_page_d(uint8_t segment) { *(uint8_t *)0xB000 = current_segment_d =segment; }

#elif LINUX

	#define USING_PAGE_A(module) extern const uint8_t K5_PAGE_A_ ## module
	#define USING_PAGE_B(module) extern const uint8_t K5_PAGE_B_ ## module
	#define USING_PAGE_C(module) extern const uint8_t K5_PAGE_C_ ## module
	#define USING_PAGE_D(module) extern const uint8_t K5_PAGE_D_ ## module
	#define SEGMENT_TO_PAGE_A(module) 0
	#define SEGMENT_TO_PAGE_B(module) 0
	#define SEGMENT_TO_PAGE_C(module) 0
	#define SEGMENT_TO_PAGE_D(module) 0
	
	inline static uint8_t load_page_a(uint8_t a) { (void)(a); return 0; }
	inline static uint8_t load_page_b(uint8_t a) { (void)(a); return 0; }
	inline static uint8_t load_page_c(uint8_t a) { (void)(a); return 0; }
	inline static uint8_t load_page_d(uint8_t a) { (void)(a); return 0; }
	inline static void restore_page_a(uint8_t a) { (void)(a); }
	inline static void restore_page_b(uint8_t a) { (void)(a); }
	inline static void restore_page_c(uint8_t a) { (void)(a); }
	inline static void restore_page_d(uint8_t a) { (void)(a); }
	inline static void fast_load_page_a(uint8_t a) { (void)(a); }
	inline static void fast_load_page_b(uint8_t a) { (void)(a); }
	inline static void fast_load_page_c(uint8_t a) { (void)(a); }
	inline static void fast_load_page_d(uint8_t a) { (void)(a); }


#else
	#error "Architecture Not Supported"
#endif



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
// ISR FUNCTIONS

typedef void (*isr_function)();
isr_function msxhal_install_isr(isr_function);

////////////////////////////////////////////////////////////////////////
// INIT FUNCTIONS
void msxhal_init();


