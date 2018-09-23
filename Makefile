NAME := skel
ADDR_CODE = 0x4020
CODE_SIZE = 0x7FDF
ADDR_DATA = 0xC000
DATA_SIZE = 0x3000


MSG = "\033[1;32m[$(@F)]\033[1;31m\033[0m"


CCZ80 = sdcc
ASM = sdasz80
MSX1 = openmsx -machine C-BIOS_MSX1 -carta
HEXBIN = makebin -s 65536

#VERBOSE = -V
INCLUDES_MSX = -Itmp/inc -Isrc/common -Isrc/msx
INCLUDES_LINUX = -Itmp/inc -Isrc/common -Isrc/linux

CCFLAGS_MSX = $(VERBOSE) -mz80 --code-loc $(ADDR_CODE) --code-size $(CODE_SIZE) --data-loc $(ADDR_DATA) --iram-size $(DATA_SIZE) --no-std-crt0 --out-fmt-ihx --opt-code-size --fomit-frame-pointer --max-allocs-per-node 10000 --allow-unsafe-read --nostdlib --no-xinit-opt
CCFLAGS_LINUX = -Wall -Wextra


HEADERS        = $(wildcard src/common/*.h) $(wildcard tmp/res/*.h) 
HEADERS_LINUX  = $(HEADERS) $(wildcard src/linux/*.h)
HEADERS_MSX    = $(HEADERS) $(wildcard src/msx/*.h) 

C_SOURCES      = $(wildcard src/common/*.c)
C_SOURCES_LINUX= $(C_SOURCES) $(wildcard src/linux/*.c)
C_SOURCES_MSX  = $(C_SOURCES) $(wildcard src/msx/*.c) 
ASM_SOURCES_MSX= $(wildcard src/msx/*.s)


.PHONY: all run msx1 clean

all: bin/$(NAME).rom bin/$(NAME).linux

##########################################################
### RESOURCE SECTION

# GENERATE FONTS
FONTS_SRC := $(wildcard res/fonts/*.png)
FONTS_GEN := $(patsubst res/fonts/%.png, tmp/res/%.c, $(FONTS_SRC))			
.PHONY: fonts
fonts: FONTS_GEN
	@true

tmp/res/%.c: res/fonts/%.png bin/mkFont
	@echo $(MSG)
	@mkdir -p $(@D)
	@bin/mkFont $< > $@


# COPY RESOURCE FILES
RESOURCES_SRC := $(wildcard res/*.c)
RESOURCES_GEN := $(patsubst res/%.c, tmp/res/%.c, $(RESOURCES_SRC))			

tmp/res/%.c: res/%.c
	@echo $(MSG)
	@mkdir -p $(@D)
	@cp $< $@

# COPY RESOURCE HEADER
RESOURCES = $(FONTS_GEN) $(RESOURCES_GEN)
tmp/inc/resources.h: $(RESOURCES)
	@echo $(MSG)
	@mkdir -p $(@D)
	@rm -f $@
	@for f in $^; do cproto -qve < $$f >> $@ ; done

C_SOURCES += $(RESOURCES)

##########################################################
### MSX SECTION
ASM_OBJ_MSX    = $(addprefix tmp/,$(ASM_SOURCES_MSX:.s=.rel))
C_OBJ_MSX      = $(addprefix tmp/,$(C_SOURCES_MSX:.c=.rel))

.PRECIOUS: tmp/%.ihx $(ASM_OBJ_MSX) $(C_OBJ_MSX) tmp/%.lib 

tmp/%.rel: %.s $(HEADERS_MSX) tmp/inc/resources.h
	@echo $(MSG)
	@mkdir -p $(@D)
	@cd tmp && $(ASM) -o ../$@ ../$<

tmp/%.rel: %.c $(HEADERS_MSX) tmp/inc/resources.h
	@echo $(MSG)
	@mkdir -p $(@D)
	@$(CCZ80) -c -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $< -o $@

tmp/%.lib: $(C_OBJ_MSX)
	@echo $(MSG)
	@mkdir -p $(@D)
	@sdcclib $@ $(C_OBJ_MSX)

tmp/%.ihx: $(ASM_OBJ_MSX) tmp/%.lib 
	@echo $(MSG)
	@mkdir -p tmp
	@$(CCZ80) -D MSX $(INCLUDES_MSX) $(CCFLAGS_MSX) $(ASM_OBJ_MSX) tmp/skel.lib -o $@

bin/%.rom: tmp/%.ihx
	@echo $(MSG)
	@$(HEXBIN) $< $(<:.ihx=.tmp)
	@dd skip=16384 count=32768 if=$(<:.ihx=.tmp) of=$@ bs=1 status=none
	@echo ROM used: $$((`grep -m1 "CODE .*\. bytes "  tmp/skel.map | cut -c 64-70 | xargs`*100/32768))% of 32K  $$((`grep -m1 "CODE .*\. bytes "  tmp/skel.map | cut -c 64-70 | xargs`))
	@echo RAM used: $$((`grep -m1 "DATA .*\. bytes "  tmp/skel.map | cut -c 64-70 | xargs`*100/16384))% of 16K

msx1: bin/$(NAME).rom
	@echo $(MSG)
	@$(MSX1) $< || true

##########################################################
### LINUX SECTION

bin/%.linux:  $(C_SOURCES_LINUX) $(HEADERS_LINUX) tmp/inc/resources.h
	@echo $(MSG)
	@mkdir -p $(@D)
	$(CC) -g -D LINUX $(INCLUDES_LINUX) $(CCFLAGS_LINUX) $(C_SOURCES_LINUX) -lSDL2 -o $@ 

linux: bin/$(NAME).linux
	@./$<


##########################################################
### UTILITIES SECTION

INCLUDE := util
CUSTOM_FLAGS = `grep -m1 "^// FLAGS:" util/$*.cc | cut -d: -f2-`
COMMON_FLAGS := -Werror -Wall -Wextra -pedantic -Winvalid-pch -Wformat=2 -Winit-self -Winline -Wpacked -Wpointer-arith -Wlarger-than-65500 -Wmissing-declarations -Wmissing-format-attribute -Wmissing-noreturn -Wredundant-decls -Wsign-compare -Wstrict-aliasing=2 -Wswitch-enum -Wundef -Wunreachable-code -Wwrite-strings -pipe $(patsubst %,-I%,$(INCLUDE))
ALL_INCLUDES := $(shell for d in $(INCLUDE); do find -L $$d -type f -name \*.h -print 2>/dev/null ; done) $(shell for d in $(INCLUDE); do find -L $$d -type f -name \*.hpp -print 2>/dev/null ; done)
.SECONDEXPANSION:

.PRECIOUS:bin/%
bin/%: util/%.cc $(ALL_INCLUDES) $$(DEPS)
	@echo $(MSG)
	@mkdir -p $(@D)
	@$(CXX) -o $@ $< $(shell echo $(COMMON_FLAGS) $(CUSTOM_FLAGS)) 

%: util/%.cc bin/% 
	@echo $(MSG)
	@true

clean:
	@echo -n "Cleaning... "
	@rm -rf tmp bin/*
	@echo "Done!"
