# sdcc-msx
Environment for mixed development linux-msx in C and assembler.

Our idea is to develop the core engine of a msx game in C within linux, allowing rapid prototyping thanks to gcc, printfs, etc...

To be able to compile a msx looking game in linux, we include a minimal implementation of the tms9918a for linux.

The idea is to share most of the code between linux and msx (hence, shared code lies in the src/common folder). 
While the parts unique to msx and linux should be placed in their respective folders (src/msx and src/linux).

The linux port is compiled with gcc, while the msx port is compiled with sdcc. 

In the doc folder we include documentation from several sources, as well as includes and startup codes for different configurations.

The name of the project as well as the starting code and memory regions for the sdcc are configured on the Makefile.

Bests, and good look msx1ing!


## Folders
```
bin        ;Here go the binaries and the rom
doc        ;Here we have handy documents for the development of the MSX
src/common ;Common source code (.c and .h) for both pc and msx
src/msx    ;Source code (.c, .h, and .s) for the msx
src/linux  ;Source code (.c, .h) for the linux port
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
