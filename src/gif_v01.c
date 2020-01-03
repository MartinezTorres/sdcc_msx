#include <gif_v01.h>
#include <psg.h>

static volatile uint8_t gif_v01_oldSegmentC;
static volatile uint8_t gif_v01_oldSegmentD;
volatile uint8_t gif_v01_frames_left;
volatile uint8_t gif_v01_bufferId;

static isr_function old_isr;

USING_MODULE(psg, PAGE_B);

static void gif_v01_isr() { 
    
    uint8_t oldSegmentB;
    
    gif_v01_bufferId = !gif_v01_bufferId;
    gif_v01_frames_left--;
    TMS99X8_activateBuffer(gif_v01_bufferId);

//    TMS99X8_setRegister(7,BWhite);

    oldSegmentB = mapper_load_module(psg, PAGE_B);
    
    PSG_init();    
    ayr_spin();
    ayFX_spin();
    PSG_syncRegisters();

   	mapper_load_segment(oldSegmentB, PAGE_B);    

//    TMS99X8_setRegister(7,BBlack);    
}

void gif_v01_initVideo() {

    TMS99X8_activateMode2(MODE2_ALL_ROWS);
    wait_frame();

    gif_v01_frames_left = 5;
    gif_v01_bufferId = 0;
    old_isr = msxhal_install_isr(&gif_v01_isr);
    gif_v01_oldSegmentC = CURRENT_SEGMENT(PAGE_C);
    gif_v01_oldSegmentD = CURRENT_SEGMENT(PAGE_D);
}

void gif_v01_deinitVideo() {

   	mapper_load_segment(gif_v01_oldSegmentD, PAGE_D);    
   	mapper_load_segment(gif_v01_oldSegmentC, PAGE_C);    
    msxhal_install_isr(old_isr);
}

void gif_v01_pre_sendPN(void) { while (gif_v01_frames_left<50) wait_frame(); gif_v01_frames_left = 0; }
void gif_v01_mid_sendPN(void) { while (gif_v01_frames_left<50) wait_frame(); gif_v01_frames_left = (msxhal_get_interrupt_frequency() == MSX_FREQUENCY_60HZ ? 3 : 2); }





#ifdef MSX

void gif_v01_clean_registers(void) {}

inline static void _gif_v01_copy_vpn_x8_asm_placeholder(void) {
	
	{__asm
	_gif_v01_copy_vpn_x8_asm::
		
		ld c,#0x98

		ld e,#0x08
	00001$:
		ld b,d
	00002$:
		outi
		jp nz,00002$
		outi
		dec e
		jp nz,00001$
		ei
		nop
		di
		ret
	__endasm;
	}
}


inline static void _gif_v01_copy_pn_asm_placeholder(void) {
	

	{__asm
	_gif_v01_copy_pn_asm::
		
		ld a,(_gif_v01_bufferId)
		or a
		jp nz, _gif_v01_copy_pn_asm_1

		in a,(#0x99)
		ld a,e
		out (#0x99),a
		ld a,#(0x40+0x18)
		add a,d
		out (#0x99),a

		ld c,#0x98
	00003$:
		outi
		jp nz,00003$
		ei
		nop
		di
		ret
	__endasm;
	}
	
	{__asm
	_gif_v01_copy_pn_asm_1::
		
		ld a,e
		out (#0x99),a
		ld a,#(0x40+0x1C)
		add a,d
		out (#0x99),a

		ld e,#0x80
	00004$:
		ld a,(hl)
		inc hl
		add a,e
		out (#0x98),a
		dec b
		jp nz,00004$
		ei
		nop
		di
		ret
	__endasm;
	}	
}

#else


#endif






