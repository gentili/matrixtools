
LIBS=	-lncurses

CFLAGS=	-Wall -g

CXX=	g++

REQOBJS=

all: $(REQOBJS) test

test: $(REQOBJS) test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(LIBS)

clean:
	rm -f *.o test
