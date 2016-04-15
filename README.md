# trainer

The calibRAtIoN submittER

Tool to quickly upload LED calibration values into Robin DL/DL4 units via RUNIT over DMX (Art-Net calibration not supported by the fixtures).

##Binary releases:
https://github.com/vanous/trainer/releases

##Quick howto:

* Set unit to DMX address 1, DMX Mode 1 and connect to it via RUNIT (the fixtures do not support calibration via Art-Net).
* Go to menu → Service → Calibrations → Calibrate Colors.
* Run trainer
* Select type of unit by pressing c.
* Run test (t) to confirm DMX reception.
* In trainer, fill in provided RGBW calibration values.
* Then run calibration by pressing r.
* Wait for this to finish.
* Press OK on the Calibrate Colors menu screen.
* Profit.

Obligatory screenshot:

![trainer](https://cloud.githubusercontent.com/assets/3680926/11918539/19d9e56c-a736-11e5-8eac-7ffb18d246d8.gif)

It requires libartnet but in reality, calibration can only happen via RUI or RUNIT WTX. One could, however, use another Robin fixture for Art-Net to XLR DMX translation.

Serial library for windows comes from: https://github.com/waynix/SPinGW

To build trainer, you need libartnet, ncurses. Simply configure, make, make install these libraries.

##Then build trainer:

gcc -O2 trainer.c -lncurses -lartnet  -o trainer


###Build on Linux for win32 and win64:

get ncurses: ftp://invisible-island.net/ncurses/ncurses-6.0.tar.gz

Configure:

###For win32:

./configure --enable-term-driver --enable-sp-funcs --host=i686-w64-mingw32 --prefix=/home/vanous/bin/projects/c/win/win32/

###For win64:

/configure --enable-term-driver --enable-sp-funcs --host=x86_64-w64-mingw32 --prefix=/home/vanous/bin/projects/c/win/win64/

Buid:

make; make install

get libartnet: 


https://github.com/OpenLightingProject/libartnet

Configure:

For win32:

./configure ac_cv_func_malloc_0_nonnull=yes  ac_cv_func_realloc_0_nonnull=yes --host=i686-w64-mingw32 --prefix=/home/vanous/bin/projects/c/win/win32/

For win64:

./configure ac_cv_func_malloc_0_nonnull=yes  ac_cv_func_realloc_0_nonnull=yes --host=x86_64-w64-mingw32 --prefix=/home/vanous/bin/projects/c/win/win64/


Build:

make; make install


Build trainer:

For win32:

i686-w64-mingw32-gcc -O2 -v trainer.c -o trainer_w32.exe -std=gnu11 -I /home/vanous/bin/projects/c/win/win32/include/artnet/ -I /home/vanous/bin/projects/c/win/win32/include/ncurses/ /home/vanous/bin/projects/c/win/win32/lib/libartnet.a  /home/vanous/bin/projects/c/win/win32/lib/libncurses.a

For win64:

x86_64-w64-mingw32-gcc -O2 -v trainer.c -o trainer_w64.exe -std=gnu11 -I /home/vanous/bin/projects/c/win/win64/include/artnet/ -I /home/vanous/bin/projects/c/win/win64/include/ncurses/ /home/vanous/bin/projects/c/win/win64/lib/libartnet.a /home/vanous/bin/projects/c/win/win64/lib/libncurses.a
