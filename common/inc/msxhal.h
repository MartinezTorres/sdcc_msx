#pragma once

#include <stdint.h>

typedef enum {
	false = 0,
	true = 1 
} bool;

#define nullptr ((void *)0)

#define cropped(a,b,c) (a<(b)?(b):(a>(c)?(c):a))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

#define REPEAT2(a)  { {a}; {a}; }
#define REPEAT4(a)  { REPEAT2(a);  REPEAT2(a);  }
#define REPEAT8(a)  { REPEAT4(a);  REPEAT4(a);  }
#define REPEAT16(a) { REPEAT8(a);  REPEAT8(a);  }
#define REPEAT32(a) { REPEAT16(a); REPEAT16(a); }

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


#ifdef MSX

	#define NOP(a) __asm nop __endasm
	#define DI(a) __asm nop __endasm
	#define EI(a) __asm nop __endasm

	#define printf(...)

#elif LINUX

	#define NOP(a)
	#define DI(a)
	#define EI(a)

	#include <stdio.h>
#else
	#error "Architecture Not Supported"
#endif


// IO FUNCTIONS
enum    { J_RIGHT=0x80,J_DOWN=0x40,J_UP=0x20,J_LEFT=0x10,J_DEL=0x08,J_INS=0x04,J_HOME=0x02,J_SPACE=0x01 };
uint8_t joystick_read(uint8_t joystickId);

// CORE FUNCTIONS
void init();
void wait_frame();
void install_isr(void (*)());



