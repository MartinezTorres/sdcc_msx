#include <stdint.h>
#include <msx1hal.h>
#include <util.h>

// VDP EMULATOR
#define N_SPRITES 32
#define TILE_WIDTH 32
#define TILE_HEIGHT 24
#define TEX_WIDTH (TILE_WIDTH*8)
#define TEX_HEIGHT (TILE_HEIGHT*8)
#define SCREEN_WIDTH (TEX_WIDTH*4)
#define SCREEN_HEIGHT (TEX_HEIGHT*4)

__sfr __at 0x98 VDP0;
__sfr __at 0x99 VDP1;

inline static void NOP(void) { __asm nop __endasm; }
inline static void DI(void) { __asm di __endasm; }
inline static void EI(void) { __asm ei __endasm; }

struct {
	union {
		uint8_t reg[8];
		struct {
			struct {
				uint8_t extvid : 1;		
				uint8_t mode2 : 1;
				uint8_t reserved1 : 6;
			};
			struct {
				uint8_t magnifySprites : 1;
				uint8_t sprites16 : 1;
				uint8_t reserved2: 1;
				uint8_t mode3 : 1;
				uint8_t mode1 : 1;
				uint8_t generateInterrupts : 1;
				uint8_t blankScreen_ : 1;
				uint8_t mem416K : 1;
			};
			uint8_t pn10, ct6, pg11, sa7, sg11;
			struct {
				uint8_t backdrop : 4;
				uint8_t textcolor : 4;
			};
		};
	};
} TMS9918Status;


inline static void setTMS9918_setRegisterFast(uint8_t reg, uint8_t val) {

	VDP1 = val; 
	NOP();
	VDP1 = 0x80 | reg;
	NOP();	
}


void setTMS9918_setMode2() {
	
	TMS9918Status.reg[0] = 0;
	TMS9918Status.reg[1] = 0;

	TMS9918Status.mode2 = 1;
	TMS9918Status.blankScreen_ = 1;
	TMS9918Status.generateInterrupts = 1;
	TMS9918Status.mem416K = 1;
	
	TMS9918Status.pn10 =  ADDRESS_PN0 >> 10;
	TMS9918Status.ct6  = (ADDRESS_CT  >>  6) | 0b01111111;
	TMS9918Status.pg11 = (ADDRESS_PG  >> 11) | 0b00000011;
	TMS9918Status.sa7  =  ADDRESS_SA0 >>  7;
	TMS9918Status.sg11 =  ADDRESS_SG >> 11;
	
	TMS9918Status.backdrop = BBlack;
	TMS9918Status.textcolor = BWhite;
	
	setTMS9918_setRegisterFast(0,TMS9918Status.reg[0]);
	setTMS9918_setRegisterFast(1,TMS9918Status.reg[1]);
	setTMS9918_setRegisterFast(2,TMS9918Status.reg[2]);
	setTMS9918_setRegisterFast(3,TMS9918Status.reg[3]);
	setTMS9918_setRegisterFast(4,TMS9918Status.reg[4]);
	setTMS9918_setRegisterFast(5,TMS9918Status.reg[5]);
	setTMS9918_setRegisterFast(6,TMS9918Status.reg[6]);
	setTMS9918_setRegisterFast(7,TMS9918Status.reg[7]);
}

void setTMS9918_activatePage0() {

	setTMS9918_setRegisterFast(2, ADDRESS_PN0 >> 10);
	setTMS9918_setRegisterFast(5, ADDRESS_SA0 >>  7);
}

void setTMS9918_activatePage1() {
	
	setTMS9918_setRegisterFast(2, ADDRESS_PN0 >> 10);
	setTMS9918_setRegisterFast(5, ADDRESS_SA0 >>  7);
}

void setTMS9918_setRegister(uint8_t reg, uint8_t val) {
	
	setTMS9918_setRegisterFast(reg,val);
}

void setTMS9918_write(uint16_t dst, uint8_t *src, uint16_t sz) {
	

	VDP1 = dst & 0xFF; 
	NOP();
	VDP1 = 0x40 | (dst>>8);
	NOP();	
	
	while (sz--) VDP0 = *src++;
}

inline static void keyboard_placeholder(void) {
	
	__asm
_keyboard_read::
	push ix
	ld ix,#0
	add ix,sp

	in a,(#0xAA)
	and #0xF0       ; only change bits 0-3
	or #8           ; row 8
	out (#0xAA),a
	in a,(#0xA9)    ; read row into A
	xor #0xFF
	ld l,a

	pop ix
	ret
	__endasm;
}



void (*new_isr)(void);
void install_isr_rom() {
	
	__asm
		push ix
		ld ix,#0
		add ix,sp

		; Set new ISR vector
		di
		ld A,#0xC3
		ld (#0xFD9F),A
		ld hl,(_new_isr)
		ld (#0xFD9F+1),hl
		ei

		pop ix
	__endasm;
}


T_f state_ptr;
void my_isr(void)  {

	state_ptr = (T_f)((*state_ptr)());
}

	
int main(void) {
	
	state_ptr = (T_f)start;
    
    new_isr = my_isr;
    
    install_isr_rom();
    while (1);
    return 1;
}
