
LIBS=	-lncurses -lutility

LIBDIRS=	-L../utility

CFLAGS=	-Wall -g

CXX=	g++

REQOBJS=

all: $(REQOBJS) test

test: $(REQOBJS) test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(LIBDIRS) $(LIBS)

clean:
	rm -f *.o test
