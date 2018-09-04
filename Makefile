NAME := skel
ADDR_CODE = 0x4020
ADDR_DATA = 0xC000


CCZ80 = sdcc
ASM = sdasz80
MSX1 = openmsx -machine C-BIOS_MSX1 -carta
HEXBIN = makebin -s 65536

#VERBOSE = -V
CCZ80FLAGS += $(VERBOSE) -mz80 --code-loc $(ADDR_CODE) --data-loc $(ADDR_DATA) --no-std-crt0 --out-fmt-ihx --opt-code-size

LINUX_HEADERS  = $(wildcard src/common/*.h) $(wildcard src/linux/*.h)
MSX_HEADERS    = $(wildcard src/common/*.h) $(wildcard src/msx/*.h) 

LINUX_SRC_C  = $(wildcard src/common/*.c) $(wildcard src/linux/*.c)
MSX_SRC_C    = $(wildcard src/common/*.c) $(wildcard src/msx/*.c) 
MSX_SRC_ASM  = $(wildcard src/msx/*.s)

MSX_OBJ_ASM  = $(patsubst src/msx/%.s, tmp/%.rel, $(MSX_SRC_ASM))			


.PHONY: all run msx1 clean

all: bin/$(NAME).rom bin/$(NAME)

tmp/%.rel: src/msx/%.s 
	@echo -n "Assembling $< -> $@ ... "
	@mkdir -p tmp && cd tmp && $(ASM) -o ../$@ ../$<  || true
	@echo "Done!"

tmp/%.ihx: $(MSX_OBJ_ASM) $(MSX_SRC_C) $(MSX_HEADERS)
	@echo -n "Compiling $< -> $@ ... "
	@mkdir -p tmp && cd tmp && $(CCZ80) -D MSX -I../src/common -I../src/msx $(CCZ80FLAGS) $(addprefix ../,$<) $(addprefix ../,$(MSX_SRC_C)) -o $(notdir $@) || true
	@echo "Done!"

bin/%.rom: tmp/%.ihx
	@echo -n "Building $< -> $@ ... "
	@$(HEXBIN) $< $(<:.ihx=.tmp)
	@dd skip=16384 count=32768 if=$(<:.ihx=.tmp) of=$@ bs=1 status=none
	@echo "Done!"

bin/%.pc:  $(LINUX_SRC_C) $(LINUX_HEADERS)
	$(CC) -D PC -o $@ $(LINUX_SRC_C) -lSDL2 -Isrc/common -Isrc/linux -Wall -Wextra

run: bin/$(NAME)
	@./$<

msx1: bin/$(NAME).rom
	@$(MSX1) $<


clean:
	@echo -n "Cleaning... "
	@rm -rf tmp bin/*
	@echo "Done!"
