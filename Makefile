all: src/main.c
	mkdir -p build
	gcc \
	-Wall \
	-Iminiaudio \
	src/main.c miniaudio/miniaudio.c \
	-o build/trackoverflow \
	-lraylib \
	-pthread -lm

run: all
	build/trackoverflow
