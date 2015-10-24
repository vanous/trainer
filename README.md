# trainer
To build trainer, you need:

libartnet - https://github.com/OpenLightingProject/libartnet

Build trainer:

gcc triner.c -lncurses -lartnet -o trainer

To build for windows, compile libartnet and trainer 
x86_64-w64-mingw32-gcc -v simple.c -o trainer.exe -std=gnu11 -I/your path to ncurses/include/ncursesw/ -I/your path to libartnet/libartnet/artnet/  /your path to libartnet/libartnet/artnet/.libs/libartnet.dll.a /your path to ncurses/lib/libncursesw.a


You can use precompiled 64bit ncurses:
ftp://invisible-island.net/ncurses/win32/mingw64-20151010.zip


