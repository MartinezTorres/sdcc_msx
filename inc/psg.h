#pragma once

#include <msxhal.h>

typedef struct {
	union {
		uint8_t reg[14];
		struct {
			uint16_t tone[3];
			uint8_t noise_period;
			union {
				struct {
					uint8_t tone_a : 1;
					uint8_t tone_b : 1;
					uint8_t tone_c : 1;
					uint8_t noise_a : 1;
					uint8_t noise_b : 1;
					uint8_t noise_c : 1;
					uint8_t ioa_out : 1;
					uint8_t iob_out : 1;
				};
				uint8_t value;
			} enable;
			union {
				struct {
					uint8_t l0 : 1;
					uint8_t l1 : 1;
					uint8_t l2 : 1;
					uint8_t l3 : 1;
					uint8_t  m : 1;
				};
				struct {
					uint8_t volume : 4;
				};
				uint8_t value;
			} amplitude[3];
			uint16_t envelope_period;
			union {
				struct {
					uint8_t envelope_shape_cycle_hold : 1;
					uint8_t envelope_shape_cycle_alt : 1;
					uint8_t envelope_shape_cycle_att : 1;
					uint8_t envelope_shape_cycle_cont : 1;
				};
				uint8_t envelope_shape_cycle;
			};
		};
	};
} 
T_AY_3_8910_Registers;

extern T_AY_3_8910_Registers AY_3_8910_Registers;

void PSG_init();
void PSG_syncRegisters();

void ayFX_init();
void ayFX_afx(const uint8_t *afx, uint8_t segment, uint8_t priority, int8_t adjustedVolume);
void ayFX_afb(const uint8_t *afb, uint8_t segment, uint8_t idx, uint8_t priority, int8_t adjustedVolume);
void ayFX_spin();

typedef struct { const uint8_t *channels[3]; } AYR;

void ayr_init();
void ayr_play(const AYR *ayr, uint8_t segment);
void ayr_spin();


