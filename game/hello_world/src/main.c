#include <msxhal.h>
#include <tms9918.h>

int main(void) {
	
	//state_ptr = (T_f)start;
    //new_isr = my_isr;
    //install_isr_rom();
    init();
    while (true) {
	//state_ptr = (T_f)((*state_ptr)());
	wait_frame();
	//setTMS9918_setRegister(7,BBlack+FWhite);
    };
}

