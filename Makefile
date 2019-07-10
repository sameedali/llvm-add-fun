.PHONY=all

all:
	./run.sh

run:
	clang -c -emit-llvm test/main.c
	mv main.bc test.bc
	opt -load ./build/skeleton/libSkeletonPass.so -skeletonpass <./test.bc> ./out.bc
	opt -O3 < out.bc > out.bc.
	llc -filetype=obj out.bc
	gcc -no-pie out.o 
