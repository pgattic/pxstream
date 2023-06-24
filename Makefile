
all:
	gcc -o pxs decoder.c -lglut -lGL -lGLU -lm
clean:
	rm -f *.o decoder
.PHONY: all clean

