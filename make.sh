pushd raylib-5.5/src
make
popd
cc gfx.c -Wall -pedantic -Lraylib-5.5/src/ -l:libraylib.a -lm -O3 -o gfx
