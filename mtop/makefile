
LIBS= -ldisplay -pthread -lpthread -lprocps -lncurses -ltinfo

LIBDIRS= -L../display

INCDIRS= -I../display

# -----------------------
ALL_TARGETS= mtop
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =

mtop: main.o MatrixTop.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
