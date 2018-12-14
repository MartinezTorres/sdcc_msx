#include <psg.h>

T_AY_3_8910_Registers AY_3_8910_Registers;

void PSG_initRegisters() {
	
	ZERO(AY_3_8910_Registers,14);
}

#ifdef MSX
	__sfr __at 0xA0 PSG0;
	__sfr __at 0xA1 PSG1;
	
	
	void PSG_syncRegisters_asm();
	
	static void holder() {

		__asm
_PSG_syncRegisters_asm::
			XOR A
			LD C,#0xA0
			LD HL,#_AY_3_8910_Registers
__LOUT:		OUT (C),A
			INC C
			OUTI 
			DEC C
			INC A
			CP #13
			JR NZ,__LOUT
			OUT (C),A
			LD A,(HL)
			AND A
			RET M
			INC C
			OUT (C),A
			RET	
		__endasm;
		
	}


	void PSG_syncRegisters() {
		
		AY_3_8910_Registers.enable.ioa_out = 0;
		AY_3_8910_Registers.enable.iob_out = 1;
		PSG_syncRegisters_asm();
	}

#else

	void PSG_syncRegisters() {}
#endif



////////////////////////////////////////////////////////////////////////
// AYFX PLAYER

//16-bit numbers stored in the LSB,MSB format.
//Single effect, file extension .afx 
//Every frame encoded with a flag byte and a number of bytes, which is vary depending from value change flags.
 //bit0..3  Volume
 //bit4     Disable T
 //bit5     Change Tone
 //bit6     Change Noise
 //bit7     Disable N
//When the bit5 set, two bytes with tone period will follow; when the bit6 set, a single byte with noise period will follow; when both bits are set, first two bytes of tone period, then single byte with noise period will follow. When none of the bits are set, next flags byte will follow. 
//End of the effect is marked with byte sequence #D0, #20. Player should detect it before outputting it to the AY registers, by checking noise period value to be equal #20. The editor considers last non-zero volume value as the last frame of an effect, other parameters aren't matter. 
//Effects bank, file extension .afb 
//Header: 

//+0 (1 byte) Total number of effects in the bank, up to 256 (0 means 256);
//+1 (2 bytes per effect) Table of offsets to data of every effect. Offset value is given relative to the second byte of the offset itself, this allows to calculate absolute address very fast: 

   //hl=offset in the effects table
   //ld c,(hl)
   //inc hl
   //ld b,(hl)
   //add hl,bc
   //hl=absolute address of effect data
//Data: 
//+n (m bytes) The effects data, format is the same as in the single effect file. After every effect there could be a null terminated text string with name of the effect. It may absent, if the bank was saved into a file using corresponding item of the File menu, in this case null terminator bytes are absent as well.

typedef union {
	uint8_t value;
	struct {
		uint8_t volume   : 4;
		uint8_t disableT : 1;
		uint8_t changeT  : 1;
		uint8_t changeN  : 1;
		uint8_t disableN : 1;
	};
} T_ayFX_Flags;


typedef struct {
	const uint8_t *afx;
	uint16_t tone;
	uint8_t noise;
	uint8_t priority;
	int8_t adjustedVolume;
} T_ayFX_Effect;

#define AYFX_MAX_EFFECTS 8
typedef struct {
	
	T_ayFX_Effect effects[AYFX_MAX_EFFECTS];
	uint8_t sortedEffects[AYFX_MAX_EFFECTS];
	uint8_t nEffects;
	
	uint8_t channel;
	
} T_ayFX_Status;

static T_ayFX_Status ayFX_status;

void ayFX_init() {
		
	ayFX_status.nEffects = 0;
	{
		uint8_t i;
		for (i=0; i<AYFX_MAX_EFFECTS; i++)
			ayFX_status.sortedEffects[i] = i;
	}
}

static inline T_ayFX_Effect *ayFX_effect(uint8_t idx) {
	
	return &ayFX_status.effects[ayFX_status.sortedEffects[idx]];
}

static void ayFX_remove(uint8_t idx) {
	
	ayFX_status.nEffects--;
	while (idx<ayFX_status.nEffects) {
		swap(uint8_t,ayFX_status.sortedEffects[idx], ayFX_status.sortedEffects[idx+1]);
		idx++;
	}
}


void ayFX_afx(const uint8_t *afx, uint8_t priority, int8_t adjustedVolume) {
	

	uint8_t idx = ayFX_status.nEffects;

	if (idx==AYFX_MAX_EFFECTS) {
		
		ayFX_remove(AYFX_MAX_EFFECTS-1);
		idx--;
	}
	ayFX_status.nEffects++;
	
	ayFX_effect(idx)->afx = afx;
	ayFX_effect(idx)->priority = priority;
	ayFX_effect(idx)->adjustedVolume = adjustedVolume;
	
	while (idx>0 && ayFX_effect(idx-1)->priority <= ayFX_effect(idx)->priority) {
		swap(uint8_t,ayFX_status.sortedEffects[idx-1], ayFX_status.sortedEffects[idx]);
		idx--;
	}
}

void ayFX_afb(const uint8_t *afb, uint8_t idx, uint8_t priority, int8_t adjustedVolume) {
	
	const uint16_t *afxTable = (const uint16_t *)(afb+1);
	ayFX_afx((const uint8_t *)&afxTable[idx] + 2 + afxTable[idx], priority, adjustedVolume);
}

void ayFX_spin() {
	
	uint8_t idx = 0, nAppliedEffects = 0;
	
	uint8_t channel = ayFX_status.channel;
	bool noiseUpdated = false;
	
	if (!channel) ayFX_status.channel = 3;
	ayFX_status.channel--;

	while (idx<ayFX_status.nEffects) {		
		
		T_ayFX_Effect *effect = ayFX_effect(idx++);
		int8_t volume;
		T_ayFX_Flags flags;
		flags.value = *effect->afx++;
		
		if (flags.value==0xD0 && *effect->afx==0x20) {
			ayFX_remove(idx);
			idx--;
			continue;
		}
		
		if (flags.changeT) {
			effect->tone = *(const uint16_t *)effect->afx;
			effect->afx += 2;
		}

		if (flags.changeN) {
			effect->noise = *effect->afx++;
		}
			
		volume = flags.volume;
		volume += effect->adjustedVolume;
		
		if (volume>15) volume=15;
		if (volume< 0) volume=0;
			
		if (volume==0) continue; //No need to write it.
		
		if (flags.disableN && flags.disableT) continue;
		if (noiseUpdated && flags.disableT) continue;

		if (nAppliedEffects++>2) continue;

		if (!channel) channel=3;
		channel--;

		AY_3_8910_Registers.amplitude[channel].volume = volume;		
		AY_3_8910_Registers.amplitude[channel].m = 0;		
		
		if (!noiseUpdated && !flags.disableN) {
			AY_3_8910_Registers.noise_period = effect->noise;
			noiseUpdated = true;
		}
		
		if (!flags.disableT) {
			AY_3_8910_Registers.tone[channel] = effect->tone;
		}
		
		AY_3_8910_Registers.enable.value |= (flags.value & 0x90) >> (4-channel);
	}
	
}


////////////////////////////////////////////////////////////////////////
// PT3 PLAYER



