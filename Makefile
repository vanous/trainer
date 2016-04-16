#not so elegant but makes the job
#take screenshot gif: byzanz-record -x 1 -y 131 -w 774 -h 525 --delay=2 -c -d 16 /tmp/trainer.gif

CCFLAGS += -O2

LIBS += -lncurses -lartnet

make_dir:
	mkdir -p ./bin
	mkdir -p ./release

trainer_linux: make_dir
	$(CC) trainer.c $(CCFLAGS) $(LIBS) -o ./bin/$@

trainer_linux_static: make_dir
	$(CC) trainer.c $(CCFLAGS) -lncurses /lib/libartnet.a -o ./bin/$@

win32: make_dir
	i686-w64-mingw32-gcc -O2 -v trainer.c -o ./bin/trainer_w32.exe -std=gnu11 -I /home/vanous/bin/projects/c/win/win32/include/artnet/ -I /home/vanous/bin/projects/c/win/win32/include/ncurses/ /home/vanous/bin/projects/c/win/win32/lib/libartnet.dll.a  /home/vanous/bin/projects/c/win/win32/lib/libncurses.a

win64: make_dir
	x86_64-w64-mingw32-gcc -O2 -v trainer.c -o ./bin/trainer_w64.exe -std=gnu11 -I /home/vanous/bin/projects/c/win/win64/include/artnet/ -I /home/vanous/bin/projects/c/win/win64/include/ncurses/ /home/vanous/bin/projects/c/win/win64/lib/libartnet.dll.a /home/vanous/bin/projects/c/win/win64/lib/libncurses.a

osx: make_dir
	gcc trainer.c -lncurses -I../libs/include/artnet/ ../libs/lib/libartnet.a -o ./bin/trainer_osx

clean:
	rm -f .*.o .*.d 
	rm -rf ./bin
	rm -rf ./release


all: clean trainer_linux trainer_linux_static win32 win64 

package_osx: osx
	gzip -c ./bin/trainer_osx > ./release/trainer_osx.gz
	scp ./release/trainer_osx.gz vanous@192.168.3.37:/home/vanous/bin/projects/c/trainer/release


package_all: all
	gzip < ./bin/trainer_linux > ./release/trainer_linux.gz
	gzip < ./bin/trainer_linux_static > ./release/trainer_linux_static.gz
	zip --junk-paths ./release/trainer_w32.zip /home/vanous/bin/projects/c/win/win32/bin/libartnet-1.dll ./bin/trainer_w32.exe
	zip --junk-paths ./release/trainer_w64.zip /home/vanous/bin/projects/c/win/win64/bin/libartnet-1.dll ./bin/trainer_w64.exe
