NAME := test_audio


include sdcc_msx/Makefile.in

CCFLAGS_MSX   += -DASCII8

pre_target:: inc/midiSources.h

inc/midiSources.h: res/midi/*/*.mid
	mkdir -p inc
	util/updateMidiSources.sh > inc/midiSources.h

OPENMSX_BIN = /opt/openMSX/bin/openmsx
