
all:
	gcc -o pxs decoder.c -lglut -lGL -lGLU
clean:
	rm -f *.o decoder
install:
	cp pxs /usr/bin/
.PHONY: all clean install

