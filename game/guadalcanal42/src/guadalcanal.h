#pragma once
#include <msxhal.h>

#ifdef DEBUG
INLINE void debugTimming(uint8_t v) {
    TMS99X8_setRegister(7,v);
}
#else
INLINE void debugTimming() {}
#endif
