GAMES := $(patsubst game/%,%,$(wildcard game/*) )

all: $(GAMES)
	@echo $(GAMES)

%: game/%/Makefile
	@make --no-print-directory -C game/$@

msx-%: %
	@make --no-print-directory -C game/$@ msx

run-%: %
	@make --no-print-directory -C game/$@ run
