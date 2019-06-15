#pragma once
#include <msxhal.h>

////////////////////////////////////////////////////////////////////////
// Random Number Generator API
//

uint16_t rand16();
void rand16_seed(uint16_t seed1, uint16_t seed2);
void rand16_seedTime();

uint8_t rand7();
