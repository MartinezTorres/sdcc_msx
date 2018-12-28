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

#define swap(a,b,c) do { a k_=b; b=c; c=k_; } while(false)

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
	#define DI(a)   do { __asm di   __endasm;  } while (false)
	#define EI(a)   do { __asm ei   __endasm;  } while (false)
	#define HALT(a) do { __asm halt __endasm;  } while (false)
	
	#define HASH_SIGN #
	#define ZERO(DATA,N) do {\
	__asm \
		push de \
		push af \
		ld de,HASH_SIGN _##DATA \
		xor a, a \
	__endasm; \
		REPEAT( __asm__("ld (de),a"); __asm__("inc de");, N) \
	__asm \
		pop af \
		pop de \
	__endasm; } while (false)
	
	inline int printf(const char *f, ...) { (f); return 0;}

#elif LINUX

	void wait_frame();

	#define NOP(a)  do { } while (false)
	#define DI(a)   do { } while (false)
	#define EI(a)   do { } while (false)
	#define HALT(a) do { wait_frame(); } while (false)

	#include <stdio.h>
	#include <string.h>

	#define ZERO(DATA,N) memset(&DATA,0,N)

#else
	#error "Architecture Not Supported"
#endif

////////////////////////////////////////////////////////////////////////
// BLOCK FUNCTIONS


#ifdef MSX

	#define USING_PAGE_A(module) uint8_t K5_PAGE_A_ ## module ()
	#define USING_PAGE_B(module) uint8_t K5_PAGE_B_ ## module ()
	#define USING_PAGE_C(module) uint8_t K5_PAGE_C_ ## module ()
	#define SEGMENT_A(module) ((uint8_t)(K5_PAGE_A_ ## module ()))
	#define SEGMENT_B(module) ((uint8_t)(K5_PAGE_B_ ## module ()))
	#define SEGMENT_C(module) ((uint8_t)(K5_PAGE_C_ ## module ()))

	extern volatile uint8_t current_segment_a;
	extern volatile uint8_t current_segment_b;
	extern volatile uint8_t current_segment_c;
	
	inline uint8_t load_page_a(uint8_t newSegment) { register uint8_t oldSegment = current_segment_a;
		*(uint8_t *)0x7000 = current_segment_a = newSegment; return oldSegment; }

	inline uint8_t load_page_b(uint8_t newSegment) { register uint8_t oldSegment = current_segment_b;
		*(uint8_t *)0x9000 = current_segment_b = newSegment; return oldSegment; }

	inline uint8_t load_page_c(uint8_t newSegment) { register uint8_t oldSegment = current_segment_c;
		*(uint8_t *)0xB000 = current_segment_c = newSegment; return oldSegment; }

	inline void restore_page_a(uint8_t oldSegment) { *(uint8_t *)0x7000 = current_segment_a = oldSegment; }
	inline void restore_page_b(uint8_t oldSegment) { *(uint8_t *)0x9000 = current_segment_b = oldSegment; }
	inline void restore_page_c(uint8_t oldSegment) { *(uint8_t *)0xB000 = current_segment_c = oldSegment; }

#elif LINUX

	#define USING_PAGE_A(module) 
	#define USING_PAGE_B(module) 
	#define USING_PAGE_C(module) 
	#define SEGMENT_A(module) 0
	#define SEGMENT_B(module) 0
	#define SEGMENT_C(module) 0
	
	inline static uint8_t load_page_a(uint8_t a) { (void)(a); return 0; }
	inline static uint8_t load_page_b(uint8_t a) { (void)(a); return 0; }
	inline static uint8_t load_page_c(uint8_t a) { (void)(a); return 0; }
	inline static void restore_page_a(uint8_t a) { (void)(a); }
	inline static void restore_page_b(uint8_t a) { (void)(a); }
	inline static void restore_page_c(uint8_t a) { (void)(a); }

#else
	#error "Architecture Not Supported"
#endif



////////////////////////////////////////////////////////////////////////
// IO FUNCTIONS
enum    { J_RIGHT=0x80,J_DOWN=0x40,J_UP=0x20,J_LEFT=0x10,J_DEL=0x08,J_INS=0x04,J_HOME=0x02,J_SPACE=0x01 };
uint8_t msxhal_joystick_read(uint8_t joystickId);

////////////////////////////////////////////////////////////////////////
// CORE FUNCTIONS
void msxhal_init();
void msxhal_install_isr(void (*)());

