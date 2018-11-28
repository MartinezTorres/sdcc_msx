# Select your assembler:
Z80_ASSEMBLER?=pasmo
#Z80_ASSEMBLER?=z80-as
#Z80_ASSEMBLER?=sjasm
#Z80_ASSEMBLER?=tniasm

PACKAGE_NAME:=cbios
VERSION:=$(shell cat version.txt)
PACKAGE_FULL:=$(PACKAGE_NAME)-$(VERSION)a

TITLE:="C-BIOS $(VERSION)      cbios.sf.net"
VERSION_FILE:=derived/asm/version.asm

ROMS:=main_msx1 main_msx1_eu main_msx1_jp main_msx1_br \
	main_msx2 main_msx2_eu main_msx2_jp main_msx2_br main_msx2+ \
	main_msx2+_eu main_msx2+_jp main_msx2+_br sub logo_msx1 logo_msx2 \
	logo_msx2+ music disk basic
ROMS_FULLPATH:=$(ROMS:%=derived/bin/cbios_%.rom)

# If needed override location of pasmo.
PASMO=pasmo

# Mark all logical targets as such.
.PHONY: all dist clean list_stub

all: $(ROMS_FULLPATH)

ifeq ($(Z80_ASSEMBLER),sjasm)
# Workaround for SjASM producing output file even if assembly failed.
.DELETE_ON_ERROR: $(ROMS_FULLPATH)
endif

ifeq ($(Z80_ASSEMBLER),z80-as)
# Z80-as can only place code into relocatable sections, we preprocess the sections and use
# z80-ld to produce the final .rom files
ASMDIR=derived/asm
SEDSCR = -e 's/ds[ \t]\+\(\$$[0-9a-fA-F]\+[ \t]*-[ \t]*\$$\)/ds\tABS0+\1/' \
	-e 's/[ \t]\+org[ \t]\+\(\$$[0-9a-fA-F]\+\|[0-9]\+\)/\
	;\0\nABS0: equ \$$-\1\n;;;-Ttext \1 --entry \1/'  \
	-e 's:\.\./derived/asm/::'
else
ASMDIR=src
endif

$(VERSION_FILE): version.txt
	@echo "Creating: $@"
	@mkdir -p $(@D)
	@echo '  db $(TITLE)' > $@

$(ROMS_FULLPATH): derived/bin/cbios_%.rom: vdep/%.asm
	@echo "Assembling: $(<:vdep/%=$(ASMDIR)/%)"
	@mkdir -p $(@D)
	@mkdir -p derived/lst
ifeq ($(Z80_ASSEMBLER),sjasm)
	@sjasm -iderived/asm -l $(<:vdep/%=src/%) $@ $(@:derived/bin/%.rom=derived/lst/%.lst)
endif
ifeq ($(Z80_ASSEMBLER),pasmo)
	@$(PASMO) -I src -I derived/asm $(<:vdep/%=src/%) \
		$@ $(@:derived/bin/%.rom=derived/lst/%.lst)
endif
ifeq ($(Z80_ASSEMBLER),tniasm)
	@cd src && tniasm ../tools/tniasm-compat $(<:vdep/%=%) ../$@ $(@:derived/bin/%.rom=derived/lst/%.sym)
endif
ifeq ($(Z80_ASSEMBLER),z80-as)
	@mkdir -p derived/obj
	@z80-as -I derived/asm -I src $(<:vdep/%=$(ASMDIR)/%) -Wall \
		-o $(@:derived/bin/%.rom=derived/obj/%.o) \
		-as=$(@:derived/bin/%.rom=derived/lst/%.lst)
	@z80-ld -n --oformat binary $(@:derived/bin/%.rom=derived/obj/%.o) `\
		grep "^;;;-Ttext" $(<:vdep/%=$(ASMDIR)/%) | \
		sed -e "s/;//g" -e 's/\\$$/0x/g'` -o $@
endif

ifeq ($(filter clean,$(MAKECMDGOALS)),)

# Include main dependency files.
-include $(ROMS:%=derived/dep/%.dep)

GENERATED_FILES:=$(VERSION_FILE)
GENERATED_DEPS:=$(GENERATED_FILES:derived/asm/%.asm=derived/dep/%.dep)

