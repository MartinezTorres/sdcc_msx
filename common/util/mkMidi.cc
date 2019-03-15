////////////////////////////////////////////////////////////////////////
// MIDI to register dumps
//
// Manuel Martinez (salutte@gmail.com)
//
// FLAGS: -std=c++14 -O3 -Wno-pedantic -Wno-missing-field-initializers -Wno-unused-variable -Wno-unused-but-set-variable

#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <list>

namespace { // MiniLog
	class Log {
		int level;
		std::stringstream * const sstr;
	public:
		static int &reportLevel() { static int rl = 0; return rl; };
		static int reportLevel(int l) { return reportLevel()=l; };
		Log (int level) : level(level), sstr(level>=reportLevel()?new std::stringstream():nullptr) {}
		~Log() {
			if (sstr) {
				if (level==0) std::cerr << "\x1b[34;1m";
				if (level==1 or level==-2) std::cerr << "\x1b[32;1m";
				if (level>=2 or level==-1) std::cerr << "\x1b[31;1m";
				std::cerr << "L" << level << " ";				

				std::cerr << sstr->str() << std::endl;
				std::cerr << "\x1b[0m";
				delete sstr;
			}
		}
		template<typename T> Log &operator<<(const T &v) { if (sstr) *sstr << v; return *this; }
		void operator<<(std::nullptr_t) {}
	};
}

//# Table built from the scummvm adlib emulator
// NOtes based on http://www.fit.vutbr.cz/~arnost/opl/opl3.html#chapter5-2
struct AdLibInstrument {

	std::string name;
	
	uint8_t modCharacteristic; // Unused
	uint8_t modScalingOutputLevel; // Unused
	uint8_t modAttackDecay; // Unused
	uint8_t modSustainRelease; // Unused
	uint8_t modWaveformSelect; // Unused


	union {
		uint8_t carCharacteristic;
		struct {
			uint8_t multi : 4; //Frequency Multiplication Factor (MULTI). Operator's frequency is set to (see registers A0, B0) F-Number * Factor.
			uint8_t KSR : 1; //Envelope scaling (KSR) on/off. When 1, higher notes are shorter than lower notes.
			uint8_t sustain : 1;
			uint8_t vibrato : 1;
			uint8_t tremolo : 1;
		};
	};
	
	union {
		uint8_t carScalingOutputLevel;
		struct {
			uint8_t output_level : 6; //Output Level. Attenuates the operator output level. 0 is the loudest, 3F is the softest. 
			uint8_t KSL : 2; //Key Scale Level. Attenuates output level towards higher pitch: KSL*1.5db/oct
		};
	};

	union {
		uint8_t carAttackDecay;
		struct {
			uint8_t decay_rate : 4; // Decay Rate. Determines the diminishing time for the sound. The higher the value, the shorter the decay.
			uint8_t attack_rate : 4; // Attack Rate. Determines the rising time for the sound. The higher the value, the faster the attack.
		};
	};

	union {
		uint8_t carSustainRelease;
		struct {
			uint8_t release_rate : 4; // Release Rate. Determines the rate at which the sound disappears after KEY-OFF. The higher the value, the shorter the release.
			uint8_t sustain_level : 4; // Sustain Level. Determines the point at which the sound ceases to decay and chages to a sound having a constant level. The sustain level is expressed as a fraction of the maximum level. 0 is the softest and F is the loudest sustain level. 
		};
	};

	uint8_t carWaveformSelect; // Unused

	uint8_t feedback; // Unused
	uint8_t duration; // AutoOff
};

// Times in ms, taken from DoomJS
static std::vector<double> attackTimeValuesTable = {
		1e5, 1e5, 1e5, 1e5,
		2826.24, 2252.80, 1884.16, 1597.44,
		1413.12, 1126.40, 942.08, 798.72,
		706.56, 563.20, 471.04, 399.36,
		353.28, 281.60, 235.52, 199.68,
		176.76, 140.80, 117.76, 99.84,
		88.32, 70.40, 58.88, 49.92,
		44.16, 35.20, 29.44, 24.96,
		22.08, 17.60, 14.72, 12.48,
		11.04, 8.80, 7.36, 6.24,
		5.52, 4.40, 3.68, 3.12,
		2.76, 2.20, 1.84, 1.56,
		1.40, 1.12, 0.92, 0.80,
		0.70, 0.56, 0.46, 0.42,
		0.38, 0.30, 0.24, 0.20,
		0.01, 0.01, 0.01, 0.01
};

static std::vector<double> decayAndReleaseTimeValuesTable = {
		1e6, 1e6, 1e6, 1e6,
		39280.64, 31416.32, 26173.44, 22446.08,
		19640.32, 15708.16, 13086.72, 11223.04,
		9820.16, 7854.08, 6543.36, 5611.52,
		4910.08, 3927.04, 3271.68, 2805.76,
		2455.04, 1936.52, 1635.84, 1402.88,
		1227.52, 981.76, 817.92, 701.44,
		613.76, 490.88, 488.96, 350.72,
		306.88, 245.44, 204.48, 175.36,
		153.44, 122.72, 102.24, 87.68,
		76.72, 61.36, 51.12, 43.84,
		38.36, 30.68, 25.56, 21.92,
		19.20, 15.36, 12.80, 10.96,
		9.60, 7.68, 6.40, 5.48,
		4.80, 3.84, 3.20, 2.74,
		2.40, 2.40, 2.40, 2.40
};

