NAME := test_gifv01


GIF_V01_SRC  += $(sort $(basename $(wildcard res/gifv01/*.*)))
HEADERS      += $(patsubst %, tmp/inc/%.h, $(GIF_V01_SRC))
SOURCES_C    += $(wildcard tmp/res/gifv01/*.c)

tmp/res/gifv01/%.c: tmp/inc/res/gifv01/%.h
	@echo -n 
	
.PRECIOUS: res/gifv01/%/img000001.png
res/gifv01/%/img000001.png: res/gifv01/%.*
	@echo $(MSG) $<
	@sdcc_msx/util/extractGif.sh $< 200

.PRECIOUS: tmp/inc/res/gifv01/%.h
tmp/inc/res/gifv01/%.h: res/gifv01/%/img000001.png tmp/sdcc_msx/util/mkGif_v01
	@echo $(MSG)
	@mkdir -p tmp/$(<D)
	@mkdir -p tmp/inc/$(<D)
	@tmp/sdcc_msx/util/mkGif_v01 $*
	@echo "You may want to do make again, as we generated new source files." && false
	
include sdcc_msx/Makefile.in
