build:
	clang -Wpedantic -Wall -std=c11 -O3 src/*.c `pkg-config SDL3 --cflags --libs` -o fdg
run:
	./fdg