static const std::vector<AdLibInstrument> g_gmInstruments = {	
	{"Acoustic Grand Piano", 0xC2, 0xC5, 0x2B, 0x99, 0x58, 0xC2, 0x1F, 0x1E, 0xC8, 0x7C, 0x0A, 0x23 },
	{"Bright Acoustic Piano", 0x22, 0x53, 0x0E, 0x8A, 0x30, 0x14, 0x06, 0x1D, 0x7A, 0x5C, 0x06, 0x00 },
	{"Electric Grand Piano", 0x06, 0x00, 0x1C, 0x79, 0x40, 0x02, 0x00, 0x4B, 0x79, 0x58, 0x08, 0x00 },
	{"Honky-tonk Piano", 0xC2, 0x89, 0x2A, 0x89, 0x49, 0xC2, 0x16, 0x1C, 0xB8, 0x7C, 0x04, 0x23 },
	{"Electric Piano 1", 0xC2, 0x17, 0x3D, 0x6A, 0x00, 0xC4, 0x2E, 0x2D, 0xC9, 0x20, 0x00, 0x00 },
	{"Electric Piano 2", 0x06, 0x1E, 0x1C, 0x99, 0x00, 0x02, 0x3A, 0x4C, 0x79, 0x00, 0x0C, 0x00 },
	{"Harpsichord", 0x84, 0x40, 0x3B, 0x5A, 0x6F, 0x81, 0x0E, 0x3B, 0x5A, 0x7F, 0x0B, 0x00 },
	{"Clavi", 0x84, 0x40, 0x3B, 0x5A, 0x63, 0x81, 0x00, 0x3B, 0x5A, 0x7F, 0x01, 0x00 },
	{"Celesta", 0x8C, 0x80, 0x05, 0xEA, 0x59, 0x82, 0x0A, 0x3C, 0xAA, 0x64, 0x07, 0x00 },
	{"Glockenspiel", 0x85, 0x40, 0x0D, 0xEC, 0x71, 0x84, 0x58, 0x3E, 0xCB, 0x7C, 0x01, 0x00 },
	{"Music Box", 0x8A, 0xC0, 0x0C, 0xDC, 0x50, 0x88, 0x58, 0x3D, 0xDA, 0x7C, 0x01, 0x00 },
	{"Vibraphone", 0xC9, 0x40, 0x2B, 0x78, 0x42, 0xC2, 0x04, 0x4C, 0x8A, 0x7C, 0x00, 0x1A },
	{"Marimba", 0x2A, 0x0E, 0x17, 0x89, 0x28, 0x22, 0x0C, 0x1B, 0x09, 0x70, 0x0A, 0x00 },
	{"Xylophone", 0xE7, 0x9B, 0x08, 0x08, 0x26, 0xE2, 0x06, 0x0A, 0x08, 0x70, 0x0A, 0x00 },
	{"Tubular Bells", 0xC5, 0x05, 0x00, 0xFC, 0x40, 0x84, 0x00, 0x00, 0xDC, 0x50, 0x08, 0x00 },
	{"Dulcimer", 0x86, 0x40, 0x5D, 0x5A, 0x41, 0x81, 0x00, 0x0B, 0x5A, 0x7F, 0x00, 0x00 },
	{"Drawbar Organ", 0xED, 0x00, 0x7B, 0xC8, 0x40, 0xE1, 0x99, 0x4A, 0xE9, 0x7E, 0x07, 0x00 },
	{"Percussive Organ", 0xE8, 0x4F, 0x3A, 0xD7, 0x7C, 0xE2, 0x97, 0x49, 0xF9, 0x7D, 0x05, 0x00 },
	{"Rock Organ", 0xE1, 0x10, 0x2F, 0xF7, 0x7D, 0xF3, 0x45, 0x8F, 0xC7, 0x62, 0x07, 0x00 },
	{"Church Organ", 0x01, 0x8C, 0x9F, 0xDA, 0x70, 0xE4, 0x50, 0x9F, 0xDA, 0x6A, 0x09, 0x00 },
	{"Reed Organ", 0x08, 0xD5, 0x9D, 0xA5, 0x45, 0xE2, 0x3F, 0x9F, 0xD6, 0x49, 0x07, 0x00 },
	{"Accordion", 0xE5, 0x0F, 0x7D, 0xB8, 0x2E, 0xA2, 0x0F, 0x7C, 0xC7, 0x61, 0x04, 0x00 },
	{"Harmonica", 0xF2, 0x2A, 0x9F, 0xDB, 0x01, 0xE1, 0x04, 0x8F, 0xD7, 0x62, 0x0A, 0x00 },
	{"Tango Accordion", 0xE4, 0x88, 0x9C, 0x50, 0x64, 0xE2, 0x18, 0x70, 0xC4, 0x7C, 0x0B, 0x00 },
	{"Acoustic Guitar (nylon)", 0x02, 0xA3, 0x0D, 0xDA, 0x01, 0xC2, 0x35, 0x5D, 0x58, 0x00, 0x06, 0x18 },
	{"Acoustic Guitar (steel)", 0x42, 0x55, 0x3E, 0xEB, 0x24, 0xD4, 0x08, 0x0D, 0xA9, 0x71, 0x04, 0x18 },
	{"Electric Guitar (jazz)", 0xC2, 0x00, 0x2B, 0x17, 0x51, 0xC2, 0x1E, 0x4D, 0x97, 0x7C, 0x00, 0x19 },
	{"Electric Guitar (clean)", 0xC6, 0x01, 0x2D, 0xA7, 0x44, 0xC2, 0x06, 0x0E, 0xA7, 0x79, 0x06, 0x00 },
	{"Electric Guitar (muted)", 0xC2, 0x0C, 0x06, 0x06, 0x55, 0xC2, 0x3F, 0x09, 0x86, 0x7D, 0x0A, 0x0A },
	{"Overdriven Guitar", 0xC2, 0x2E, 0x4F, 0x77, 0x00, 0xC4, 0x08, 0x0E, 0x98, 0x59, 0x0A, 0x00 },
	{"Distortion Guitar", 0xC2, 0x30, 0x4F, 0xCA, 0x01, 0xC4, 0x0D, 0x0E, 0xB8, 0x7F, 0x08, 0x00 },
	{"Guitar harmonics", 0xC4, 0x29, 0x4F, 0xCA, 0x03, 0xC8, 0x0D, 0x0C, 0xB7, 0x7D, 0x00, 0x0B },
	{"Acoustic Bass", 0xC2, 0x40, 0x3C, 0x96, 0x58, 0xC4, 0xDE, 0x0E, 0xC7, 0x7C, 0x00, 0x20 },
	{"Electric Bass (finger)", 0x31, 0x13, 0x2D, 0xD7, 0x3C, 0xE2, 0x18, 0x2E, 0xB8, 0x7C, 0x08, 0x00 },
	{"Electric Bass (pick)", 0x22, 0x86, 0x0D, 0xD7, 0x50, 0xE4, 0x18, 0x5E, 0xB8, 0x7C, 0x06, 0x28 },
	{"Fretless Bass", 0xF2, 0x0A, 0x0D, 0xD7, 0x40, 0xE4, 0x1F, 0x5E, 0xB8, 0x7C, 0x0A, 0x00 },
	{"Slap Bass 1", 0xF2, 0x09, 0x4B, 0xD6, 0x48, 0xE4, 0x1F, 0x1C, 0xB8, 0x7C, 0x0A, 0x28 },
	{"Slap Bass 2", 0x62, 0x11, 0x0C, 0xE6, 0x3C, 0xE4, 0x1F, 0x0C, 0xC8, 0x7C, 0x0A, 0x00 },
	{"Synth Bass 1", 0xE2, 0x12, 0x3D, 0xE6, 0x34, 0xE4, 0x1F, 0x7D, 0xB8, 0x7C, 0x0A, 0x00 },
	{"Synth Bass 2", 0xE2, 0x13, 0x3D, 0xE6, 0x34, 0xE4, 0x1F, 0x5D, 0xB8, 0x7D, 0x08, 0x00 },
	{"Violin", 0xA2, 0x40, 0x5D, 0xBA, 0x3F, 0xE2, 0x00, 0x8F, 0xD8, 0x79, 0x00, 0x00 },
	{"Viola", 0xE2, 0x40, 0x3D, 0xDA, 0x3B, 0xE1, 0x00, 0x7E, 0xD8, 0x7A, 0x04, 0x00 },
	{"Cello", 0x62, 0x00, 0x6D, 0xFA, 0x5D, 0xE2, 0x00, 0x8F, 0xC8, 0x79, 0x04, 0x00 },
	{"Contrabass", 0xE1, 0x00, 0x4E, 0xDB, 0x4A, 0xE3, 0x18, 0x6F, 0xE9, 0x7E, 0x00, 0x00 },
	{"Tremolo Strings", 0xE1, 0x00, 0x4E, 0xDB, 0x66, 0xE2, 0x00, 0x7F, 0xE9, 0x7E, 0x06, 0x00 },
	{"Pizzicato Strings", 0x02, 0x0F, 0x66, 0xAA, 0x51, 0x02, 0x64, 0x29, 0xF9, 0x7C, 0x08, 0x04 },
	{"Orchestral Harp", 0x16, 0x4A, 0x04, 0xBA, 0x39, 0xC2, 0x58, 0x2D, 0xCA, 0x7C, 0x0A, 0x03 },
	{"Timpani", 0x02, 0x00, 0x01, 0x7A, 0x79, 0x02, 0x3F, 0x28, 0xEA, 0x7C, 0x08, 0x02 },
	{"String Ensemble 1", 0x62, 0x53, 0x9C, 0xBA, 0x31, 0x62, 0x5B, 0xAD, 0xC9, 0x55, 0x04, 0x00 },
	{"String Ensemble 2", 0xF2, 0x40, 0x6E, 0xDA, 0x49, 0xE2, 0x13, 0x8F, 0xF9, 0x7D, 0x08, 0x00 },
	{"SynthStrings 1", 0xE2, 0x40, 0x8F, 0xFA, 0x50, 0xF2, 0x04, 0x7F, 0xFA, 0x7D, 0x0A, 0x00 },
	{"SynthStrings 2", 0xE4, 0xA0, 0xCE, 0x5B, 0x02, 0xE2, 0x32, 0x7F, 0xFB, 0x3D, 0x04, 0x00 },
	{"Choir Aahs", 0xE6, 0x80, 0x9C, 0x99, 0x42, 0xE2, 0x04, 0x7D, 0x78, 0x60, 0x04, 0x00 },
	{"Voice Oohs", 0xEA, 0xA0, 0xAC, 0x67, 0x02, 0xE2, 0x00, 0x7C, 0x7A, 0x7C, 0x06, 0x00 },
	{"Synth Voice", 0xE7, 0x94, 0xAD, 0xB7, 0x03, 0xE2, 0x00, 0x7C, 0xBA, 0x7C, 0x00, 0x00 },
	{"Orchestra Hit", 0xC3, 0x3F, 0x4B, 0xE9, 0x7E, 0xC1, 0x3F, 0x9B, 0xF9, 0x7F, 0x0B, 0x06 },
	{"Trumpet", 0xB2, 0x20, 0xAD, 0xE9, 0x00, 0x62, 0x05, 0x8F, 0xC8, 0x68, 0x0E, 0x00 },
	{"Trombone", 0xF2, 0x00, 0x8F, 0xFB, 0x50, 0xF6, 0x47, 0x8F, 0xE9, 0x68, 0x08, 0x00 },
	{"Tuba", 0xF2, 0x00, 0xAF, 0x88, 0x58, 0xF2, 0x54, 0x6E, 0xC9, 0x7C, 0x0A, 0x00 },
	{"Muted Trumpet", 0xF2, 0x2A, 0x9F, 0x98, 0x01, 0xE2, 0x84, 0x4E, 0x78, 0x6C, 0x0E, 0x00 },
	{"French Horn", 0xE2, 0x02, 0x9F, 0xB8, 0x48, 0x22, 0x89, 0x9F, 0xE8, 0x7C, 0x00, 0x00 },
	{"Brass Section", 0xE2, 0x2A, 0x7F, 0xB8, 0x01, 0xE4, 0x00, 0x0D, 0xC5, 0x7C, 0x0C, 0x00 },
	{"SynthBrass 1", 0xE4, 0x28, 0x8E, 0xE8, 0x01, 0xF2, 0x00, 0x4D, 0xD6, 0x7D, 0x0C, 0x00 },
	{"SynthBrass 2", 0x62, 0x23, 0x8F, 0xEA, 0x00, 0xF2, 0x00, 0x5E, 0xD9, 0x7C, 0x0C, 0x00 },
	{"Soprano Sax", 0xB4, 0x26, 0x6E, 0x98, 0x01, 0x62, 0x00, 0x7D, 0xC8, 0x7D, 0x00, 0x00 },
	{"Alto Sax", 0xE2, 0x2E, 0x20, 0xD9, 0x01, 0xF2, 0x0F, 0x90, 0xF8, 0x78, 0x0E, 0x00 },
	{"Tenor Sax", 0xE4, 0x28, 0x7E, 0xF8, 0x01, 0xE2, 0x23, 0x8E, 0xE8, 0x7D, 0x08, 0x00 },
	{"Baritone Sax", 0xB8, 0x28, 0x9E, 0x98, 0x01, 0x62, 0x00, 0x3D, 0xC8, 0x7D, 0x08, 0x00 },
	{"Oboe", 0x62, 0x00, 0x8E, 0xC9, 0x3D, 0xE6, 0x00, 0x7E, 0xD8, 0x68, 0x0A, 0x00 },
	{"English Horn", 0xE2, 0x00, 0x5F, 0xF9, 0x48, 0xE6, 0x98, 0x8F, 0xF8, 0x7D, 0x08, 0x00 },
	{"Bassoon", 0x62, 0x0C, 0x6E, 0xD8, 0x3D, 0x2A, 0x06, 0x7D, 0xD8, 0x58, 0x04, 0x00 },
	{"Clarinet", 0xE4, 0x00, 0x7E, 0x89, 0x38, 0xE6, 0x84, 0x80, 0xF8, 0x68, 0x0C, 0x00 },
	{"Piccolo", 0xE4, 0x80, 0x6C, 0xD9, 0x30, 0xE2, 0x00, 0x8D, 0xC8, 0x7C, 0x00, 0x00 },
	{"Flute", 0xE2, 0x80, 0x88, 0x48, 0x40, 0xE2, 0x0A, 0x7D, 0xA8, 0x7C, 0x08, 0x00 },
	{"Recorder", 0xE4, 0x00, 0x77, 0xC5, 0x54, 0xE2, 0x00, 0x9E, 0xD7, 0x70, 0x06, 0x00 },
	{"Pan Flute", 0xE4, 0x80, 0x86, 0xB9, 0x64, 0xE2, 0x05, 0x9F, 0xD7, 0x78, 0x0A, 0x00 },
	{"Blown Bottle", 0xE2, 0x00, 0x68, 0x68, 0x56, 0xE2, 0x08, 0x9B, 0xB3, 0x7C, 0x08, 0x00 },
	{"Shakuhachi", 0xE4, 0x00, 0xA6, 0x87, 0x41, 0xE2, 0x0A, 0x7E, 0xC9, 0x7C, 0x06, 0x00 },
	{"Whistle", 0xE4, 0x80, 0x9A, 0xB8, 0x48, 0xE2, 0x00, 0x9E, 0xF9, 0x60, 0x09, 0x00 },
	{"Ocarina", 0xE2, 0x80, 0x8E, 0x64, 0x68, 0xE2, 0x28, 0x6F, 0x73, 0x7C, 0x01, 0x00 },
	{"Lead 1 (square)", 0xE8, 0x00, 0x7D, 0x99, 0x54, 0xE6, 0x80, 0x80, 0xF8, 0x7C, 0x0C, 0x00 },
	{"Lead 2 (sawtooth)", 0xE6, 0x00, 0x9F, 0xB9, 0x6D, 0xE1, 0x00, 0x8F, 0xC8, 0x7D, 0x02, 0x00 },
	{"Lead 3 (calliope)", 0xE4, 0x00, 0x09, 0x68, 0x4A, 0xE2, 0x2B, 0x9E, 0xF3, 0x7C, 0x0E, 0x00 },
	{"Lead 4 (chiff)", 0xC4, 0x00, 0x99, 0xE8, 0x3B, 0xE2, 0x25, 0x6F, 0x93, 0x7C, 0x0E, 0x00 },
	{"Lead 5 (charang)", 0xE6, 0x00, 0x6F, 0xDA, 0x69, 0xE2, 0x05, 0x2F, 0xD8, 0x6A, 0x08, 0x00 },
	{"Lead 6 (voice)", 0xEC, 0x60, 0x9D, 0xC7, 0x00, 0xE2, 0x21, 0x7F, 0xC9, 0x7C, 0x06, 0x00 },
	{"Lead 7 (fifths)", 0xE3, 0x00, 0x0F, 0xF7, 0x7D, 0xE1, 0x3F, 0x0F, 0xA7, 0x01, 0x0D, 0x00 },
	{"Lead 8 (bass + lead)", 0xE4, 0xA9, 0x0F, 0xA8, 0x02, 0xE2, 0x3C, 0x5F, 0xDA, 0x3C, 0x0E, 0x00 },
	{"Pad 1 (new age)", 0xE8, 0x40, 0x0D, 0x89, 0x7D, 0xE2, 0x17, 0x7E, 0xD9, 0x7C, 0x07, 0x00 },
	{"Pad 2 (warm)", 0xE1, 0x00, 0xDF, 0x8A, 0x56, 0xE2, 0x5E, 0xCF, 0xBA, 0x7E, 0x08, 0x00 },
	{"Pad 3 (polysynth)", 0xE2, 0x00, 0x0B, 0x68, 0x60, 0xE2, 0x01, 0x9E, 0xB8, 0x7C, 0x0A, 0x00 },
	{"Pad 4 (choir)", 0xEA, 0x00, 0xAE, 0xAB, 0x49, 0xE2, 0x00, 0xAE, 0xBA, 0x6C, 0x08, 0x00 },
	{"Pad 5 (bowed)", 0xEB, 0x80, 0x8C, 0xCB, 0x3A, 0xE2, 0x86, 0xAF, 0xCA, 0x7C, 0x08, 0x00 },
	{"Pad 6 (metallic)", 0xE5, 0x40, 0xDB, 0x3B, 0x3C, 0xE2, 0x80, 0xBE, 0xCA, 0x71, 0x00, 0x00 },
	{"Halo Pad", 0xE4, 0x00, 0x9E, 0xAA, 0x3D, 0xE1, 0x43, 0x0F, 0xBA, 0x7E, 0x04, 0x00 },
	{"Pad 8 (sweep)", 0xE7, 0x40, 0xEC, 0xCA, 0x44, 0xE2, 0x03, 0xBF, 0xBA, 0x66, 0x02, 0x00 },
	{"FX 1 (rain)", 0xEA, 0x00, 0x68, 0xB8, 0x48, 0xE2, 0x0A, 0x8E, 0xB8, 0x7C, 0x0C, 0x00 },
	{"FX 2 (soundtrack)", 0x61, 0x00, 0xBE, 0x99, 0x7E, 0xE3, 0x40, 0xCF, 0xCA, 0x7D, 0x09, 0x00 },
	{"FX 3 (crystal)", 0xCD, 0x00, 0x0B, 0x00, 0x48, 0xC2, 0x58, 0x0C, 0x00, 0x7C, 0x0C, 0x1C },
	{"FX 4 (atmosphere)", 0xE2, 0x00, 0x0E, 0x00, 0x52, 0xE2, 0x58, 0x5F, 0xD0, 0x7D, 0x08, 0x00 },
	{"FX 5 (brightness)", 0xCC, 0x00, 0x7D, 0xDA, 0x40, 0xC2, 0x00, 0x5E, 0x9B, 0x58, 0x0C, 0x00 },
	{"FX 6 (goblins)", 0xE9, 0xC0, 0xEE, 0xD8, 0x43, 0xE2, 0x05, 0xDD, 0xAA, 0x70, 0x06, 0x00 },
	{"FX 7 (echoes)", 0xDA, 0x00, 0x8F, 0xAC, 0x4A, 0x22, 0x05, 0x8D, 0x8A, 0x75, 0x02, 0x00 },
	{"FX 8 (sci-fi)", 0x62, 0x8A, 0xCB, 0x7A, 0x74, 0xE6, 0x56, 0xAF, 0xDB, 0x70, 0x02, 0x00 },
	{"Sitar", 0xC2, 0x41, 0xAC, 0x5B, 0x5B, 0xC2, 0x80, 0x0D, 0xCB, 0x7D, 0x0A, 0x12 },
	{"Banjo", 0x75, 0x00, 0x0E, 0xCB, 0x5A, 0xE2, 0x1E, 0x0A, 0xC9, 0x7D, 0x0A, 0x10 },
	{"Shamisen", 0x41, 0x00, 0x0E, 0xEA, 0x53, 0xC2, 0x00, 0x08, 0xCA, 0x7C, 0x08, 0x07 },
	{"Koto", 0xC1, 0x40, 0x0C, 0x59, 0x6A, 0xC2, 0x80, 0x3C, 0xAB, 0x7C, 0x08, 0x0D },
	{"Kalimba", 0x4B, 0x00, 0x0A, 0xF5, 0x61, 0xC2, 0x19, 0x0C, 0xE9, 0x7C, 0x08, 0x07 },
	{"Bag pipe", 0x62, 0x00, 0x7F, 0xD8, 0x54, 0xEA, 0x00, 0x8F, 0xD8, 0x7D, 0x0A, 0x00 },
	{"Fiddle", 0xE1, 0x00, 0x7F, 0xD9, 0x56, 0xE1, 0x00, 0x8F, 0xD8, 0x7E, 0x06, 0x00 },
	{"Shanai", 0xE1, 0x00, 0x7F, 0xD9, 0x56, 0xE1, 0x00, 0x8F, 0xD8, 0x7E, 0x06, 0x00 },
	{"Tinkle Bell", 0xCF, 0x40, 0x09, 0xEA, 0x54, 0xC4, 0x00, 0x0C, 0xDB, 0x64, 0x08, 0x02 },
	{"Agogo", 0xCF, 0x40, 0x0C, 0xAA, 0x54, 0xC4, 0x00, 0x18, 0xF9, 0x64, 0x0C, 0x02 },
	{"Steel Drums", 0xC9, 0x0E, 0x88, 0xD9, 0x3E, 0xC2, 0x08, 0x1A, 0xEA, 0x6C, 0x0C, 0x05 },
	{"Woodblock", 0x03, 0x00, 0x15, 0x00, 0x64, 0x02, 0x00, 0x08, 0x00, 0x7C, 0x09, 0x02 },
	{"Taiko Drum", 0x01, 0x00, 0x47, 0xD7, 0x6C, 0x01, 0x3F, 0x0C, 0xFB, 0x7C, 0x0A, 0x04 },
	{"Melodic Tom", 0x00, 0x00, 0x36, 0x67, 0x7C, 0x01, 0x3F, 0x0E, 0xFA, 0x7C, 0x00, 0x05 },
	{"Synth Drum", 0x02, 0x00, 0x36, 0x68, 0x7C, 0x01, 0x3F, 0x0E, 0xFA, 0x7C, 0x00, 0x05 },
	{"Reverse Cymbal", 0xCB, 0x00, 0xAF, 0x00, 0x7E, 0xC0, 0x00, 0xC0, 0x06, 0x7F, 0x0E, 0x0F },
	{"Guitar Fret Noise", 0x05, 0x0D, 0x80, 0xA6, 0x7F, 0x0B, 0x38, 0xA9, 0xD8, 0x00, 0x0E, 0x04 },
	{"Breath Noise", 0x0F, 0x00, 0x90, 0xFA, 0x68, 0x06, 0x00, 0xA7, 0x39, 0x54, 0x0E, 0x06 },
	{"Seashore", 0xC9, 0x15, 0xDD, 0xFF, 0x7C, 0x00, 0x00, 0xE7, 0xFC, 0x6C, 0x0E, 0x38 },
	{"Bird Tweet", 0x48, 0x3C, 0x30, 0xF6, 0x03, 0x0A, 0x38, 0x97, 0xE8, 0x00, 0x0E, 0x04 },
	{"Telephone Ring", 0x07, 0x80, 0x0B, 0xC8, 0x65, 0x02, 0x3F, 0x0C, 0xEA, 0x7C, 0x0F, 0x05 },
	{"Helicopter", 0x00, 0x21, 0x66, 0x40, 0x03, 0x00, 0x3F, 0x47, 0x00, 0x00, 0x0E, 0x02 },
	{"Applause", 0x08, 0x00, 0x0B, 0x3C, 0x7C, 0x08, 0x3F, 0x06, 0xF3, 0x00, 0x0E, 0x02 },
	{"Gunshot", 0x00, 0x3F, 0x4C, 0xFB, 0x00, 0x00, 0x3F, 0x0A, 0xE9, 0x7C, 0x0E, 0x05 }
};

