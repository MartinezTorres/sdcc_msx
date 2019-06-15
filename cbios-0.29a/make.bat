@echo off
mkdir derived\bin 2> NUL
mkdir derived\lst 2> NUL
IF %1.==. GOTO NOOPT

rem -- TODO: get the SVN version of Changelog into version.asm
mkdir derived\asm 2> NUL
del derived\asm\version.asm 2> NUL
echo db "C-BIOS "> derived\asm\version.asm
echo incbin "../version.txt">> derived\asm\version.asm
echo db "      cbios.sf.net">> derived\asm\version.asm

cd src
IF %1.==pasmo. GOTO ASM
IF %1.==tniasm. GOTO ASM
:NOOPT
echo usage make.bat [pasmo or tniasm]
GOTO QUIT
:ASM
FOR %%i IN (main_msx1;main_msx2;main_msx2+) DO call ..\build %%i %1
FOR %%i IN (basic;sub;music;disk;logo_msx1;logo_msx2;logo_msx2+) DO call ..\build %%i %1
FOR %%i IN (main_msx1_eu;main_msx2_eu;main_msx2+_eu) DO call ..\build %%i %1
FOR %%i IN (main_msx1_jp;main_msx2_jp;main_msx2+_jp) DO call ..\build %%i %1
FOR %%i IN (main_msx1_br;main_msx2_br;main_msx2+_br) DO call ..\build %%i %1
:END
cd ..
:QUIT