# Note: The dependency generation code is here twice.
#       That's not great, but the alternatives are worse.

$(GENERATED_DEPS): derived/dep/%.dep: derived/asm/%.asm
	@echo "Depending: $<"
	@mkdir -p $(@D)
	@echo "INCLUDES:=" > $@
	@sed -n '/include/s/^[\t ]*include[\t ]*"\(\.\.\/derived\/asm\/\)\{0,1\}\(.*\)".*$$/INCLUDES+=\2/p' \
		< $< >> $@
	@echo "INCBINS:=" >> $@
	@sed -n '/incbin/s/^[\t ]*incbin[\t ]*"\(\.\.\/derived\/asm\/\)\{0,1\}\(.*\)".*$$/INCBINS+=\2/p' \
		< $< >> $@
	@echo ".SECONDARY: $(<:derived/asm/%=vdep/%)" >> $@
	@echo "$(<:derived/asm/%=vdep/%): $<" >> $@
	@echo "$(<:derived/asm/%=vdep/%): \$$(INCLUDES:%=vdep/%) \$$(INCBINS:%=src/%)" >> $@
	@echo "ifneq (\$$(INCLUDES),)" >> $@
	@echo "-include \$$(INCLUDES:%.asm=derived/dep/%.dep)" >> $@
	@echo "endif" >> $@

derived/dep/%.dep: src/%.asm
	@echo "Depending: $<"
	@mkdir -p $(@D)
	@echo "INCLUDES:=" > $@
	@sed -n '/include/s/^[\t ]*include[\t ]*"\(\.\.\/derived\/asm\/\)\{0,1\}\(.*\)".*$$/INCLUDES+=\2/p' \
		< $< >> $@
	@echo "INCBINS:=" >> $@
	@sed -n '/incbin/s/^[\t ]*incbin[\t ]*"\(\.\.\/derived\/asm\/\)\{0,1\}\(.*\)".*$$/INCBINS+=\2/p' \
		< $< >> $@
	@echo ".SECONDARY: $(<:src/%=vdep/%)" >> $@
	@echo "$(<:src/%=vdep/%): $(<:src/%=$(ASMDIR)/%)" >> $@
	@echo "$(<:src/%=vdep/%): \$$(INCLUDES:%=vdep/%) \$$(INCBINS:%=src/%)" >> $@
	@echo "ifneq (\$$(INCLUDES),)" >> $@
	@echo "-include \$$(INCLUDES:%.asm=derived/dep/%.dep)" >> $@
	@echo "endif" >> $@
else
# Clean build -> treat all dependencies as outdated.
.PHONY: $(ROMS:%=vdep/%.asm)
endif

ifneq ($(ASMDIR),src)
$(ASMDIR)/%.asm: src/%.asm
	@echo "Preprocessing: $<"
	@mkdir -p $(@D)
	@sed $(SEDSCR) \
		< $< > $@
endif

clean:
	@rm -rf derived

dist: all
	@rm -rf derived/dist
	@mkdir -p derived/dist/$(PACKAGE_FULL)
	@cp Makefile version.txt *.bat derived/dist/$(PACKAGE_FULL)
	@cp -R configs doc src tools derived/dist/$(PACKAGE_FULL)
	@SCRIPT=derived/dist/inject_sha1.sed \
		&& shasum -a1 $(ROMS_FULLPATH) | sed -nf tools/subst_sha1.sed > $$SCRIPT \
		&& sed -i'~' -f $$SCRIPT \
			derived/dist/$(PACKAGE_FULL)/configs/openMSX/*.xml \
		&& rm $$SCRIPT derived/dist/$(PACKAGE_FULL)/configs/openMSX/*~
	@mkdir -p derived/dist/$(PACKAGE_FULL)/roms
	@cp $(ROMS_FULLPATH) derived/dist/$(PACKAGE_FULL)/roms
	@cd derived/dist ; zip -9 -X -D -r $(PACKAGE_FULL).zip $(PACKAGE_FULL)

list_stub:
	cd src && grep -n _text *.asm | grep ',0$$' | awk '{print $$1}' | sed -e 's/_text://'