static std::map<uint8_t, AdLibInstrument> g_gmPercussionInstruments = {
	{ 0x22, { "Acoustic Bass Drum",  0x1A, 0x3F, 0x15, 0x05, 0x7C, 0x02, 0x21, 0x2B, 0xE4, 0x7C, 0x0E, 0x06 } },
	{ 0x23, { "Bass Drum 1",         0x11, 0x12, 0x04, 0x07, 0x7C, 0x02, 0x23, 0x0B, 0xE5, 0x7C, 0x0E, 0x05 } },
	{ 0x24, { "Side Stick",          0x0A, 0x3F, 0x0B, 0x01, 0x7C, 0x1F, 0x1C, 0x46, 0xD0, 0x7C, 0x0E, 0x01 } },
	{ 0x25, { "Acoustic Snare",      0x00, 0x3F, 0x0F, 0x00, 0x7C, 0x10, 0x12, 0x07, 0x00, 0x7C, 0x0E, 0x02 } },
	{ 0x26, { "Hand Clap",           0x0F, 0x3F, 0x0B, 0x00, 0x7C, 0x1F, 0x0F, 0x19, 0xD0, 0x7C, 0x0E, 0x02 } },
	{ 0x27, { "Electric Snare",      0x00, 0x3F, 0x1F, 0x00, 0x7E, 0x1F, 0x16, 0x07, 0x00, 0x7C, 0x0E, 0x03 } },
	{ 0x28, { "Low Floor Tom",       0x12, 0x3F, 0x05, 0x06, 0x7C, 0x03, 0x1F, 0x4A, 0xD9, 0x7C, 0x0E, 0x03 } },
	{ 0x29, { "Closed Hi Hat",       0xCF, 0x7F, 0x08, 0xFF, 0x7E, 0x00, 0xC7, 0x2D, 0xF7, 0x73, 0x0E, 0x02 } },
	{ 0x2A, { "High Floor Tom",      0x12, 0x3F, 0x05, 0x06, 0x7C, 0x43, 0x21, 0x0C, 0xE9, 0x7C, 0x0E, 0x03 } },
	{ 0x2B, { "Pedal Hi-Hat",        0xCF, 0x7F, 0x08, 0xCF, 0x7E, 0x00, 0x45, 0x2A, 0xF8, 0x4B, 0x0E, 0x0C } },
	{ 0x2C, { "Low Tom",             0x12, 0x3F, 0x06, 0x17, 0x7C, 0x03, 0x27, 0x0B, 0xE9, 0x7C, 0x0E, 0x03 } },
	{ 0x2D, { "Open Hi-Hat",         0xCF, 0x7F, 0x08, 0xCD, 0x7E, 0x00, 0x40, 0x1A, 0x69, 0x63, 0x0E, 0x0C } },
	{ 0x2E, { "Low-Mid Tom",         0x13, 0x3F, 0x05, 0x06, 0x7C, 0x03, 0x17, 0x0A, 0xD9, 0x7C, 0x0E, 0x03 } },
	{ 0x2F, { "Hi-Mid Tom",          0x15, 0x3F, 0x05, 0x06, 0x7C, 0x03, 0x21, 0x0C, 0xE9, 0x7C, 0x0E, 0x03 } },
	{ 0x30, { "Crash Cymbal 1",      0xCF, 0x3F, 0x2B, 0xFB, 0x7E, 0xC0, 0x1E, 0x1A, 0xCA, 0x7F, 0x0E, 0x10 } },
	{ 0x31, { "High Tom",            0x17, 0x3F, 0x04, 0x09, 0x7C, 0x03, 0x22, 0x0D, 0xE9, 0x7C, 0x0E, 0x03 } },
	{ 0x32, { "Ride Cymbal 1",       0xCF, 0x3F, 0x0F, 0x5E, 0x7C, 0xC6, 0x13, 0x00, 0xCA, 0x7F, 0x04, 0x03 } },
	{ 0x33, { "Chinese Cymbal",      0xCF, 0x3F, 0x7E, 0x9D, 0x7C, 0xC8, 0xC0, 0x0A, 0xBA, 0x74, 0x0E, 0x06 } },
	{ 0x34, { "Ride Bell",           0xCF, 0x3F, 0x4D, 0x9F, 0x7C, 0xC6, 0x00, 0x08, 0xDA, 0x5B, 0x04, 0x04 } },
	{ 0x35, { "Tambourine",          0xCF, 0x3F, 0x5D, 0xAA, 0x7A, 0xC0, 0xA4, 0x67, 0x99, 0x7C, 0x0A, 0x02 } },
	{ 0x36, { "Splash Cymbal",       0xCF, 0x3F, 0x4A, 0xFD, 0x7C, 0xCF, 0x00, 0x59, 0xEA, 0x7C, 0x0E, 0x02 } },
	{ 0x37, { "Cowbell",             0x0F, 0x18, 0x0A, 0xFA, 0x57, 0x06, 0x07, 0x06, 0x39, 0x7C, 0x0A, 0x02 } },
	{ 0x38, { "Crash Cymbal 2",      0xCF, 0x3F, 0x2B, 0xFC, 0x7C, 0xCC, 0xC6, 0x0B, 0xEA, 0x7F, 0x0E, 0x10 } },
	{ 0x39, { "Vibrasla",            } },
	{ 0x3A, { "Ride Cymbal 2",       } },
	{ 0x3B, { "Hi Bongo",            0x05, 0x1A, 0x04, 0x00, 0x7C, 0x12, 0x10, 0x0C, 0xEA, 0x7C, 0x0E, 0x07 } },
	{ 0x3C, { "Low Bongo",           0x04, 0x19, 0x04, 0x00, 0x7C, 0x12, 0x10, 0x2C, 0xEA, 0x7C, 0x0E, 0x04 } },
	{ 0x3D, { "Mute Hi Conga",       0x04, 0x0A, 0x04, 0x00, 0x6C, 0x01, 0x07, 0x0D, 0xFA, 0x74, 0x0E, 0x07 } },
	{ 0x3E, { "Open Hi Conga",       0x15, 0x14, 0x05, 0x00, 0x7D, 0x01, 0x07, 0x5C, 0xE9, 0x7C, 0x0E, 0x05 } },
	{ 0x3F, { "Low Conga",           0x10, 0x10, 0x05, 0x08, 0x7C, 0x01, 0x08, 0x0D, 0xEA, 0x7C, 0x0E, 0x05 } },
	{ 0x40, { "High Timbale",        0x11, 0x00, 0x06, 0x87, 0x7F, 0x02, 0x40, 0x09, 0x59, 0x68, 0x0E, 0x08 } },
	{ 0x41, { "Low Timbale",         0x13, 0x26, 0x04, 0x6A, 0x7F, 0x01, 0x00, 0x08, 0x5A, 0x7C, 0x0E, 0x08 } },
	{ 0x42, { "High Agogo",          0xCF, 0x4E, 0x0C, 0xAA, 0x50, 0xC4, 0x00, 0x18, 0xF9, 0x54, 0x04, 0x02 } },
	{ 0x43, { "Low Agogo",           0xCF, 0x4E, 0x0C, 0xAA, 0x50, 0xC3, 0x00, 0x18, 0xF8, 0x54, 0x04, 0x02 } },
	{ 0x44, { "Cabasa",              0xCB, 0x3F, 0x8F, 0x00, 0x7E, 0xC5, 0x00, 0x98, 0xD6, 0x5F, 0x0E, 0x0D } },
	{ 0x45, { "Maracas",             } },
	{ 0x46, { "Short Whistle",       } },
	{ 0x47, { "Long Whistle",        } },
	{ 0x48, { "Short Guiro",         } },
	{ 0x49, { "Long Guiro",          } },
	{ 0x4A, { "Claves",              0x0C, 0x18, 0x87, 0xB3, 0x7F, 0x19, 0x10, 0x55, 0x75, 0x7C, 0x0E, 0x02 } },
	{ 0x4B, { "Hi Wood Block",       0x05, 0x11, 0x15, 0x00, 0x64, 0x02, 0x08, 0x08, 0x00, 0x5C, 0x09, 0x02 } },
	{ 0x4C, { "Low Wood Block",      0x04, 0x08, 0x15, 0x00, 0x48, 0x01, 0x08, 0x08, 0x00, 0x60, 0x08, 0x02 } },
	{ 0x4D, { "Mute Cuica",          } },
	{ 0x4E, { "Open Cuica",          } },
	{ 0x4F, { "Mute Triangle",       0xDA, 0x00, 0x53, 0x30, 0x68, 0x07, 0x1E, 0x49, 0xC4, 0x7E, 0x03, 0x00 } },
	{ 0x50, { "Open Triangle",       0x1C, 0x00, 0x07, 0xBC, 0x6C, 0x0C, 0x14, 0x0B, 0x6A, 0x7E, 0x0B, 0x03 } },
	{ 0x51, { "Shaker",              } },
	{ 0x52, { "Jingle Bell",         } },
	{ 0x53, { "Belltree",            } },
	{ 0x54, { "Castanets",           0x0A, 0x0E, 0x7F, 0x00, 0x7D, 0x13, 0x20, 0x28, 0x03, 0x7C, 0x06, 0x00 } },
};
	
