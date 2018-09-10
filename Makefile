NAME := skel
ADDR_CODE = 0x4020
CODE_SIZE = 0x7FDF
ADDR_DATA = 0xC000
DATA_SIZE = 0x3000


CCZ80 = sdcc
ASM = sdasz80
MSX1 = openmsx -machine C-BIOS_MSX1 -carta
HEXBIN = makebin -s 65536

#VERBOSE = -V
INCLUDES_MSX = -I. -Isrc/common -Isrc/msx
INCLUDES_LINUX = -I. -Isrc/common -Isrc/linux

CCFLAGS_MSX = $(VERBOSE) -mz80 --code-loc $(ADDR_CODE) --code-size $(CODE_SIZE) --data-loc $(ADDR_DATA) --iram-size $(DATA_SIZE) --no-std-crt0 --out-fmt-ihx --opt-code-size
CCFLAGS_LINUX = -Wall -Wextra

HEADERS_LINUX  = $(wildcard src/common/*.h) $(wildcard src/linux/*.h)
HEADERS_MSX    = $(wildcard src/common/*.h) $(wildcard src/msx/*.h) 

C_SOURCES_LINUX  = $(wildcard src/common/*.c) $(wildcard src/linux/*.c)
C_SOURCES_MSX    = $(wildcard src/common/*.c) $(wildcard src/msx/*.c) 
ASM_SOURCES_MSX  = $(wildcard src/msx/*.s)

OBJ_MSX      = $(patsubst src/%.c, tmp/%.rel, $(C_SOURCES_MSX))	$(patsubst src/%.s, tmp/%.rel, $(ASM_SOURCES_MSX))		


.PHONY: all run msx1 clean

all: bin/$(NAME).rom bin/$(NAME).linux

tmp/msx/%.rel: src/msx/%.s 
	@echo -n "Assembling $< -> $@ ... "
	@mkdir -p tmp && cd tmp && $(ASM) -o ../$@ ../$<  || true
	@echo "Done!"

tmp/msx/%.rel: src/msx/%.c 
	@echo -n "Compiling $< -> $@ ... "
	@mkdir -p tmp/msx/
	@$(CCZ80) -c -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $< -o tmp/msx/
	@echo "Done!"

tmp/common/%.rel: src/common/%.c 
	@echo -n "Compiling $< -> $@ ... "
	@mkdir -p tmp/common/
	@$(CCZ80) -c -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $< -o tmp/common/
	@echo "Done!"

tmp/%.ihx: $(OBJ_MSX) $(HEADERS_MSX)
	@echo -n "Compiling -> $@ ... "
	@mkdir -p tmp
	@$(CCZ80) -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $(OBJ_MSX) -o $@
	@echo "Done!"

bin/%.rom: tmp/%.ihx
	@echo -n "Building $< -> $@ ... "
	@$(HEXBIN) $< $(<:.ihx=.tmp)
	@dd skip=16384 count=32768 if=$(<:.ihx=.tmp) of=$@ bs=1 status=none
	@echo "Done!"

bin/%.linux:  $(C_SOURCES_LINUX) $(HEADERS_LINUX)
	$(CC) -g -D LINUX $(INCLUDES_LINUX) $(CCFLAGS_LINUX) $(C_SOURCES_LINUX) -lSDL2 -o $@ 

linux: bin/$(NAME).linux
	@./$<

msx1: bin/$(NAME).rom
	@$(MSX1) $<


clean:
	@echo -n "Cleaning... "
	@rm -rf tmp bin/*
	@echo "Done!"
