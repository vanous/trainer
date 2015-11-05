#not so elegant but makes the job

CCFLAGS += -O2

LIBS += -lncurses -lartnet

trainer:
	$(CC) trainer.c $(CCFLAGS) $(LIBS) -o $@

win32:
	i686-w64-mingw32-gcc -O2 -v trainer.c -o trainer_w32.exe -std=gnu11 -static -mwindows -I /home/vanous/bin/projects/c/win/win32/include/artnet/ -I /home/vanous/bin/projects/c/win/win32/include/ncurses/ /home/vanous/bin/projects/c/win/win32/lib/libartnet.dll.a  /home/vanous/bin/projects/c/win/win32/lib/libncurses.a

win64:
	x86_64-w64-mingw32-gcc -O2 -v trainer.c -o trainer_w64.exe -std=gnu11 -static -mwindows -I /home/vanous/bin/projects/c/win/win64/include/artnet/ -I /home/vanous/bin/projects/c/win/win64/include/ncurses/ /home/vanous/bin/projects/c/win/win64/lib/libartnet.dll.a /home/vanous/bin/projects/c/win/win64/lib/libncurses.a

clean:
	rm -f .*.o .*.d trainer trainer_w64.exe trainer_w32.exe

all: clean trainer win32 win64
