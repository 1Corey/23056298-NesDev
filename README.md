# NESDevTemplate
NES Dev Template Tutorial repo
There will be folders that you will have to install additional software in. 
Details are provided in readme.txt files in the relevant locations

To avoid tracking local or temporary files, or source code/executables these
have been added to the .gitignore file, but to keep the appropriate folders
in the remote repo the readme.txt files have been added to tracked files using:

 > git add -f */readme.txt

 > git add -f */BUILD/readme.txt
 
 > git add -f NESHugg/neslib/*.o as Hugg uses a partially compiled library in neslib

If you download this repo as a template for your own, the tools folders will
not form part of your own repo. If you fork, you will only see the readme.txt
files in the tools folders.

Software needed for NES development:

This should already be installed but if not, or you are setting up your own machine, download it here:

https://code.visualstudio.com/download

Visual Studio Code add-ons:
               Alchemy65
               GitHub Pull Requests
               (plus others you find useful)

Mesen2 (NES emulator) from https://github.com/SourMesen/Mesen2
NOTE: Right now Alchemy65 only works with the original, so...

MesenX (old NES Emulator, compbatible with Alchemy65) from:
https://github.com/NovaSquirrel/Mesen-X which should be 11/9/2023

cc65 (a cross compiler for 6502) from:
https://cc65.github.io/
GitHub repo (for source and updates):
https://github.com/cc65/cc65 <- use this one

NEXXT, an updated version of NES Screen Tool is available at:
https://frankengraphics.itch.io/nexxt
NOTE: This is "pay what you want" so effectively FREE

FamiTracker from https://famitracker.org/

FamiStudio from https://famistudio.org/
Use the Windows Portible EXE option

# Resources

NES Programming Guide https://www.nesdev.org/wiki/Programming_guide

NESDoug Tutorials https://nesdoug.com/
NOTE: This template is based upon NES Doug's extention of Shiru's
 original C NES prgramming site:
 https://shiru.untergrund.net/articles/programming_nes_games_in_c.htm