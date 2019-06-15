#include <gif_v00.h>

static volatile uint8_t gif_v00_frames_left;
static volatile uint8_t gif_v00_bufferId;

static isr_function old_isr;
static void gif_v00_isr();
void gif_v00_initVideo() {
	
	TMS99X8_activateMode2(MODE2_ALL_ROWS);
	HALT();
    gif_v00_frames_left = 5;
    gif_v00_bufferId = 0;
    old_isr = msxhal_install_isr(&gif_v00_isr);
}

void gif_v00_deinitVideo() {

    msxhal_install_isr(old_isr);
}


static void gif_v00_isr() { 
    
//    uint8_t oldSegmentPageB;
	gif_v00_bufferId = !gif_v00_bufferId;
    gif_v00_frames_left--;
    TMS99X8_activateBuffer(gif_v00_bufferId);
/*    return;
    debugTimming(0x55);
    oldSegmentPageB = load_page_b(SEGMENT_B(psg));

    debugTimming(0x88);
    ayr_spin();
    debugTimming(0x44);

    debugTimming(0x88);
    ayFX_spin();

    debugTimming(0x44);    
    PSG_syncRegisters();

    debugTimming(0x55);
    restore_page_b(oldSegmentPageB);    
    debugTimming(0x00);*/
}



static void gif_v00_copyToPN0(const uint8_t *patternNames) {
	TMS99X8_memcpy(MODE2_ADDRESS_PN0+(32* 0),patternNames+(32* 0),32*4);
	TMS99X8_memcpy(MODE2_ADDRESS_PN0+(32* 4),patternNames+(32* 4),32*4);
	TMS99X8_memcpy(MODE2_ADDRESS_PN0+(32* 8),patternNames+(32* 8),32*4);
	TMS99X8_memcpy(MODE2_ADDRESS_PN0+(32*12),patternNames+(32*12),32*4);
	TMS99X8_memcpy(MODE2_ADDRESS_PN0+(32*16),patternNames+(32*16),32*4);
	TMS99X8_memcpy(MODE2_ADDRESS_PN0+(32*20),patternNames+(32*20),32*4);
}

static uint8_t msg[32*4];
static void gif_v00_copyToPN1(const uint8_t *patternNames) {

	uint8_t i;
	for (i=0; i<24; i+=4) {
		uint8_t j;
		for (j=0; j<32*4; j++) 
			msg[j] = *patternNames++ + 128;
		TMS99X8_memcpy(MODE2_ADDRESS_PN1+(32*i),msg,32*4);
	}
}

void gif_v00_sendPatternNames(const uint8_t *patternNames) {
	
	while (gif_v00_frames_left<5) HALT();
	gif_v00_frames_left = 0;
	if (gif_v00_bufferId) gif_v00_copyToPN0(patternNames); else gif_v00_copyToPN1(patternNames);
	while (gif_v00_frames_left<5) HALT();
	gif_v00_frames_left = (msxhal_get_interrupt_frequency() == MSX_FREQUENCY_60HZ ? 3 : 2);
	if (gif_v00_bufferId) gif_v00_copyToPN0(patternNames); else gif_v00_copyToPN1(patternNames);
}

#ifdef MSX

inline static void gif_v00_copy_asm_placeholder(void) {
	
	{__asm
	_gif_v00_copy16_asm::
		call _gif_v00_copy8_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy15_asm::
		call _gif_v00_copy7_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy14_asm::
		call _gif_v00_copy6_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy13_asm::
		call _gif_v00_copy5_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy12_asm::
		call _gif_v00_copy4_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy11_asm::
		call _gif_v00_copy3_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy10_asm::
		call _gif_v00_copy2_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy9_asm::
		call _gif_v00_copy1_asm
		call _gif_v00_copy8_asm
		ret

	_gif_v00_copy8_asm::

		ld b,#07
	00001$:
		outi
		jp nz,00001$
		outi
		nop
		
	_gif_v00_copy7_asm::
		ld b,#07
	00002$:
		outi
		jp nz,00002$
		outi
		add hl,de

	_gif_v00_copy6_asm::
		ld b,#07
	00003$:
		outi
		jp nz,00003$
		outi
		nop

	_gif_v00_copy5_asm::
		ld b,#07
	00004$:
		outi
		jp nz,00004$
		outi
		nop

	_gif_v00_copy4_asm::
		ld b,#07
	00005$:
		outi
		jp nz,00005$
		outi
		nop

	_gif_v00_copy3_asm::
		ld b,#07
	00006$:
		outi
		jp nz,00006$
		outi
		nop

	_gif_v00_copy2_asm::
		ld b,#07
	00007$:
		outi
		jp nz,00007$
		outi
		nop

	_gif_v00_copy1_asm::
		ld b,#07
	00008$:
		outi
		jp nz,00008$
		outi
		nop

		ret
	__endasm;}


	{
	__asm
	_gif_v00_copy_inc16_asm::
		call _gif_v00_copy_inc8_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc15_asm::
		call _gif_v00_copy_inc7_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc14_asm::
		call _gif_v00_copy_inc6_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc13_asm::
		call _gif_v00_copy_inc5_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc12_asm::
		call _gif_v00_copy_inc4_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc11_asm::
		call _gif_v00_copy_inc3_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc10_asm::
		call _gif_v00_copy_inc2_asm
		call _gif_v00_copy_inc8_asm
		ret

	_gif_v00_copy_inc9_asm::
		call _gif_v00_copy_inc4_asm
		call _gif_v00_copy_inc5_asm
		ret

	_gif_v00_copy_inc8_asm::

		add hl,de
		ld b,#07
	00001$:
		outi
		jp nz,00001$
		outi
		
	_gif_v00_copy_inc7_asm::
		add hl,de
		ld b,#07
	00002$:
		outi
		jp nz,00002$
		outi

	_gif_v00_copy_inc6_asm::
		add hl,de
		ld b,#07
	00003$:
		outi
		jp nz,00003$
		outi

	_gif_v00_copy_inc5_asm::
		add hl,de
		ld b,#07
	00004$:
		outi
		jp nz,00004$
		outi

	_gif_v00_copy_inc4_asm::
		add hl,de
		ld b,#07
	00005$:
		outi
		jp nz,00005$
		outi

	_gif_v00_copy_inc3_asm::
		add hl,de
		ld b,#07
	00006$:
		outi
		jp nz,00006$
		outi

	_gif_v00_copy_inc2_asm::
		add hl,de
		ld b,#07
	00007$:
		outi
		jp nz,00007$
		outi

		add hl,de
		ld b,#07
	00008$:
		outi
		jp nz,00008$
		outi

		ret
	__endasm;
	}
}

#else


const uint8_t *gif_v00_source;
uint16_t gif_v00_target;

#endif