struct MidiFile {

	// Header info:
	int format, ntracks, division;
	int tempo = 500000;
	int ts_numerator=4, ts_denominator=4, ts_midiClocks=24, ts_notated32=8;
	int ks_sf=0, ks_mi=0;
	
	
	static size_t readmsb(std::istream &in, size_t sz) {

		size_t ret = 0;
		while (sz--) ret = (ret<<8) + in.get();
		return ret;
	}

	static size_t readvlv(std::istream &in) {
		
		size_t ret = 0, v;
		do {
			v = in.get();
			ret = (ret<<7) + (v&0x7F);
		} while (in and (v&0x80));
		return ret;
	}
	
	static std::string hex(int i) {
		
		std::ostringstream oss;
		oss << std::hex << "0x" << i;
		return oss.str();
	}
	
	enum EnvelopeStatus { ES_ATTACK, ES_DECAY, ES_SUSTAIN, ES_RELEASE, ES_OFF };

	struct Track;
	struct NoteEvent {
		
		Track &track;
		int channel;
		int tone;
		int velocity;
		
		NoteEvent(Track &track, int channel, int tone, int velocity) :
			track(track), channel(channel), tone(tone), velocity(velocity) {}
		
		bool pressed = true;
		
		EnvelopeStatus envelopeStatus = ES_ATTACK; // ADSRD (attack, decay, sustain, release, dead
		double envelopeVolume = -16;
		
