
LIBS=	-lncurses -lutility -ldisplay

LIBDIRS=	-L../utility -L.

INCDIRS=	-I../utility

# -----------------------
ALL_TARGETS= libdisplay.a test
include ../makefile.in
# -----------------------

OFILES =	screen.o

libdisplay.a: $(OFILES)
	ar vr $@ $^

test: libdisplay.a test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
