all: src/main.c
	gcc -o build/trackoverflow src/main.c

r: all
	build/trackoverflow

w: src/main.c
	x86_64-w64-mingw32-gcc -o build/trackoverflow.exe src/main.c

rw: w
	wine build/trackoverflow.exe
