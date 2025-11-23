CC=gcc
CFLAGS=-Wall -pedantic -O0 -g
LFLAGS=-Lraylib-5.5/src/ -l:libraylib.a -lm 
SOURCE=gfx.c
BINARY=gfx
DEPS=./raylib-5.5/src/libraylib.a
DEPSDIR=./raylib-5.5/src/
$(BINARY): $(SOURCE) $(DEPS)
	$(CC) $(SOURCE) $(CFLAGS) $(LFLAGS) -o $(BINARY)

$(DEPS):
	make -C $(DEPSDIR)
