
LIBS=	-lncurses -lutility -ldisplay -lpthread

LIBDIRS=	-L../utility -L.

INCDIRS=	-I../utility

# -----------------------
ALL_TARGETS= libdisplay.a test drop
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =	Screen.o MatrixColumn.o

libdisplay.a: $(OFILES)
	ar vr $@ $^

test: libdisplay.a test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

drop: libdisplay.a drop.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
