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


void TMS9918_write(uint16_t dst, const uint8_t *src, uint16_t sz) {

	register const uint8_t *p = src;
	
	VDP1 = dst & 0xFF; 
	NOP();
	VDP1 = 0x40 | (dst>>8);
	NOP();	
	
	while (sz--) VDP0 = *p++;
}

void TMS9918_write8(uint16_t dst, const uint8_t *src, uint8_t sz8) {

	register const uint8_t *p = src;
	
	VDP1 = dst & 0xFF; 
	NOP();
	VDP1 = 0x40 | (dst>>8);
	NOP();	
	
	while (sz8--) {
		VDP0 = *p++;
		VDP0 = *p++;
		VDP0 = *p++;
		VDP0 = *p++;

		VDP0 = *p++;
		VDP0 = *p++;
		VDP0 = *p++;
		VDP0 = *p++;
	}
}


inline static void function_placeholders(void) {
	
	__asm
	_keyboard_read_old::
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

	__asm
___sdcc_call_hl:
	jp	(hl)
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


volatile uint8_t clk;
volatile T_f state_ptr;
void my_isr(void) { 

	__asm
		push af
	__endasm;

	//setTMS9918_setRegister(7,BLightRed+FWhite);
	clk++; 

	__asm
		ld A,#0x3
		ld (#0xF3F6),A

		pop af
	__endasm;
}

void TMS9918_waitFrame() {
	
	while (clk==0);
	clk=0;
}

int main(void) {
	
	state_ptr = (T_f)start;
    new_isr = my_isr;
    install_isr_rom();
    while (TRUE) {
		state_ptr = (T_f)((*state_ptr)());
		TMS9918_waitFrame();
		//setTMS9918_setRegister(7,BBlack+FWhite);
	};
}
