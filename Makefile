LDFLAGS=-lncurses

all: wry 
test: wry
	./wry

#wry-standalone:
#	CC -W -Wall  wry.c -o wry

clean:
	rm wry

