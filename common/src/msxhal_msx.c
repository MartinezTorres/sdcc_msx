#include <msxhal.h>

#ifdef MSX

volatile uint8_t current_segment_a;
volatile uint8_t current_segment_b;
volatile uint8_t current_segment_c;
volatile uint8_t current_segment_d;

uint8_t *__PHONY_POINTER__;

volatile uint8_t interrupt_count;

volatile bool enable_keyboard_routine;

static isr_function custom_isr;
void msx_hal_isr(void) { 

	interrupt_count++;

	if (custom_isr != nullptr)
		(*custom_isr)();		

	//enable_keyboard_routine = false;
	if (!enable_keyboard_routine) {
		*((uint8_t *)(0xF3F6)) = 3;
	}
}

isr_function msxhal_install_isr(isr_function new_isr) {
	
	register isr_function old = custom_isr;
	DI();
	custom_isr = new_isr;
	EI();
	return old;
}

void msxhal_init() {

	//skip_keyboard_routine = false;
	enable_keyboard_routine = true;

	custom_isr = nullptr;
	// Install our ISR 
	
	current_segment_a = 0;
	current_segment_b = 1;
	current_segment_c = 2;
	current_segment_d = 3;

	DI();
	__asm
		push ix
		ld ix,#0
		add ix,sp
		
		ld A,#0x00
		ld (#0x5000),A ; // Konami5 mapper init
		ld (#0x5000),A ; // Konami5 mapper init
		ld (#0x5000),A ; // Konami5 mapper init
		ld (#0x5000),A ; // Konami5 mapper init

		inc A
		ld (#0x7000),A ; // Konami5 mapper init
		ld (#0x7000),A ; // Konami5 mapper init
		ld (#0x7000),A ; // Konami5 mapper init
		ld (#0x7000),A ; // Konami5 mapper init

		inc A
		ld (#0x9000),A ; // Konami5 mapper init
		ld (#0x9000),A ; // Konami5 mapper init
		ld (#0x9000),A ; // Konami5 mapper init
		ld (#0x9000),A ; // Konami5 mapper init

		inc A
		ld (#0xB000),A ; // Konami5 mapper init
		ld (#0xB000),A ; // Konami5 mapper init
		ld (#0xB000),A ; // Konami5 mapper init
		ld (#0xB000),A ; // Konami5 mapper init

		; Set new ISR vector
		ld A,#0xC3 ; opcode for JP
		ld (#0xFD9F),A
		
		ld hl,#_msx_hal_isr
		ld (#0xFD9F+1),hl
		pop ix
	__endasm;
	EI();
}


inline static void joystick_read_placeholder(void) {
	
	__asm
	_msxhal_joystick_read::
;		push ix
;		ld ix,#0
;		add ix,sp
		
		di

		in a,(#0xAA)
		and #0xF0       ; only change bits 0-3
		or #8           ; row 8
		out (#0xAA),a
		in a,(#0xA9)    ; read row into A
		xor #0xFF
		ld l,a
		
		ei

;		pop ix
		ret
	__endasm;

	// WHat the heck is this?
	__asm
___sdcc_call_hl::
	jp	(hl)
	__endasm;
}


inline static void msxhal_getch_placeholder(void) {
	
	__asm
	_msxhal_getch::
		call #0x009C      ; call CHSNS
		jr nz,00001$    ; skip if there is no buffer
		ld l,#0
		ret
00001$:
		call #0x009f		; call CHGET
		ld l,a
		ret
	__endasm;	


}
/*
void memset(uint8_t *d, uint8_t c, uint8_t n) { 

	register uint8_t *dd = d;
	register uint8_t nn = n;
	register uint8_t cc = c;
	while (nn--) 
		*dd++ = cc; 
}
void memset8(uint8_t *d, uint8_t c, uint8_t n) { 

	register uint8_t *dd = d;
	register uint8_t nn = n;
	register uint8_t cc = c;
	while (nn--) 
		REPEAT8(*dd++ = cc;) 
}
void memcpy(uint8_t *d, uint8_t *s, uint8_t n) { 

	register uint8_t *ss = s;
	register uint8_t *dd = d;
	register uint8_t nn = n;
	while (nn--) 
		*dd++ = *ss++; 
}
void memcpy8(uint8_t *d, uint8_t *s, uint8_t n) { 

	register uint8_t *dd = d;
	register uint8_t *ss = s;
	register uint8_t nn = n;
	while (nn--) 
		REPEAT8(*dd++ = *ss++;) 
}*/

#endif
