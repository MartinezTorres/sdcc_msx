#pragma once

#include <tms99X8.h>

extern uint8_t gif_v00_frames_left;
extern uint8_t gif_v00_hz;

void gif_v00_copy1();
void gif_v00_copy2();
void gif_v00_copy3();
void gif_v00_copy4();
void gif_v00_copy5();
void gif_v00_copy6();
void gif_v00_copy7();
void gif_v00_copy8();

void gif_v00_setTargetAddress(uint16_t address);
void gif_v00_setSourceAddress(uint16_t address);
void gif_v00_setNameTableOffset(uint8_t offset);

void gif_v00_fillNameTableRow(const uint8_t *data);