		double toneVolume = 16+velocity/8.; // In AY-3-8910 scale, from 0 quietest, to 15 loudest. 
		double instrumentVolume = 0;
		double controllerVolume = 0;

		double priority = tone/12.; // Higher value means higher priority
		
		double volume() const { return toneVolume + instrumentVolume + controllerVolume + envelopeVolume; }

		bool operator<(const NoteEvent &rhs) const {
			return (0.5*rhs.priority + rhs.volume()) < (0.5*priority + volume());
		};
		
		int percussionInstrument=0;
		double percussionDuration;
	};

	struct Track {
		
		MidiFile &midiFile;
		
		std::istringstream in;

		std::map<int, std::map<int, int> > controller; // controller[channel][register] = value;
		std::map<int, AdLibInstrument> program;
		uint8_t status_byte;
		bool active = true;
		int time = 0;
		
		
		Track(MidiFile &midiFile, std::string data) :
			midiFile(midiFile),
			in(data)
		{
			
			for (int i=0; i<16; i++)
				program[i] = g_gmInstruments[0];	
			
			time = readvlv(in);
		}
		

		bool process_event(int current_time, std::list<NoteEvent> &notes) {
			
			if (not active) return false;
			
			if (time > current_time) return false;
			
			uint8_t b = in.get();
			if (b>=0x80) {
				status_byte = b;				
				b = in.get();
			}
			
			if        (status_byte>>5 == 0x04) { // Note off/on;
				int channel = status_byte & 0xF;
				int kk = b;
				int vv = in.get();
				
				if (vv == 0 or ((status_byte>>4) == 0x08)) { // note off
					if (channel!=9)
						for (auto &ne : notes)
							if (&ne.track == this and ne.channel == channel and ne.tone == kk)
								ne.pressed = false;
					
				} else { // Note on
					notes.emplace_back(*this, channel, kk, vv); 
				}
			} else if (status_byte>>4 == 0x0A) { // Polyphonic Key Pressure;
				Log(0) << "Polyphonic Key Pressure" ;
				int channel = status_byte & 0xF;
				int kk = b;
				int ww = in.get();
				
			} else if (status_byte>>4 == 0x0B) { // Controller Change;
				int channel = status_byte & 0xF;
				int cc = b;
				int nn = in.get();
				//Log(0) << "Controller Change: " << channel << " " << std::dec << cc << std::hex << " 0x" << nn ;
				controller[channel][b] = nn;
				
				if (b==121) controller[channel].clear();
				
				
			} else if (status_byte>>4 == 0x0C) { // Program Change;
				int channel = status_byte & 0xF;
				int pp = b;
				Log(0) << "Program Change: " << hex(channel) << " to " << g_gmInstruments[pp].name ;
				program[channel] = g_gmInstruments[pp];
				
			} else if (status_byte>>4 == 0x0D) { // Channel Key Pressure;
				int channel = status_byte & 0xF;
				Log(0) << "Channel Key Pressure" ;
				int ww = b;
			} else if (status_byte>>4 == 0x0E) { // Pitch Bend;
				int channel = status_byte & 0xF;
				Log(0) << "Pitch Bend" ;
				int lsb = b;
				int msb = in.get();
			} else if (status_byte == 0xF0) { // Sysex Events;
				Log(0) << "Sysex Event" ;
				std::string sysexData; 
				sysexData.resize(readvlv(in));
				in.read(&sysexData[0], sysexData.size());
			} else if (status_byte == 0xF7) { // Sysex Events;
				Log(0) << "Sysex Event" ;
				std::string sysexData; 
				sysexData.resize(readvlv(in));
				in.read(&sysexData[0], sysexData.size());
			} else if (status_byte == 0xFF) { // Meta Events;
				int meta_type = b;
				std::string metaData; 
				metaData.resize(readvlv(in));
				in.read(&metaData[0], metaData.size());
				Log(0) << "Meta Event of type: " << hex(meta_type) << " (" << hex(metaData.size()) << ") ";
				
				std::istringstream metaDataStream(metaData);
				
				switch (meta_type) {
				case 0x01: // Text event
				case 0x02: // Copyright Notice
				case 0x03: // Sequence Name
				case 0x04: // Instrument Name
				case 0x05: // Lyric
				case 0x06: // Marker
				case 0x07: // Cue Point
					Log(0) << metaData;
					break;

				case 0x21: // Tempo (microseconds per quarter note)
					break;
					
				case 0x51: // Tempo (microseconds per quarter note)
					midiFile.tempo = readmsb(metaDataStream,3);
					Log(0) << "Tempo " << midiFile.tempo;
					break;
				case 0x58: // Time Signature
					midiFile.ts_numerator = metaDataStream.get();
					midiFile.ts_denominator = metaDataStream.get();
					midiFile.ts_midiClocks = metaDataStream.get();
					midiFile.ts_notated32 = metaDataStream.get();
					//Log(0) << "TS: " << std::dec << ts_numerator << " " << ts_denominator << " " << ts_midiClocks << " " << ts_notated32 << std::hex;
					break;
				case 0x59: // Key Signature
					midiFile.ks_sf = metaDataStream.get();
					midiFile.ks_mi = metaDataStream.get();
					//Log(0) << "KS: " << std::dec << ks_sf << " " << ks_mi << std::hex;
					break;
			
				case 0x2f: // Finish track
					
					midiFile.ntracks--;
					Log(0) ;
					return active = false;

				default:
					break;
				}
				Log(0) ;
				
			} else {
				Log(0) << "Something got wrong" ;
			}
		
			time += readvlv(in);
			
			return active = not not in;
		}

	};

