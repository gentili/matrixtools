
LIBS=	-lncurses -lutility -ldisplay -lpthread

LIBDIRS=	-L../utility -L.

INCDIRS=	-I../utility

# -----------------------
ALL_TARGETS= libdisplay.a test droptest reptest
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =	Screen.o MatrixColumn.o

libdisplay.a: $(OFILES)
	ar vr $@ $^

test: libdisplay.a test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

droptest: libdisplay.a droptest.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

reptest: libdisplay.a reptest.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
