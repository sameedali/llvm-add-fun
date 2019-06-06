cd build
make 
cd ..
clang-6.0 -Xclang -load -Xclang build/skeleton/libSkeletonPass.so test/main.c
