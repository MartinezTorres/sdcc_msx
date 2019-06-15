//#define DEBUG

#include <msxhal.h>
#include <tms99X8.h>
#include <psg.h>
#include <monospace.h>

#include "saitama/saitama.h"

#include <common/res/fonts/itfont-ek-zelda.h>

#ifdef DEBUG
INLINE void debugTimming(uint8_t v) {
    TMS99X8_setRegister(7,v);
}
#else
INLINE void debugTimming() {}
#endif



USING_PAGE_B(psg);
USING_PAGE_B(saitama);

void audio_isr(); 
INLINE void audio_init() {
	
	uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));
	ayr_init();
	ayFX_init();
	PSG_init();
	restore_page_b(oldSegmentPageB);

    msxhal_install_isr(&audio_isr);
}

uint8_t frameNumber;
void audio_isr() { 
    
    uint8_t oldSegmentPageB;
    frameNumber++;
    return;
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
    debugTimming(0x00);
}


int main(void) {

    msxhal_init();
    audio_init();
    
    load_page_b(SEGMENT_B(saitama));
    while (true) {
	saitama_play();
	//startMenu();
	HALT();
    }
}


