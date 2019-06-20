//#define DEBUG
#include <msxhal.h>
#include <tms99X8.h>

#include <res/gifv01/nyan.h>

USING_PAGE_B(nyan);

int main(void) {

    msxhal_init();
    
    load_page_b(SEGMENT_TO_PAGE_B(nyan));
    while (true) {
	nyan_play();
	wait_frame();
    }
}


