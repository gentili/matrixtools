
LIBS=	-ldisplay -lpthread -lncurses -ltinfo

LIBDIRS=	-L.

INCDIRS=	

# -----------------------
ALL_TARGETS= libdisplay.a test test2 droptest reptest attrtest
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =	Screen.o MatrixColumn.o

libdisplay.a: $(OFILES)
	ar vr $@ $^

test: libdisplay.a test.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

test2: libdisplay.a test2.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

droptest: libdisplay.a droptest.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

reptest: libdisplay.a reptest.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)

attrtest: libdisplay.a attrtest.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
