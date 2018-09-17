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

#CCFLAGS_MSX = $(VERBOSE) -mz80 --code-loc $(ADDR_CODE) --code-size $(CODE_SIZE) --data-loc $(ADDR_DATA) --iram-size $(DATA_SIZE) --no-std-crt0 --out-fmt-ihx --opt-code-size
CCFLAGS_MSX = $(VERBOSE) -mz80 --code-loc $(ADDR_CODE) --code-size $(CODE_SIZE) --data-loc $(ADDR_DATA) --iram-size $(DATA_SIZE) --no-std-crt0 --out-fmt-ihx --opt-code-size --fomit-frame-pointer --max-allocs-per-node 10000 --allow-unsafe-read --nostdlib --no-xinit-opt
CCFLAGS_LINUX = -Wall -Wextra


HEADERS        = tmp/fonts.h
HEADERS_LINUX  = $(HEADERS) $(wildcard src/common/*.h) $(wildcard src/linux/*.h)
HEADERS_MSX    = $(HEADERS) $(wildcard src/common/*.h) $(wildcard src/msx/*.h) 

C_SOURCES_LINUX= $(wildcard src/common/*.c) $(wildcard src/linux/*.c)
C_SOURCES_MSX  = $(wildcard src/common/*.c) $(wildcard src/msx/*.c) 
ASM_SOURCES_MSX= $(wildcard src/msx/*.s)

ASM_OBJ_MSX    = $(patsubst src/%.s, tmp/%.rel, $(ASM_SOURCES_MSX))			
C_OBJ_MSX      = $(patsubst src/%.c, tmp/%.rel, $(C_SOURCES_MSX))			

.PHONY: all run msx1 clean
.PRECIOUS: tmp/%.ihx tmp/msx/%.rel tmp/common/%.rel tmp/*/*.rel

all: bin/$(NAME).rom bin/$(NAME).linux

tmp/msx/%.rel: src/msx/%.s $(HEADERS_MSX)
	@echo -n "Assembling $< -> $@ ... "
	@mkdir -p tmp/msx/ 
	@cd tmp && $(ASM) -o ../$@ ../$<  || true
	@echo "Done!"

tmp/msx/%.rel: src/msx/%.c $(HEADERS_MSX)
	@echo -n "Compiling $< -> $@ ... "
	@mkdir -p tmp/msx/
	@$(CCZ80) -c -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $< -o tmp/msx/
	@echo "Done!"

tmp/common/%.rel: src/common/%.c $(HEADERS_MSX)
	@echo -n "Compiling $< -> $@ ... "
	@mkdir -p tmp/common/
	@$(CCZ80) -c -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $< -o tmp/common/
	@echo "Done!"

tmp/%.lib: $(C_OBJ_MSX)
	@echo -n "Linking $(OBJ_MSX) -> $@ ... "
	@mkdir -p tmp
	sdcclib $@ $(C_OBJ_MSX)
	@echo "Done!"

tmp/%.ihx: $(ASM_OBJ_MSX) tmp/%.lib $(HEADERS_MSX)
	@echo -n "Linking $(ASM_OBJ_MSX) -> $@ ... "
	@mkdir -p tmp
	$(CCZ80) -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $(ASM_OBJ_MSX) tmp/skel.lib -o $@
	@echo "Done!"

bin/%.rom: tmp/%.ihx
	@echo -n "Building $< -> $@ ... "
	@$(HEXBIN) $< $(<:.ihx=.tmp)
	@dd skip=16384 count=32768 if=$(<:.ihx=.tmp) of=$@ bs=1 status=none
	@echo "Done!"
	@echo ROM used: $$((`grep -m1 "CODE .*\. bytes "  tmp/skel.map | cut -c 64-70 | xargs`*100/32768))% of 32K  $$((`grep -m1 "CODE .*\. bytes "  tmp/skel.map | cut -c 64-70 | xargs`))
	@echo RAM used: $$((`grep -m1 "DATA .*\. bytes "  tmp/skel.map | cut -c 64-70 | xargs`*100/16384))% of 16K

bin/%.linux:  $(C_SOURCES_LINUX) $(HEADERS_LINUX)
	$(CC) -g -D LINUX $(INCLUDES_LINUX) $(CCFLAGS_LINUX) $(C_SOURCES_LINUX) -lSDL2 -o $@ 


linux: bin/$(NAME).linux
	@./$<

msx1: bin/$(NAME).rom
	@$(MSX1) $< || true

# GENERATE UTILITIES
INCLUDE := util
CUSTOM_FLAGS = `grep -m1 "^// FLAGS:" util/$*.cc | cut -d: -f2-`
COMMON_FLAGS := -Werror -Wall -Wextra -pedantic -Winvalid-pch -Wformat=2 -Winit-self -Winline -Wpacked -Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations -Wmissing-format-attribute -Wmissing-noreturn -Wredundant-decls -Wsign-compare -Wstrict-aliasing=2 -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings -pipe $(patsubst %,-I%,$(INCLUDE))
ALL_INCLUDES := $(shell for d in $(INCLUDE); do find -L $$d -type f -name \*.h -print 2>/dev/null ; done) $(shell for d in $(INCLUDE); do find -L $$d -type f -name \*.hpp -print 2>/dev/null ; done)
.SECONDEXPANSION:

.PRECIOUS:bin/%
bin/%: util/%.cc $(ALL_INCLUDES) $$(DEPS)
	@mkdir -p $(@D)
	@echo "\033[1;32m[$(@F)]\033[1;31m\033[12GFLAGS:\033[0m$(CUSTOM_FLAGS)"
	@$(CXX) -o $@ $< $(shell echo $(COMMON_FLAGS) $(CUSTOM_FLAGS)) 

%: util/%.cc bin/% 
	@echo

# GENERATE FONTS
.PHONY: fonts
fonts: tmp/fonts.h
	@echo

tmp/fonts.h: bin/mkFont res/fonts/*
	@echo -n "Generating  $@ ... "
	rm -f $@
	mkdir -p tmp
	for f in res/fonts/*.png; do bin/mkFont $$f >> $@; done
	@echo "Done!"

clean:
	@echo -n "Cleaning... "
	@rm -rf tmp bin/*
	@echo "Done!"
