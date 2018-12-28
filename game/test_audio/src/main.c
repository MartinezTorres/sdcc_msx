#include <msxhal.h>
#include <tms9918.h>
#include <psg.h>


#include <res/ayfx/inicio_juego.afb.h>

#include <res/midi/indy4/theme_and_opening_credits.mid.h>
#include <res/midi/chopin/chpn_op10_e05.mid.h>

USING_PAGE_A(psg);
USING_PAGE_C(chpn_op10_e05_mid);
USING_PAGE_C(inicio_juego_afb);

void audio_isr();

void audio_isr() {
    
    uint8_t oldSegmentPageA = load_page_a(SEGMENT_A(psg));

    TMS9918_setRegister(7,0x77);
    ayr_spin();
    TMS9918_setRegister(7,0x44);

    TMS9918_setRegister(7,0x88);
    ayFX_spin();
    TMS9918_setRegister(7,0x44);
    
    PSG_syncRegisters();
    TMS9918_setRegister(7,0x55);

    restore_page_a(oldSegmentPageA);    
    TMS9918_setRegister(7,0);    
}

INLINE int start() {

    bool released = true;
    uint8_t bd = 0x34;
    

    TMS9918_activateMode2(false);
    
	//asdff();

    while (true) {
	
	uint8_t key = msxhal_joystick_read(0);
	
	if (key!=0 && released) {
	    
	    uint8_t oldSegmentPageA = load_page_a(SEGMENT_A(psg));
	    ayFX_afb(inicio_juego_afb,SEGMENT_C(inicio_juego_afb),key>>4,15,0);
	    restore_page_a(oldSegmentPageA);    
	    
	    bd = bd + 0x11;
	    bd = bd & 0x33;
	}
	
	released = (key==0);
	
	HALT();
    };

}

int main(void) {

    uint8_t oldSegmentPageA = load_page_a(SEGMENT_A(psg));

    msxhal_init();
    ayr_init();
    ayFX_init();
    msxhal_install_isr(&audio_isr);
    
    PSG_initRegisters();
    ayr_play(&chpn_op10_e05_mid,SEGMENT_C(chpn_op10_e05_mid));

    restore_page_a(oldSegmentPageA);    

    start();
}


