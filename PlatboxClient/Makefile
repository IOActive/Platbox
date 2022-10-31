all:
	make clean
	clang -c -static -IDeltaFuzz/*.h -o DeltaFuzz.o DeltaFuzz/*.c -Iinc
	llvm-ar rc DeltaFuzz.a DeltaFuzz.o
	clang++ -static -O2 -Iinc/ -Iinc/smm/ -Iinc/amd/ -Iinc/intel/ -Iinc/linux -I../Platbox/linux/include/ -IDeltaFuzz/  -o platbox src/*.cpp src/smm/*.cpp src/amd/*.cpp src/intel/*.cpp DeltaFuzz.a

clean:
	rm -f *.o platbox DeltaFuzz.a
