# Warning - Big rework in progress... currently the PSG / midi processor is working, as well as the banking capable linker. See game/test_audio for an example.

go to game/test_audio and do:

make msx
(press arrow keys to play music)

Requires: gcc, opencv, cproto and make, among other packages. I'd appreciate feedback regarding libraries. 





The below information is outdated.

# sdcc-msx



Environment for mixed development linux/msx in C and assembler.

Our idea is to develop the core engine of a msx game in C within linux, allowing rapid prototyping thanks to gcc, gdb, printfs, etc...

To be able to compile a msx looking game in linux, we include a minimal implementation of the tms9918a for linux, and we might add other devices as needed.

Our idea is to share most of the code between linux and msx targets.
While the parts unique to msx and linux should be placed in their respective folders.

The linux port is compiled with gcc, while the msx port is compiled/assembled with sdcc. 

In the doc folder we include documentation from several sources, as well as includes and startup codes for different configurations.

Bests, and good look msx1ing!


## Makes
```
make hello <- builds the hello-world example for all platforms
make linux-hello <- builds the linux version of the hello world
make run-linux-hello <- builds the linux version of the hello world and executes it
make gdb-linux-hello <- builds the linux version of the hello world and runs it through gdb
```

## Extensions
```
.c <- compiled for all targets
.s <- assembled using sdasz80
```


## Folders
```
bin/target/exec  ;Here go the binaries and the rom
doc              ;Here we have handy documents for the development of the MSX
lib/       
lib/msx

game/name/src       ;Common source code (.c and .h) for both pc and msx
game/name/res       ;Resources for a game
	
tmp        ;Folder for the temporal files
```

## Commands
```
make bin/skel.rom ;Build only the msx rom
make bin/skel.pc  ;Build only the linux executable
make all          ;Build both linux and msx versions of the project
make msx1         ;Build and launch the rom with openmsx for the msx1
make run          ;Build and launch the executable for linux
```

## Based on the work by:
- [Sandy Pleyte](https://github.com/sndpl/skeleton-sdcc-msx)
- [NataliaPC](https://github.com/nataliapc/template-sdcc-msx)
- [Alberto de Hoyo Nebot](http://albertodehoyonebot.blogspot.com.es/p/how-to-create-msx-roms-with-sdcc.html])
- [Avelino Herrera Morales](http://msx.atlantes.org/index_en.html])
- [Laurens Holst](http://map.grauw.nl/)

## Selected fonts from:
- [Electrickeet](http://electrickeet.com/line-itfont.html)
