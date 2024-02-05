
all:
	gcc -o pxs decoder.c -lglut -lGL -lGLU

clean:
	rm pxs

install:
	cp pxs /usr/local/bin/

.PHONY: all clean install

