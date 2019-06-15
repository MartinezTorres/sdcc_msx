@echo off
IF %2.==. GOTO NOOPT
echo Assembling %1.asm
goto %2
:NOOPT
echo Error! build.bat should be called from make.bat, please run make.bat
goto no_error
:tniasm
tniasm ..\tools\tniasm-compat %1.asm ..\derived\bin\cbios_%1.rom ..\derived\lst\cbios_%1.sym 1> NUL
if errorlevel 1 goto error
GOTO no_error
:pasmo
pasmo -d %1.asm ..\derived\bin\cbios_%1.rom > ..\derived\lst\cbios_%1.sym
if errorlevel 1 goto error
goto no_error
:error
pause
:no_error
