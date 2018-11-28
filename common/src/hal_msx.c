#include <msxhal.h>

#ifdef MSX

inline static void joystick_read_placeholder(void) {
	__asm
	joystick_read:
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

	// WHat the heck is this?
	__asm
___sdcc_call_hl:
	jp	(hl)
	__endasm;
}	

static volatile uint8_t clk;
static void (*custom_isr)(void);
void msx_hal_isr(void) { 

	__asm
		push af
	__endasm;

	//setTMS9918_setRegister(7,BLightRed+FWhite);
	clk++; 
	
	if (custom_isr != nullptr)
		(*custom_isr)();

	__asm
		ld A,#0x3 // Skip the bios routine that reads the keyboard!
		ld (#0xF3F6),A

		pop af
	__endasm;
}

void install_isr(void (*new_isr)(void)) {
	
	DI();	
	custom_isr = new_isr;
	EI();
}

void wait_frame() {
	
	while (clk==0);
	clk=0;
}

void init() {

	custom_isr = nullptr;
	// Install our ISR 
	
	DI();
	__asm
		push ix
		ld ix,#0
		add ix,sp

		; Set new ISR vector
		ld A,#0xC3
		ld (#0xFD9F),A
		ld hl,(_msx_hal_isr)
		ld (#0xFD9F+1),hl
		pop ix
	__endasm;
	EI();
}


#endif
