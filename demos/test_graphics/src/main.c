#include <msxhal.h>
#include <tms9918.h>
#include <psg.h>
#include <tile.h>

#include <common/res/fonts/itfont-ek-zelda.h>

#include <res/ayfx/inicio_juego.afb.h>

#include <res/midi/indy4/theme_and_opening_credits.mid.h>
#include <res/midi/chopin/chpn_op10_e05.mid.h>
#include <res/midi/chopin/chpn_op10_e01.mid.h>
#include <res/midi/chopin/chpn_op10_e12.mid.h>
#include <res/midi/indy4/theme_and_opening_credits.mid.h>


USING_PAGE_B(psg);
USING_PAGE_C(chpn_op10_e05_mid);
USING_PAGE_C(chpn_op10_e01_mid);
USING_PAGE_C(chpn_op10_e12_mid);
USING_PAGE_C(theme_and_opening_credits_mid);
USING_PAGE_C(inicio_juego_afb);

void audio_isr();
void audio_isr() {
    
    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));

    TMS9918_setRegister(7,0x77);
    ayr_spin();
    TMS9918_setRegister(7,0x44);

    TMS9918_setRegister(7,0x88);
    ayFX_spin();
    TMS9918_setRegister(7,0x44);
    
    PSG_syncRegisters();
    TMS9918_setRegister(7,0x55);

    restore_page_b(oldSegmentPageB);    
    TMS9918_setRegister(7,0);    
}


INLINE void loadFont();
INLINE void loadFont() {
    
    U8x8 color;
    color[0] = BTransparent + FWhite;
    color[1] = BTransparent + FWhite;
    color[2] = BTransparent + FWhite;
    color[3] = BTransparent + FWhite;
    color[4] = BTransparent + FWhite;
    color[5] = BTransparent + FWhite;
    color[6] = BTransparent + FWhite;
    color[7] = BTransparent + FWhite;

    M2_MS_loadFont(MODE2_PGPAGE_0, font_zelda, color,' ','z');
    
}


INLINE void start() {

    bool released = true;
    TMS9918_activateMode2(MODE2_ENABLE_PGPAGE_0);
    
    while (true) {
	
	uint8_t key = msxhal_joystick_read(0);
	
	if (key!=0 && released) {
	    
	    uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));
	    DI();

	    if (key==J_SPACE) ayr_play(&chpn_op10_e05_mid,SEGMENT_C(chpn_op10_e05_mid));
	    if (key==J_DOWN)  ayr_play(&chpn_op10_e01_mid,SEGMENT_C(chpn_op10_e01_mid));
	    if (key==J_UP)    ayr_play(&chpn_op10_e12_mid,SEGMENT_C(chpn_op10_e12_mid));
	    if (key==J_LEFT)  ayr_play(&theme_and_opening_credits_mid,SEGMENT_C(theme_and_opening_credits_mid));

	    ayFX_afb(inicio_juego_afb,SEGMENT_C(inicio_juego_afb),key>>4,15,0);
	    EI();
	    restore_page_b(oldSegmentPageB);    
	}
	
	released = (key==0);
	HALT();
    };

}


int main(void) {

    msxhal_init();
    {
	uint8_t oldSegmentPageB = load_page_b(SEGMENT_B(psg));
	ayr_init();
	ayFX_init();
	PSG_initRegisters();
	restore_page_b(oldSegmentPageB);
    }
	
    msxhal_install_isr(&audio_isr);
    
    start();
}