	std::vector<Track> tracks;
		
	MidiFile(std::istream &&iff) {
		
		while (true) {

			std::string type;
			std::string chunk;
			
			try {
				type = "    ";
				iff.read(&type[0],4);
				size_t length = readmsb(iff, 4);
				chunk.resize(length);
				iff.read(&chunk[0],length);
			} catch (std::exception &e) {}
			
			if (chunk.size()==0) return;
			
			Log(0) << "Read chunk: " << type << " of size: " << hex(chunk.size()) ;

			
			if (type=="MThd") {

				std::istringstream in(chunk);
				
				format = readmsb(in,2);
				ntracks = readmsb(in,2);
				division = readmsb(in,2);

				Log(0) << "Header: format: " << format << " Tracks: " << ntracks << " division: " << division ;

				if (format>1) throw std::runtime_error("Only format 0 and 1 midis are supported");
				if (division&0x8000) throw std::runtime_error("Only format 0 for the division field is supported");
				
			} else if (type=="MTrk") {
				
				Log(0)  << "New Track" ;
				tracks.emplace_back(*this, chunk);
			}
		}
	}	


	struct FrameActuation {
		
		double amplitude = 0;
		double priority = 0;
		
		bool operator<(FrameActuation &rhs) {
			return priority>rhs.priority;
		}
	};
	

