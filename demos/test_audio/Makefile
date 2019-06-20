NAME := test_audio

include sdcc_msx/Makefile.in

pre_target:: inc/midiSources.h

inc/midiSources.h: res/midi/*/*.mid
	mkdir -p inc
	util/updateMidiSources.sh > inc/midiSources.h
