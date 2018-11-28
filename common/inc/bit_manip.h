#pragma once
#include <msxhal.h>

// BIT MANIP FUNCTIONS
uint8_t  reverse8 (uint8_t  i);
uint16_t reverse16(uint16_t i);

uint16_t *hflip8  (U8x8   s);
uint16_t *hflip16 (U16x16 s);

uint16_t *rotate8 (U8x8   s);
uint16_t *rotate16(U16x16 s);

uint16_t *load8   (U8x8   s, const U8x8   src);
uint16_t *load16  (U16x16 s, const U16x16 src);