	std::list<MidiFile::NoteEvent> notes; // Live note events.
	double time = 0;
	std::map<int, FrameActuation> calculateFrame(double fps) {
	
		std::map<int, FrameActuation> frame;
	
		double timeToSeconds =  tempo * 1e-6 / division;
		double timeToFrames =  fps * timeToSeconds;
		
		time += 1 / timeToFrames;
		
		for (auto &track : tracks)
			while (track.process_event(time, notes));

		for (auto &ne: notes) {
				
			if (ne.track.controller[ne.channel].count(7)) 
				ne.controllerVolume = (ne.track.controller[ne.channel][7]-127)/16.;
				

			if (ne.channel==9 and not ne.percussionInstrument) { // To be improved
				ne.percussionInstrument = ne.tone;
				ne.tone = 0;
			}

			AdLibInstrument &instrument = 
				(ne.channel!=9 ? 
					ne.track.program[ne.channel]:
					g_gmPercussionInstruments[ne.percussionInstrument]);
					
			if (ne.channel==9) { // To be improved
				
				if (ne.percussionDuration==0)
					ne.percussionDuration = instrument.duration;
				
				ne.percussionDuration--;
				if (ne.percussionDuration==0)
					ne.pressed = false;
					
				//ne.pressed = false;
			}
			
			

				
			if (instrument.name.empty()) {
				
				Log(0) << "Instrument not supported " << ne.channel << " " ;
				ne.envelopeStatus = ES_OFF;
				continue;
			}
			
			ne.instrumentVolume = -12.*instrument.output_level/double(0x3F);
			
			//ne.instrumentVolume = 0 ;

//			Log(0) << "ne.instrumentVolume " << ne.instrumentVolume ;
			
			////////////////////////////////////////////
			// Let's calculate the envelop.
			
			int rof = std::min(instrument.KSR?ne.tone>>3:ne.tone>>5,15);
			
			double attack_rate  = 1000*16./fps/attackTimeValuesTable[(15-instrument.attack_rate)*4+rof];
			double decay_rate   = 1000*16./fps/decayAndReleaseTimeValuesTable[(15-instrument.decay_rate)*4+rof];
			double release_rate = 1000*16./fps/decayAndReleaseTimeValuesTable[(15-instrument.release_rate)*4+rof];

			//Log(0) << "ne.attack_rate " << attack_rate << " " << instrument.name ;


			if (ne.envelopeStatus == ES_ATTACK) {
				
				ne.envelopeVolume += attack_rate + 0.1; 
				
			} else if (ne.envelopeStatus == ES_DECAY) {
				ne.envelopeVolume -= decay_rate + 0.1;
				
				if (ne.envelopeVolume < (instrument.sustain_level-15)) {
					if (instrument.sustain) {
						ne.envelopeVolume = instrument.sustain_level-15;
						ne.envelopeStatus = ES_SUSTAIN;
					} else {
						ne.envelopeStatus = ES_RELEASE;
					}
				}
				ne.priority -= 1;
				
			} else if (ne.envelopeStatus==ES_SUSTAIN) {
				
				ne.priority -= 1;
			} else if (ne.envelopeStatus==ES_RELEASE) {
				
				ne.envelopeVolume -= release_rate + 1;
				ne.priority -= 2;
			}

			if (ne.envelopeVolume >=  0) { ne.envelopeVolume = 0; ne.envelopeStatus = ES_DECAY; }
			if (not ne.pressed) ne.envelopeStatus = ES_RELEASE;
			if (ne.envelopeVolume < -16) ne.envelopeStatus = ES_OFF;
		}
		notes.remove_if([](const NoteEvent &ne){ return ne.envelopeStatus==ES_OFF; });
		notes.sort();
		
		for (auto &ne : notes) {

			if (std::min(15.,std::max(0.,std::round(ne.volume())))==0) continue;

			if (frame.size()>2 and frame.count(ne.tone)==0) continue;

			frame[ne.tone].amplitude = std::max(frame[ne.tone].amplitude,std::min(15.,std::max(0.,std::round(ne.volume()))));
			frame[ne.tone].priority = std::max(frame[ne.tone].priority, ne.priority);
			//ne.priority -= 0.5;
		}
	
		return frame;
	}


	operator bool() const {
		
		bool active = false;
		for (auto &track : tracks)
			active = active or track.active;
		return active;
	}
};


inline std::vector<uint8_t> compress(std::vector<uint8_t> data) {
	
	auto vec = [&](int i, int j){ return std::vector<uint8_t>(&data[i],&data[j]); };
	
	std::vector<uint8_t> out;
	
	int i=0;
	while (i<int(data.size()-1)) {
		
		int l=std::min(15,int(data.size()-i-1)), k=0;
		
		while (l>0) {
			k=std::min(15,i);
			while (k>0) {
				//Log(7) << l << " " << vec(i-k,i-k+l).size() << " " << vec(i,i+l).size();
				if (vec(i-k,i-k+l) == vec(i,i+l)) 
					goto out;

				k--;
			}
			l--;
		}
out:
		if (l==0) k=0;
		out.push_back(l*16+k);
		//Log(6) << l << " " << k << " 0x" << std::hex << int(data[i]) <<  std::dec;
		i += l;
		out.push_back(data[i++]);
	}
	return out;
}

