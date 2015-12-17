#not so elegant but makes the job

CCFLAGS += -O2

LIBS += -lncurses -lartnet

make_dir:
	mkdir ./bin
	mkdir ./release

trainer: make_dir
	$(CC) trainer.c $(CCFLAGS) $(LIBS) -o ./bin/$@

trainer-static: make_dir
	$(CC) trainer.c $(CCFLAGS) -lncurses /lib/libartnet.a -o ./bin/$@

win32: make_dir
	i686-w64-mingw32-gcc -O2 -v trainer.c -o ./bin/trainer_w32.exe -std=gnu11 -I /home/vanous/bin/projects/c/win/win32/include/artnet/ -I /home/vanous/bin/projects/c/win/win32/include/ncurses/ /home/vanous/bin/projects/c/win/win32/lib/libartnet.dll.a  /home/vanous/bin/projects/c/win/win32/lib/libncurses.a

win64: make_dir
	x86_64-w64-mingw32-gcc -O2 -v trainer.c -o ./bin/trainer_w64.exe -std=gnu11 -I /home/vanous/bin/projects/c/win/win64/include/artnet/ -I /home/vanous/bin/projects/c/win/win64/include/ncurses/ /home/vanous/bin/projects/c/win/win64/lib/libartnet.dll.a /home/vanous/bin/projects/c/win/win64/lib/libncurses.a

osx: make_dir
	gcc trainer.c -lncurses -I../libs/include/artnet/ ../libs/lib/libartnet.a -o ./bin/trainer-osx

clean:
	rm -f .*.o .*.d 
	rm -rf ./bin
	rm -rf ./release


all: clean trainer trainer-static win32 win64 

package_osx:
	gzip ./bin/trainer.osx > ./release/trainer-osx.gz

package_all: all
	gzip < ./bin/trainer > ./release/trainer.gz
	gzip < ./bin/trainer-static > ./release/trainer-static.gz
	zip --junk-paths ./release/trainer-w32.zip /home/vanous/bin/projects/c/win/win32/bin/libartnet-1.dll ./bin/trainer_w32.exe
	zip --junk-paths ./release/trainer-w64.zip /home/vanous/bin/projects/c/win/win64/bin/libartnet-1.dll ./bin/trainer_w64.exe
