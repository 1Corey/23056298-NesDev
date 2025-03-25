@echo off

set name="climber_game"

set CC65_HOME=..\cc65\

set path=%path%;%CC65_HOME%\bin\

cl65 -c -g -C nes.cfg -t nes -Oirs -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib climber.c -o climber.o --add-source
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib bcd.c -o bcd.o
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib vrambuf.c -o vrambuf.o
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib chr_generic.s -o chr_generic.o
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib demosounds.s -o demosounds.o
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib famitone.s -o famitone.o
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib header.s -o header.o
cl65 -c -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib music_dangerstreets.s -o music_dangerstreets.o


setlocal ENABLEDELAYEDEXPANSION
set O_FILES=
for %%i in ( ./neslib/*.o ) do (
    set O_FILES=!O_FILES! ./neslib/%%i
)

: echo %O_FILES%

cl65 -o %name%.nes -C nes.cfg -t nes -Oisr -m %name%.map -Ln %name%.labels -vm --include-dir ./neslib bcd.o climber.o vrambuf.o chr_generic.o demosounds.o famitone.o header.o music_dangerstreets.o %O_FILES% -Wl --dbgfile -Wl %name%.dbg

del *.o

move /Y %name%.labels BUILD\
move /Y %name%.nes BUILD\ 
move /Y %name%.map BUILD\
move /Y %name%.dbg BUILD\