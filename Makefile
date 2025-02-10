build:
	clang -Wall -O3 -std=c99 src/*.c `pkg-config raylib --cflags --libs` -o fdg

run:
	./fdg
