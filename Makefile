LDFLAGS=-lncurses

all: wry

test: wry
	./wry

clean:
	rm wry

