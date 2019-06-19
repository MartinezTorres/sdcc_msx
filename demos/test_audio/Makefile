NAME := test_audio

ADDR_CODE := 0x4020
CODE_SIZE := 0x7FDF
ADDR_DATA := 0xC000
DATA_SIZE := 0x3000

updateMidiHeader : res/*/*
	mkdir -p inc
	util/updateMidiSources.sh > inc/midiSources.h

include sdcc_msx/Makefile.in

