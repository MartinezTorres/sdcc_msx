#include <msxhal.h>
#include <tms9918.h>
#include <psg.h>

#include <res/ayfx/inicio_juego.afb.h>

int main(void) {
    
    bool released = true;
    
    uint8_t bd = 0x34;
    msxhal_init();
    ayFX_init();
    TMS9918_activateMode2(false);

    

    while (true) {
	
	uint8_t key = msxhal_joystick_read(0);
	
	if (key!=0 && released) {
	    
	    ayFX_afb(inicio_juego_afb,key>>4,15,0);
	    
	    bd = bd + 0x11;
	    bd = bd & 0x33;
	}
	
	released = (key==0);
	
	PSG_initRegisters();
	HALT();
	DI();
	TMS9918_setRegister(7,0x88);
	ayFX_spin();
	TMS9918_setRegister(7,0x44);
	PSG_syncRegisters();
	TMS9918_setRegister(7,0);
	EI();
    };
}