int main(int argc, char *argv[]) {


	Log::reportLevel(5);
	
	std::map<std::string,double> options;
	options["-fps"] = 60;
	std::string sourceMidi, targetC;
	bool targetAudio;
	{
		for (int i=1; i<argc; i++) {

			std::string s(argv[i]);
			if (s.find("-")==0 and s.find("=")!=s.npos) {

				options[s.substr(0,s.find("="))] = atof(s.substr(s.find("=")+1).c_str());
			} else if (s.find("-")==0) {

				options[s] = 1;
			} else if (s.size()-s.find(".mid") == 4) {
				sourceMidi = s;
			} else if (s.size()-s.find(".c") == 2) {
				targetC = s;
			} else {
				Log(0) << "Unrecognized option: " << s ;
				return -1;
			}
		}
	}
	
	for (auto &f : options) Log(0) << f.first << " " << f.second ;
	if (sourceMidi.empty()) return -1;
	if (targetC.empty() and not options["audio"]) return -1;
	
	double fps = options["-fps"];
	
	MidiFile midiFile = MidiFile(std::ifstream(sourceMidi));
	
	// frames[frameIndex][tone].{amplitude|priority}
	// a note value of 0 means Noise (used for percussion, maybe)
	//std::map<int, std::map<int, MidiFile::FrameActuation> > frames = midiFile.calculateFrames(50);

	struct TNote { int note=0, duration=0, volume=0; };

	std::map<int, std::map<int, TNote>> noteMap;
	noteMap[0][0];
	noteMap[1][0];
	noteMap[2][0];


	if (false) {
		std::vector<uint8_t> yayTone = {0,0,0};
		std::vector<uint8_t> yayVolume = {100,100,100};
		int yayCount = 0;
			
		int count = 0;
		int frameN = 0;
		while (midiFile) {
			
			if ((++frameN & 0xFF) == 0) Log(0) << frameN ;	
				
			auto F = midiFile.calculateFrame(options["-fps"]);
								
			if (F.size()<=3) {

				std::set<uint8_t> freeChannels = {0,1,2};
				std::vector<uint8_t> newTone = yayTone;
				std::vector<uint8_t> newVolume(3,0);
				
				std::map<int,int> oldToneMaps;
				for (size_t i=0; i<yayTone.size(); i++)
					oldToneMaps[yayTone[i]] = i;
				
				// First, try to reuse a channel
				std::vector<std::pair<int,int> > pending;
				for (auto &&fa: F) {
					int vol = std::max(0,std::min(127,int(127-fa.second.amplitude)))>>3;
					if (oldToneMaps.count(fa.first)) {
						newVolume[oldToneMaps[fa.first]] = vol;
						if (vol>0) freeChannels.erase(oldToneMaps[fa.first]);
					} else if (vol) {
						pending.emplace_back(fa.first, vol);
					}
				}
				for (auto &p : pending) {
					int channel = *freeChannels.begin();
					freeChannels.erase(channel);
					
					newTone[channel]   = p.first;
					newVolume[channel] = p.second;
				}
				
				
					
				if (newTone!=yayTone or newVolume!=yayVolume) {
					
					while (yayCount) {
						
						//if (yayCount > 5) Log(0) << "yay:" << yayCount ;
						yayCount--;
						
						uint16_t miniHeader = 0;
						miniHeader = (miniHeader<<1) + (!!yayCount);
						miniHeader = (miniHeader<<1) + (newTone[0]!=yayTone[0]);
						miniHeader = (miniHeader<<1) + (newTone[1]!=yayTone[1]);
						miniHeader = (miniHeader<<1) + (newTone[2]!=yayTone[2]);
						miniHeader = (miniHeader<<4) + newVolume[0];
						miniHeader = (miniHeader<<4) + newVolume[1];
						miniHeader = (miniHeader<<4) + newVolume[2];
						
						//ossyay.put(miniHeader&0xFF);
						//ossyay.put((miniHeader>>8)&0xFF);
						
						if (!!yayCount) {
							
							//ossyay.put(std::min(256,yayCount)-1);
							yayCount-=std::min(256,yayCount);
						}
						
						//if (newTone[0]!=yayTone[0]) ossyay.put(newTone[0]);
						//if (newTone[0]!=yayTone[1]) ossyay.put(newTone[1]);
						//if (newTone[0]!=yayTone[2]) ossyay.put(newTone[2]);

						yayTone = newTone;
						yayVolume = newVolume;
					}
					
					yayTone = newTone;
					yayVolume = newVolume;
					yayCount = 1;
				} else {
					yayCount++;
				}
			
				for (int j=0; j<3; j++) {
					if (noteMap[j].rbegin()->second.note == newTone[j] and
						noteMap[j].rbegin()->second.volume == newVolume[j] and
						noteMap[j].rbegin()->first + noteMap[j].rbegin()->second.duration == frameN) {
							
						noteMap[j].rbegin()->second.duration++;
						
					} else {
						
						noteMap[j][frameN].note = newTone[j];
						noteMap[j][frameN].volume = newVolume[j];
						noteMap[j][frameN].duration = 1;
					}
				}
			}		
		}		
	}

	{
		int nFrame = 0;
		while (midiFile) {

			auto F = midiFile.calculateFrame(options["-fps"]);

			auto F2 = F;
			for (auto &&fa: F) {
				int vol = std::max(0,std::min(15,int(fa.second.amplitude)));
				int note = fa.first;
				for (auto &&cn : noteMap) {
					if (cn.second.rbegin()->second.note == note) {
						if (cn.second.rbegin()->second.volume == vol) {
							cn.second.rbegin()->second.duration++;
						} else {
							cn.second[nFrame].duration=1;
							cn.second[nFrame].volume=vol;
							cn.second[nFrame].note=note;							
						}
						F2.erase(note);
						break;
					}
				}
			}
			
			for (auto &&cn : noteMap) {
				if (cn.second.rbegin()->first + cn.second.rbegin()->second.duration != nFrame + 1) {
					if (cn.second.rbegin()->second.volume == 0) {
						cn.second.rbegin()->second.duration++;
					} else {
						cn.second[nFrame].duration=1;
						cn.second[nFrame].volume=0;
						cn.second[nFrame].note=cn.second.rbegin()->second.note;
					}
				}
			}
			
			for (auto &&fa : F2) {
				int vol = std::max(0,std::min(15,int(fa.second.amplitude)));
				int note = fa.first;
				for (auto &&cn : noteMap) {
					if (cn.second.rbegin()->second.volume == 0) {
						if (cn.second.rbegin()->first != nFrame) {
							cn.second.rbegin()->second.duration--;
						}
						cn.second[nFrame].duration=1;
						cn.second[nFrame].volume=vol;
						cn.second[nFrame].note=note;
						break;
					}
				}
			}

			nFrame++;		
		}
	}	
		
	if (targetC!="") {	
		
		std::vector<size_t> lastFrames;

		std::vector<std::vector<uint8_t>> data;
		for (auto &cm: noteMap) {
			
			data.emplace_back();
			
			int lastTime = cm.second.begin()->first;
			int lastNote = 0;
			int lastVolume = 0;
			int channel = cm.first;
			for (auto &fn: cm.second) {
				int time = fn.first;
				if (time!=lastTime) Log(0) << "Something's fishy" << time << " " << lastTime ;
				
				int note = fn.second.note;
				int volume = fn.second.volume;
				int duration = fn.second.duration;
				
				while (duration>0) {
					
					uint8_t out = 0; 
					if (note != lastNote) out += 0x80;
					out += std::min(7,duration)<<4;
					out += (volume & 0x0F);
					
					data.back().push_back(out);
					if (note != lastNote) data.back().push_back(note);
					if (std::min(7,duration)==7) data.back().push_back(std::min(255,duration));
					
					lastVolume = volume;
					lastNote = note;
					time += std::min(255,duration);
					duration -= std::min(255,duration);
				}
				lastTime = time;
			}
			
			lastFrames.push_back(lastTime);
			Log(0) << "LT: " << lastTime ;
		}
	
		for (auto &&d : data) 
			if (d.size()>2500) 
				d.resize(2500);
		std::ofstream oss(targetC);
		
		//std::string name = sourceMidi.substr(std::min(sourceMidi.rfind('/'),sourceMidi.rfind('\\'))+1);
		//name = "midi_"+name.substr(0,name.size()-4);

		std::string name = sourceMidi;
		name = name.substr(std::min(name.rfind('/'),name.rfind('\\'))+1);
		for (auto &&s : "_!-+/\\.'() ")
			for (auto &&c : name)
				if (c==s) c = '_';

		//for (auto &m : data) Log(5) << m.size() << " " << compress(m).size();

		oss << "// Music file generated from: " << name << ".mid" << std::endl;
		oss << "typedef unsigned char uint8_t;" << std::endl;
		for (size_t j=0; j<data.size(); j++) {
			auto &D = data[j];
			oss << "static const uint8_t data" << char('A'+j) << "[] = {\n\t";
			for (size_t i=0; i<D.size(); i++) {
				char buf[20];
				snprintf(buf,20,"0x%02X",D[i]);
				oss << buf << ((i+1)%16?", ":", \n\t");
			}
			oss << "0x00, 0x00, 0x00 };" << std::endl;
		}

		oss << "typedef struct { const uint8_t *channels[3]; } AYR;" << std::endl;
		oss << "const AYR " + name + " = { { &dataA[0], &dataB[0], &dataC[0]} };" << std::endl;
	}
	
	
/*	if (targetC!="") {	
		
		std::vector<size_t> lastFrames;

		std::vector<std::vector<uint8_t>> metas;
		std::vector<std::vector<uint8_t>> durations;
		std::vector<std::vector<uint8_t>> notes;
		for (auto &cm: noteMap) {
			
			metas.emplace_back();
			durations.emplace_back();
			notes.emplace_back();
			
			int lastTime = cm.second.begin()->first;
			int lastNote = 0;
			int lastVolume = 0;
			int channel = cm.first;
			for (auto &fn: cm.second) {
				int time = fn.first;
				if (time!=lastTime) Log(0) << "Something's fishy" << time << " " << lastTime ;
				
				int note = fn.second.note;
				int volume = fn.second.volume;
				int duration = fn.second.duration;
				
				while (duration>0) {
					
					uint8_t out = 0; 
					if (note != lastNote) out += 0x80;
					out += std::min(7,duration)<<4;
					out += (volume & 0x0F);
					
					metas.back().push_back(out);
					if (note != lastNote) notes.back().push_back(note);
					if (std::min(7,duration)==7) durations.back().push_back(std::min(255,duration));
					
					lastVolume = volume;
					lastNote = note;
					time += std::min(255,duration);
					duration -= std::min(255,duration);
				}
				lastTime = time;
			}
			
			lastFrames.push_back(lastTime);
			Log(0) << "LT: " << lastTime ;
		}
	
		
		std::string name = sourceMidi.substr(std::min(sourceMidi.rfind('/'),sourceMidi.rfind('\\'))+1);
		name = name.substr(0,name.size()-4);

		for (auto &m : metas) Log(5) << m.size() << " " << compress(m).size();
		for (auto &m : durations) Log(5) << m.size() << " " << compress(m).size();
		for (auto &m : notes) Log(5) << m.size() << " " << compress(m).size();

		std::ofstream oss(targetC);

		oss << "// Music file generated from: " << name << ".mid" << std::endl;
		oss << "#include <stdint.h>" << std::endl;
		for (size_t j=0; j<metas.size(); j++) {
			auto &D = metas[j];
			oss << "static const uint8_t meta" << char('A'+j) << "[] = {\n\t";
			for (size_t i=0; i<D.size(); i++) {
				char buf[20];
				snprintf(buf,20,"0x%02X",D[i]);
				oss << buf << ((i+1)%16?", ":", \n\t");
			}
			oss << "};" << std::endl;
		}

		for (size_t j=0; j<notes.size(); j++) {
			auto &D = notes[j];
			oss << "static const uint8_t note" << char('A'+j) << "[] = {\n\t";
			for (size_t i=0; i<D.size(); i++) {
				char buf[20];
				snprintf(buf,20,"0x%02X",D[i]);
				oss << buf << ((i+1)%16?", ":", \n\t");
			}
			oss << "};" << std::endl;
		}

		for (size_t j=0; j<durations.size(); j++) {
			auto &D = durations[j];
			oss << "static const uint8_t duration" << char('A'+j) << "[] = {\n\t";
			for (size_t i=0; i<D.size(); i++) {
				char buf[20];
				snprintf(buf,20,"0x%02X",D[i]);
				oss << buf << ((i+1)%16?", ":", \n\t");
			}
			oss << "};" << std::endl;
		}
		oss << "struct AYR { const uint8_t *metas[3], *notes[3], *durations[3]; uint16_t fames; };" << std::endl;
		oss << "const struct AYR " + name + " = { { &metaA[0], &metaB[0], &metaC[0]}, { &noteA[0], &noteB[0], &noteC[0]}, { &durationA[0], &durationB[0], &durationC[0]}, " << lastFrames.front() << "};" << std::endl;
	}*/
}
