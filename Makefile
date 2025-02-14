build:
	clang -Wpedantic -Wall -std=c99 src/*.c `pkg-config SDL3 --cflags --libs` -o fdg
run:
	./fdg
