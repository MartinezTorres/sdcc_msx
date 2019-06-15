DEMOS := $(patsubst demos/%,%,$(wildcard demos/*) )

all: $(DEMOS)
	@echo $(DEMOS)

%: demos/%/Makefile
	@make --no-print-directory -C demos/$@

msx-%: %
	@make --no-print-directory -C demos/$< msx

run-%: %
	@make --no-print-directory -C demos/$< run
