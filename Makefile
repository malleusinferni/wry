LDFLAGS=-lncurses

all: wry
	./wry

clean:
	find . -depth 1 -type f -perm +0111 -print -exec rm '{}' ';'

