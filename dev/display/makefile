
LIBS=	-lncurses -lutility -ldisplay -lpthread

LIBDIRS=	-L../utility -L.

INCDIRS=	-I../utility

# -----------------------
ALL_TARGETS= libdisplay.a test
include ../makefile.in
# -----------------------

CFLAGS +=

OFILES =	Screen.o MatrixColumn.o

libdisplay.a: $(OFILES)
	ar vr $@ $^

test: libdisplay.a test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
