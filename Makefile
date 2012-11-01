LDFLAGS=-lncurses

all: test

test: wry
	./wry declshort.txt

clean:
	find . -depth 1 -type f -perm +0111 -print -exec rm '{}' ';'

