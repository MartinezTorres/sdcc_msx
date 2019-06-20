# sdcc-msx
Environment for mixed development linux/msx in C and assembler.

This environment compiles the same source code with two different targets: either native linux using gcc, or msx using sdcc.
When compiling for the MSX target the "MSX" macro gets defined, while the "LINUX" macro gets defined when
linux is the target. This allows us to specialize critical parts of the code, i.e., some MSX functions may be implemented in assembly.

The idea is to have a fast development environment for MSX software.
The linux target compiles faster and allows faster testing, and also allows us to use debuggers, printfs, etc... And because linux and msx targets share most of the code, solving the problems in the linux port will automatically solve them in the msx port.

To be able to compile a msx looking game in linux, we use SDL to simulate the keyboard and the tms9918a and the psg for linux, and will add other devices as needed.

## Unique properties

### Supported MSX targets and the K5-linker
This environment supports exclusively Megarom MSX cartridges with a Konami-5 mapper.
The use of mappers for "CODE" sections is not natively supported by SDCC, so we created a linker specific for Konamy-5 mappers, named k5link. 
This linker creates megaroms that should be mapped in the memory region between 0x4000 and 0xC000, 
and the area above 0xC000 is assumed to be used for RAM. 
BIOS is available @0x0000.

Sadly, the compiler does not produce enough information to the linker to solve the mapping between segments, so the mapping between segments must be determied by hand.
This is done in the source code as follows:


SDCC treats each C file as a module that will be included in the ROM if *any* of their symbols is used.

We have 4 pages, A, B, C, and D, and many many segments. 
The module that defines the *main* symbol will be in segment nº0, and be mapped at page A (0x4000) from boot.
If we use the declaration "USING_PAGE_D(module_name)", means that the referred module will be mapped at page D when required, but we are forced to map it on demand.

One can query the segment where a module is located by using the macro SEGMENT_TO_PAGE_D(module_name).
And it's typical (but not required) to restore the segment mapping when a function returns.
As usual, care must be taken to juggle between segments (check test_audio for examples).

### Audio players
We provide support for the PSG only, and we support two players.
The first one is the ayFx player, that is used to encode sound effects.
This player supports priorities, juggles between channels, and supports up to 8 simultaneous sounds. 

The second one is the ayr player, which is a custom player that we used to encode midi files. 
A generic midi-to-ayr encoder is included, but due to the limited nature of the PSG, tradeoffs must be taken,
and adapting the parameters of the midi-to-ayr converter to each midi file is encouraged.

I would love to have more advanced replayers (PT3, WYZ?), but as of now I only found assembler versions of those players, and I could not find good enough documentation to replicate the player in C (to compile it for linux). I'd appreciate collaborations!

### Video / Graphics 
I like the technique of swapping screen buffers at each interruption to provide more colors to the limited MSX1 palette.
This is used to show multicolor text in the hello world example, and is also used in the GIF_v01 demo that allows to play any video file at 12 /12.5 frames per second in full screen. 

## Organization of the sdcc

### sdcc_msx
```
demos/            ;Here are the included demos in this environment
doc/              ;Here we have handy documents for the development of the MSX
inc/              ;The includes corresponding to the different modules of the sdcc_msx
src/              ;The sources of the sdcc_msx
util/             ;Utilities (bash and cc files) required to build files (e.g., the k5-linker, mkMidi, mkFont, etc...)

Makefile.in       ;Makefile that must be included to use this library.
```

### project using sdcc_msx
```
inc/              ;headers, optional
out/linux/        ;output of the linux exec
out/msx/          ;output of the ROM
res/              ;resources used
res/midi/         ;midi files used in the ROM
res/ayfx/         ;ayfx sound files used in the ROM
res/fonts/        ;fonts used in the rom (in PNG)
res/gifv01/       ;video files used in the rom
src/              ;source files
tmp/              ;directory with all generated temporals (sources, headers, assembler, etc...)
util/             ;utilities specific to this project

sdcc_msx ->       ;link to the location of the sdcc_msx

Makefile          ;Makefile that must include sdcc_msx/Makefile.in
```


### Documented makes
```
make              ;builds the project for both platforms
make run          ;builds the project for linux and executes it 
make msx          ;builds the project for msx and executes it using openmsx
make gdb          ;builds the project on low optimization and debugs it
```

### Extensions
```
.c                ;compiled for all targets
.s                ;assembler file, compiled only for MSX using sdasz80
```

### Demos
```
hello_world       ;non trivial hello_world program with a custom font, not using the mapper.
test_audio        ;slightly custom build system, dynamically finds all midis and plays them. Extensive use of the mapper.
test_gifv01       ;really messy build system with abuse of mappers and assembler.
```

### Dependences
```
core:
  build_essentials
  sdcc
  cproto
  openmsx

gifv01:
  ffmpeg
  opencv
```

## Based on the work by:
- [Sandy Pleyte](https://github.com/sndpl/skeleton-sdcc-msx)
- [NataliaPC](https://github.com/nataliapc/template-sdcc-msx)
- [Alberto de Hoyo Nebot](http://albertodehoyonebot.blogspot.com.es/p/how-to-create-msx-roms-with-sdcc.html])
- [Avelino Herrera Morales](http://msx.atlantes.org/index_en.html])
- [Laurens Holst](http://map.grauw.nl/)

## Selected fonts from:
- [Electrickeet](http://electrickeet.com/line-itfont.html)
