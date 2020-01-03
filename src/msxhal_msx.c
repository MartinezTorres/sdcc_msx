#include <msxhal.h>

#ifdef MSX

uint8_t mapper_current_segments[4];

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
	custom_isr = new_isr;
	return old;
}

void msxhal_init() {

	__asm
		di
	__endasm;

	//skip_keyboard_routine = false;
	enable_keyboard_routine = true;

	custom_isr = nullptr;
	// Install our ISR 
	
	mapper_current_segments[0] = 0;
	mapper_current_segments[1] = 1;
	mapper_current_segments[2] = 2;
	mapper_current_segments[3] = 3;

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
}


inline static void joystick_read_placeholder(void) {
	
	__asm
	_msxhal_joystick_read::
;		push ix
;		ld ix,#0
;		add ix,sp
		
		in a,(#0xAA)
		and #0xF0       ; only change bits 0-3
		or #8           ; row 8
		out (#0xAA),a
		in a,(#0xA9)    ; read row into A
		xor #0xFF
		ld l,a
		
;		pop ix
		ret
	__endasm;

	// WHat the heck is this? 
	// Update: now I know! To implement function pointers!
	__asm
___sdcc_call_hl::
	jp	(hl)
___sdcc_call_ix::
	jp	(ix)
___sdcc_call_iy::
	jp	(iy)
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

#endif
